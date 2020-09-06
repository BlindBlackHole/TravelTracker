#include "queries.h"

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

	double QueryBus::GetLength(const StopInfo & lhs, const StopInfo & rhs) const {
		return acos(sin(lhs.latitude) * sin(rhs.latitude) + cos(lhs.latitude) * cos(rhs.latitude) *
			cos(abs(lhs.longtitude - rhs.longtitude))) * 6371000;
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


	QueryRoute::QueryRoute(RouteDatabase & data, Map & map, GraphPtr graph, RouterPtr router,
		Graph::VertexId from, Graph::VertexId to, int id, ostream & out)
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