#pragma once

#include <optional>
#include <memory>

#include "structures.h"
#include "routes_database.h"
#include "graph.h"
#include "router.h"

class Map {
private:
	RouteDatabase& data;
	mutable double time_in_road = 0.0;
public:
	using InternalGraph = Graph::DirectedWeightedGraph<Weight>;

	Map(RouteDatabase& data) : data(data) {}

	double GetDistanceBetweenStops(const std::vector<std::string>& stops, size_t from, size_t to) const;

	InternalGraph BuildMap();

	std::optional<std::vector<Weight>> CreateRoute(std::shared_ptr<Graph::DirectedWeightedGraph<Weight>> graph,
		std::shared_ptr<Graph::Router<Weight>> router,
		Graph::VertexId from, Graph::VertexId to) const;

	double GetTimeInRoad();
};