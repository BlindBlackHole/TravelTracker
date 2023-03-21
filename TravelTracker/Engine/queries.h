#pragma once

#include <iostream>

#include "graph.h"
#include "json.h"
#include "map.h"
#include "router.h"
#include "routes_database.h"
#include "structures.h"

class Query
{
public:
    virtual Query& Procces() = 0;
    virtual Query& GetResult() = 0;
};

class QueryStop : public Query
{
private:
    StopFullInfo stop_info;
    std::string stop_name;
    int id;
    std::ostream& out;
    RouteDatabase& data;

public:
    QueryStop(
        RouteDatabase& data, const std::string& stop_name, int id, std::ostream& out = std::cout
    );

    QueryStop& Procces() override;

    QueryStop& GetResult() override;
};

class QueryBus : public Query
{
private:
    std::string bus_name;
    int id;
    std::ostream& out;
    size_t stop_count = 0;
    size_t unic_stop_count = 0;
    BusFullInfo bus_info;
    RouteDatabase& data;

    double ConvertCoords(double coord) const;

private:
    struct Route
    {
        double route_full = 0.0;
        double route_forward = 0.0;

        Route& operator+=(const Route& rhs)
        {
            route_full += rhs.route_full;
            route_forward += rhs.route_forward;
            return *this;
        }

        double GetCur() { return route_full / route_forward; }
    };

    double GetLength(const StopInfo& lhs, const StopInfo& rhs) const;

    template <int SIGN>
    Route GetRoute(const std::vector<std::string>& stops) const
    {
        static_assert(SIGN == -1 || SIGN == 1);
        Route result;
        const auto& stops_info = data.GetStopsInfo();
        if (SIGN == -1) {
            for (size_t i = stops.size() - 1; i >= 1; --i) {
                std::string stop_name_lhs = stops[i];
                std::string stop_name_rhs = stops[i - 1];
                auto lhs = stops_info.at(stop_name_lhs);
                auto rhs = stops_info.at(stop_name_rhs);
                auto dis = lhs.distance.find(stop_name_rhs);
                if (dis == end(lhs.distance)) {
                    dis = rhs.distance.find(stop_name_lhs);
                }
                result.route_full += dis->second.dis;
                result.route_forward += GetLength(lhs, rhs);
            }
        }
        else {
            for (size_t i = 1; i < stops.size(); ++i) {
                std::string stop_name_lhs = stops[i - 1];
                std::string stop_name_rhs = stops[i];
                auto lhs = stops_info.at(stop_name_lhs);
                auto rhs = stops_info.at(stop_name_rhs);
                auto dis = lhs.distance.find(stop_name_rhs);
                if (dis == end(lhs.distance)) {
                    dis = rhs.distance.find(stop_name_lhs);
                }
                result.route_full += dis->second.dis;
                result.route_forward += GetLength(lhs, rhs);
            }
        }
        return result;
    }

public:
    QueryBus(
        RouteDatabase& data, const std::string& bus_name, int id, std::ostream& out = std::cout
    );

    QueryBus& Procces() override;

    QueryBus& GetResult() override;
};

class QueryRoute : public Query
{
private:
    Graph::VertexId from;
    Graph::VertexId to;
    int id;
    std::ostream& out;
    Map& map_;
    std::shared_ptr<Graph::DirectedWeightedGraph<Weight>> graph_;
    std::shared_ptr<Graph::Router<Weight>> router_;
    std::optional<std::vector<Weight>> route_;
    RouteDatabase& data;
    Svg::Document doc;

public:
    QueryRoute(
        RouteDatabase& data, Map& map, std::shared_ptr<Graph::DirectedWeightedGraph<Weight>> graph,
        std::shared_ptr<Graph::Router<Weight>> router, Graph::VertexId from, Graph::VertexId to,
        int id, std::ostream& out = std::cout
    );

    QueryRoute& Procces() override;

    QueryRoute& GetResult() override;
};

class QueryMap : public Query
{
private:
    int id;
    std::ostream& out;
    RouteDatabase& data;
    Svg::Document doc;

public:
    QueryMap(RouteDatabase& data, int id, std::ostream& out = std::cout);

    QueryMap& Procces() override;

    QueryMap& GetResult() override;
};
