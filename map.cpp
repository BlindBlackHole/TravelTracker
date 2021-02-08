#include "map.h"

using namespace std;

using GraphPtr = shared_ptr<Graph::DirectedWeightedGraph<Weight>>;
using RouterPtr = shared_ptr<Graph::Router<Weight>>;

double Map::GetDistanceBetweenStops(const vector<string>& stops, size_t from, size_t to) const {
	double distance = 0.0;
	const auto& stops_info = data.GetStopsInfo();

	string stop_name_lhs = stops[from];
	string stop_name_rhs = stops[to];
	auto lhs = stops_info.at(stop_name_lhs);
	auto rhs = stops_info.at(stop_name_rhs);

	auto dis = lhs.distance.find(stop_name_rhs);
	if (dis == end(lhs.distance)) {
		dis = rhs.distance.find(stop_name_lhs);
	}
	distance = dis->second.dis;
	return distance;
}

Map::InternalGraph Map::BuildMap() {
	const auto& stops_info = data.GetStopsInfo();
	const auto& busses_info = data.GetBusesInfo();
	const auto& route_info = data.GetRouteInfo();
	InternalGraph graph(stops_info.size() * 2);

	unordered_map<string, size_t> substops;
	for (const auto& bus : busses_info) {
		auto stops = bus.second.stops;
		for (size_t i = 0; i < stops.size(); ++i) {
			const StopInfo& from = stops_info.at(stops[i]);

			//add substop with weight of wait time
			StopId substop_id;
			double distance = 0.0;
			double distance_backward = 0.0;

			auto it = substops.find(from.name);
			if (it != end(substops)) {
				substop_id = it->second;
			}
			else {
				substop_id = data.AddSubstop(from.name);
				substops[from.name] = substop_id;
				graph.AddEdge(Graph::Edge<Weight>({ from.id, substop_id,
					Weight{ EdgeType::WAIT, route_info.bus_wait_time, from.name, 0 } }));
			}

			//add edges to others vertexes of the corrent route
			for (size_t j = i + 1; j < stops.size(); ++j) {
				const StopInfo& to = stops_info.at(stops[j]);

				//get distance and time
				distance += GetDistanceBetweenStops(stops, j - 1, j);
				double time = distance / (route_info.bus_velocity * 50.0 / 3.0);

				//add edge
				graph.AddEdge(Graph::Edge<Weight>({ substop_id, to.id,
					Weight{ EdgeType::BUS, time, bus.first, j - i } }));

				if (!bus.second.circle) {
					StopId substop_id2;
					auto it2 = substops.find(to.name);
					if (it2 != end(substops)) {
						substop_id2 = it2->second;
					}
					else {
						substop_id2 = data.AddSubstop(to.name);
						substops[to.name] = substop_id2;
						graph.AddEdge(Graph::Edge<Weight>({ to.id, substop_id2,
							Weight{ EdgeType::WAIT, route_info.bus_wait_time, to.name, 0 } }));
					}

					distance_backward += GetDistanceBetweenStops(stops, j, j - 1);
					time = distance_backward / (route_info.bus_velocity * 50.0 / 3.0);
					graph.AddEdge(Graph::Edge<Weight>({ substop_id2, from.id,
						Weight{ EdgeType::BUS, time, bus.first, j - i } }));
				}
			}
		}
	}
	return move(graph);
}

optional<vector<Weight>> Map::CreateRoute(std::shared_ptr<Graph::DirectedWeightedGraph<Weight>> graph,
	std::shared_ptr<Graph::Router<Weight>> router,
	Graph::VertexId from, Graph::VertexId to) const {
	auto route = router->BuildRoute(from, to);
	time_in_road = 0.0;
	if (!route) {
		return nullopt;
	}
	time_in_road = route->weight.weight;
	vector<Weight> stops;
	for (Graph::VertexId i = 0; i < route->edge_count; ++i) {
		auto edge = graph->GetEdge(router->GetRouteEdge(route->id, i));
		stops.push_back(edge.weight);
	}
	router->ReleaseRoute(route->id);
	return move(stops);
}

double Map::GetTimeInRoad() {
	return time_in_road;
}