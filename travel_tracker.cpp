#include "travel_tracker.h"

using namespace std;

using GraphPtr = shared_ptr<Graph::DirectedWeightedGraph<Weight>>;
using RouterPtr = shared_ptr<Graph::Router<Weight>>;

TravelTracker::TravelTracker(istream& in, ostream& out) : out(out) {
	doc = make_unique<Json::Document>(Json::Load(in));
	requests = doc->GetRoot().AsMap();
	queary_pos = begin(requests);
}

Svg::Color GetColor(const Json::Node& info) {
	Svg::Color color;
	try {
		color = info.AsString();
	}
	catch (...) {
		const auto arr = info.AsArray();
		if (arr.size() == 3) {
			color = Svg::Rgb{
				  static_cast<uint8_t>(arr[0].AsDouble()),
				  static_cast<uint8_t>(arr[1].AsDouble()),
				  static_cast<uint8_t>(arr[2].AsDouble())
			};
		}
		else {
			color = Svg::Rgba{
				  static_cast<uint8_t>(arr[0].AsDouble()),
				  static_cast<uint8_t>(arr[1].AsDouble()),
				  static_cast<uint8_t>(arr[2].AsDouble()),
				  arr[3].AsDouble()
			};
		}
	}
	return color;
}


Reader::Reader(RouteDatabase& data) : data_(data) {}

void Reader::ReadRoutingSettings(const map<string, Json::Node>& info) {
	data_.SetRouteInfo(RouteInfo({
		info.at("bus_wait_time").AsDouble(), info.at("bus_velocity").AsDouble() }));
}

void Reader::ReadRenderSettings(const map<string, Json::Node>& info) {
	MapInfo map_info;
	map_info.width = info.at("width").AsDouble();
	map_info.height = info.at("height").AsDouble();
	map_info.padding = info.at("padding").AsDouble();
	map_info.line_width = info.at("line_width").AsDouble();
	map_info.stop_label_font_size = info.at("stop_label_font_size").AsDouble();
	map_info.stop_radius = info.at("stop_radius").AsDouble();
	map_info.underlayer_width = info.at("underlayer_width").AsDouble();
	auto stop_label_offset = info.at("stop_label_offset").AsArray();
	map_info.stop_label_offset = {
		stop_label_offset[0].AsDouble(), stop_label_offset[1].AsDouble() };
	map_info.underlayer_color = GetColor(info.at("underlayer_color"));
	auto colors = info.at("color_palette").AsArray();
	for (auto& color : colors) {
		map_info.color_palette.push_back(GetColor(color));
	}
	data_.SetMapInfo(move(map_info));
}

void Reader::ReadStopInfo(const map<string, Json::Node>& info) {
	StopInfo stop_info;
	Extremes& ex = data_.GetExtremes();
	stop_info.id = data_.GetStopIdByName(info.at("name").AsString());
	stop_info.name = info.at("name").AsString();
	stop_info.latitude = info.at("latitude").AsDouble(); //*PI / 180;
	if (stop_info.latitude > ex.max_lat) {
		ex.max_lat = stop_info.latitude;
	}
	else if (ex.min_lat == -1 || stop_info.latitude < ex.min_lat) {
		ex.min_lat = stop_info.latitude;
	}
	stop_info.longtitude = info.at("longitude").AsDouble(); //*PI / 180;
	if (stop_info.longtitude > ex.max_lon) {
		ex.max_lon = stop_info.longtitude;
	}
	else if (ex.min_lon == -1 || stop_info.longtitude < ex.min_lon) {
		ex.min_lon = stop_info.longtitude;
	}
	for (const auto& dis : info.at("road_distances").AsMap()) {
		StopId id = data_.GetStopIdByName(dis.first);
		stop_info.distance.insert({ dis.first, StopInternalData({ id, (int)dis.second.AsDouble() }) });
	}
	data_.SetStopInfo(move(stop_info));
}

void Reader::ReadBusInfo(const map<string, Json::Node>& info) {
	BusInfo bus_info;
	bus_info.name = info.at("name").AsString();
	bus_info.circle = info.at("is_roundtrip").AsBool();
	for (const auto& stop : info.at("stops").AsArray()) {
		bus_info.stops.push_back(stop.AsString());
		bus_info.unic_stops.insert(stop.AsString());
	}
	data_.SetBusInfo(move(bus_info));
}

void Reader::ReadBaseRequests(const vector<Json::Node>& requests) {
	for (auto& block : requests) {
		auto info = block.AsMap();
		if (info["type"].AsString() == "Stop") {
			ReadStopInfo(info);
		}
		else if (info["type"].AsString() == "Bus") {
			ReadBusInfo(info);
		}
	}
}
	

TravelTracker& TravelTracker::ReadDataFromJson(RouteDatabase& data) {
	Reader reader(data);
	for (auto request = requests.begin(); request != end(requests); ++request) {
		if (request->first == "routing_settings") {
			reader.ReadRoutingSettings(request->second.AsMap());
		}
		if (request->first == "render_settings") {
			reader.ReadRenderSettings(request->second.AsMap());
		}
		if (request->first == "base_requests") {
			reader.ReadBaseRequests(request->second.AsArray());
		}
		else if (request->first == "stat_requests") {
			queary_pos = request;
			break;
		}
	}
	return *this;
}

TravelTracker& TravelTracker::QueryProcessing(RouteDatabase& data) {
	Map map(data);
	auto graph = map.BuildMap();
	Graph::Router<Weight> router(graph);

	GraphPtr graph_ptr = make_unique<Map::InternalGraph>(graph);
	RouterPtr router_ptr = make_unique<Graph::Router<Weight>>(router);

	out << "[\n";
	for (auto request = queary_pos; request != end(requests); ++request) {
		size_t size = request->second.AsArray().size();
		for (size_t i = 0; i < size; ++i) {
			auto info = request->second.AsArray()[i].AsMap();
			if (info["type"].AsString() == "Stop") {
				QueryStop stop(data, info["name"].AsString(), info["id"].AsDouble(), out);
				stop.Procces().GetResult();
				if (i != size - 1)
					out << ",\n";
			}
			else if (info["type"].AsString() == "Bus") {
				QueryBus bus(data, info["name"].AsString(), info["id"].AsDouble(), out);
				bus.Procces().GetResult();
				if (i != size - 1)
					out << ",\n";
			}
			else if (info["type"].AsString() == "Route") {
				auto from = data.GetStopIdByName(info["from"].AsString());
				auto to = data.GetStopIdByName(info["to"].AsString());
				auto id = info["id"].AsDouble();
				QueryRoute route(data, map, graph_ptr, router_ptr, from, to, id, out);
				route.Procces().GetResult();
				if (i != size - 1)
					out << ",\n";
			}
			else if (info["type"].AsString() == "Map") {
				QueryMap map(data, info["id"].AsDouble(), out);
				map.Procces().GetResult();
				if (i != size - 1)
					out << ",\n";
			}
		}
	}

	out << "\n]";
	return *this;
}

void TravelManager(std::istream& in, ostream& out) {
	RouteDatabase data;
	TravelTracker travel(in, out);
	travel.ReadDataFromJson(data).QueryProcessing(data);
}