#include "queries.h"
#include <fstream>
#include <math.h>

using namespace std;

using GraphPtr = shared_ptr<Graph::DirectedWeightedGraph<Weight>>;
using RouterPtr = shared_ptr<Graph::Router<Weight>>;

	QueryStop::QueryStop(RouteDatabase & data, const string & stop_name, int id, ostream & out)
		: data(data)
		, out(out)
		, stop_name(stop_name)
		, id(id) {}

	QueryStop& QueryStop::Procces() {
		const auto& stops_info = data.GetStopsInfo();
		auto stop = stops_info.find(stop_name);
		if (stop == end(stops_info)) {
			stop_info = StopFullInfo({
				false,
				{},
				id
				});
			return *this;
		}
		stop_info = StopFullInfo({
			true,
			stop->second.busses,
			id
			});
		return *this;
	}

	QueryStop& QueryStop::GetResult() {
		JsonParser::JsonStopOutput(move(stop_info), out);
		return *this;
	}

	double QueryBus::ConvertCoords(double coord) const {
		return coord * PI / 180;
	}

	double QueryBus::GetLength(const StopInfo & lhs, const StopInfo & rhs) const {
		return acos(sin(ConvertCoords(lhs.latitude)) * sin(ConvertCoords(rhs.latitude)) 
			+ cos(ConvertCoords(lhs.latitude)) * cos(ConvertCoords(rhs.latitude)) *
			cos(abs(ConvertCoords(lhs.longtitude) - ConvertCoords(rhs.longtitude)))) * 6371000;
	}

	QueryBus::QueryBus(RouteDatabase & data, const string & bus_name, int id, ostream & out) 
		: data(data)
		, out(out)
		, bus_name(bus_name)
		, id(id) {}

	QueryBus& QueryBus::Procces() {
		const auto& busses_info = data.GetBusesInfo();
		auto bus = busses_info.find(bus_name);
		if (bus == busses_info.end()) {
			bus_info = BusFullInfo({
				false,
				bus_name,
				id,
				unic_stop_count,
				0.0,
				0.0,
				stop_count
				});
			return *this;
		}
		Route route = GetRoute<1>(bus->second.stops);
		if (bus->second.circle) {
			stop_count = bus->second.stops.size();
		}
		else {
			stop_count = bus->second.stops.size() * 2 - 1;
			route += GetRoute<-1>(bus->second.stops);
		}

		double curvature = route.GetCur();

		unic_stop_count = bus->second.unic_stops.size();

		bus_info = BusFullInfo({
			true,
			bus_name,
			id,
			unic_stop_count,
			route.route_full,
			curvature,
			stop_count
			});
		return *this;
	}

	QueryBus& QueryBus::GetResult() {
		JsonParser::JsonBusOutput(move(bus_info), out);
		return *this;
	}


	QueryRoute::QueryRoute(RouteDatabase& data, Map& map, GraphPtr graph, RouterPtr router,
		Graph::VertexId from, Graph::VertexId to, int id, ostream& out)
		: data(data)
		, out(out)
		, map_(map)
		, from(from)
		, to(to)
		, id(id)
	{
		graph_ = graph;
		router_ = router;
	}

	QueryRoute& QueryRoute::Procces() {
		route_ = map_.CreateRoute(graph_, router_, from, to);
		return *this;
	}

	QueryRoute& QueryRoute::GetResult() {
		JsonParser::JsonRouteOutput(move(route_), map_.GetTimeInRoad(), data.GetRouteInfo().bus_wait_time, id, out);
		return *this;
	}

	QueryMap::QueryMap(RouteDatabase& data, int id, std::ostream& out) 
		: data(data), id(id), out(out) {}

	class Coords {
	private:
		const MapInfo& map_info;
		const Extremes& extremes;
		double x = 0.0;
		double y = 0.0;
		double zoom_coef = 0.0;
	private:
		double GetZoomCoef() const {
			double zoom_coef = 0.0;
			double width_zoom_coef = 0.0;
			double height_zoom_coef = 0.0;
			if (((extremes.max_lon - extremes.min_lon) == 0) && ((extremes.max_lat - extremes.min_lat) == 0)) {
				zoom_coef = 0.0;
			}else if (((extremes.max_lon - extremes.min_lon) == 0) && ((extremes.max_lat - extremes.min_lat) != 0)) {
				height_zoom_coef = (map_info.height - 2.0 * map_info.padding) / (extremes.max_lat - extremes.min_lat);
				zoom_coef = height_zoom_coef;
			}
			else if (((extremes.max_lat - extremes.min_lat) == 0) && ((extremes.max_lon - extremes.min_lon) != 0)) {
				width_zoom_coef = (map_info.width - 2.0 * map_info.padding) / (extremes.max_lon - extremes.min_lon);
				zoom_coef = width_zoom_coef;
			}
			else if(((extremes.max_lat - extremes.min_lat) != 0) && ((extremes.max_lon - extremes.min_lon) != 0)){
				height_zoom_coef = (map_info.height - 2.0 * map_info.padding) / (extremes.max_lat - extremes.min_lat);
				width_zoom_coef = (map_info.width - 2.0 * map_info.padding) / (extremes.max_lon - extremes.min_lon);
				zoom_coef = min(width_zoom_coef, height_zoom_coef);
			}
			return zoom_coef;
		}
	public:
		Coords(const MapInfo& map_info, const Extremes& ex) : map_info(map_info), extremes(ex) {
			zoom_coef = GetZoomCoef();
		};

		pair<double, double> GetCoords(double lon, double lat) {
			x = (lon - extremes.min_lon) * zoom_coef + map_info.padding;
			y = (extremes.max_lat - lat) * zoom_coef + map_info.padding;
			return { x, y };
		}
	};

	QueryMap& QueryMap::Procces() {
		const auto& map_info = data.GetMapInfo();
		const auto& extremes = data.GetExtremes();
		Coords coords(map_info, extremes);
		size_t size = map_info.color_palette.size();
		size_t i = 0;
		const auto& buses = data.GetBusesInfo();
		const auto& stops = data.GetStopsInfo();
		//Get bus
		for (const auto& bus : data.GetBuses()) {
			Svg::Polyline line = Svg::Polyline{}
				.SetStrokeWidth(map_info.line_width)
				.SetStrokeLineCap("round")
				.SetStrokeLineJoin("round")
				.SetStrokeColor(map_info.color_palette[i % size]);
			for (size_t i = 0; i < buses.at(bus).stops.size(); ++i) {
				auto stop_info = stops.at(buses.at(bus).stops[i]);
				auto [x, y] = coords.GetCoords(stop_info.longtitude, stop_info.latitude);
				line.AddPoint({ x,y });
			}
			if (!buses.at(bus).circle) {
				for (int i = buses.at(bus).stops.size() - 2; i >= 0; --i) {
					auto stop_info = stops.at(buses.at(bus).stops[i]);
					auto [x, y] = coords.GetCoords(stop_info.longtitude, stop_info.latitude);
					line.AddPoint({ x,y });
				}
			}
			doc.Add(move(line));
			++i;
		}
		for (const auto& stop : data.GetStopsInfo()) {
			auto [x, y] = coords.GetCoords(stop.second.longtitude, stop.second.latitude);
			doc.Add(Svg::Circle{}
				.SetFillColor("white")
				.SetRadius(map_info.stop_radius)
				.SetCenter({x,y}));
		}
		for (const auto& stop : data.GetStopsInfo()) {
			auto [x, y] = coords.GetCoords(stop.second.longtitude, stop.second.latitude);
			doc.Add(Svg::Text{}
				.SetFillColor(map_info.underlayer_color)
				.SetStrokeColor(map_info.underlayer_color)
				.SetStrokeWidth(map_info.underlayer_width)
				.SetStrokeLineJoin("round")
				.SetStrokeLineCap("round")
				.SetFontFamily("Verdana")
				.SetFontSize(map_info.stop_label_font_size)
				.SetData(stop.first)
				.SetOffset(map_info.stop_label_offset)
				.SetPoint({ x,y }));
			doc.Add(Svg::Text{}
				.SetFillColor("black")
				.SetFontFamily("Verdana")
				.SetFontSize(map_info.stop_label_font_size)
				.SetData(stop.first)
				.SetOffset(map_info.stop_label_offset)
				.SetPoint({ x,y }));
		}

		return *this;
	}

	QueryMap& QueryMap::GetResult() {
		ofstream out("out" + to_string(id) + ".svg");
		doc.Render(out);
		JsonParser::JsonMapOutput(doc, id);
		return *this;
	}