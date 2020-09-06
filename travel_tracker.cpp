#include "travel_tracker.h"

using namespace std;

using GraphPtr = shared_ptr<Graph::DirectedWeightedGraph<Weight>>;
using RouterPtr = shared_ptr<Graph::Router<Weight>>;

TravelTracker::TravelTracker(istream& in, ostream& out) : out(out) {
	doc = make_unique<Json::Document>(Json::Load(in));
	requests = move(doc->GetRoot().AsMap());
	queary_pos = begin(requests);
}

TravelTracker& TravelTracker::ReadDatabaseFromJson(RouteDatabase& data) {
	for (auto it = requests.begin(); it != end(requests); ++it) {
		auto request = *it;
		if (request.first == "routing_settings") {
			auto route_info = request.second.AsMap();
			data.SetRouteInfo(RouteInfo({ route_info["bus_wait_time"].AsDouble(), route_info["bus_velocity"].AsDouble() }));
		}
		if (request.first == "base_requests") {
			for (auto& block : request.second.AsArray()) {
				auto info = block.AsMap();
				if (info["type"].AsString() == "Stop") {
					StopInfo stop_info;
					stop_info.id = data.GetStopIdByName(info["name"].AsString());
					stop_info.name = info["name"].AsString();
					stop_info.latitude = info["latitude"].AsDouble() * PI / 180;
					stop_info.longtitude = info["longitude"].AsDouble() * PI / 180;
					for (const auto& dis : info["road_distances"].AsMap()) {
						StopId id = data.GetStopIdByName(dis.first);
						stop_info.distance.insert({ dis.first, StopInternalData({ id, (int)dis.second.AsDouble() }) });
					}
					data.SetStopInfo(move(stop_info));
					++stop_id;
				}
				else if (info["type"].AsString() == "Bus") {
					BusInfo bus_info;
					bus_info.name = info["name"].AsString();
					bus_info.circle = info["is_roundtrip"].AsBool();
					for (const auto& stop : info["stops"].AsArray()) {
						bus_info.stops.push_back(stop.AsString());
						bus_info.unic_stops.insert(stop.AsString());
					}
					data.SetBusInfo(move(bus_info));
				}
			}
		}
		else if (request.first == "stat_requests") {
			queary_pos = it;
			break;
		}
	}
	return *this;
}

TravelTracker& TravelTracker::ReadQuearies(RouteDatabase& data) {
	Map map(data);
	auto graph = map.BuildMap();
	Graph::Router<Weight> router(graph);

	GraphPtr graph_ptr = make_unique<Map::InternalGraph>(graph);
	RouterPtr router_ptr = make_unique<Graph::Router<Weight>>(router);

	out << "[\n";
	for (auto it = queary_pos; it != end(requests); ++it) {
		auto request = *it;
		size_t size = request.second.AsArray().size();
		for (size_t i = 0; i < size; ++i) {
			auto info = request.second.AsArray()[i].AsMap();
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
		}
	}

	out << "\n]";
	return *this;
}

void TravelManager(std::istream& in, ostream& out) {
	RouteDatabase data;
	TravelTracker travel(in, out);
	travel.ReadDatabaseFromJson(data).ReadQuearies(data);
}