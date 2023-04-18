#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <string>

#include "Utils/json.h"
#include "graph.h"
#include "queries.h"
#include "routes_database.h"
#include "structures.h"
#include <QJsonObject>

class Reader
{
private:
    RouteDatabase& data_;

public:
    explicit Reader(RouteDatabase& data);

    void ReadRoutingSettings(const std::map<std::string, Json::Node>& info);

    void ReadRenderSettings(const std::map<std::string, Json::Node>& info);

    void ReadStopInfo(const std::map<std::string, Json::Node>& info);

    void ReadBusInfo(const std::map<std::string, Json::Node>& info);

    void ReadBaseRequests(const std::vector<Json::Node>& info);
};

class RouteManager
{
private:
    using GraphPtr = std::shared_ptr<Graph::DirectedWeightedGraph<Weight>>;
    using RouterPtr = std::shared_ptr<Graph::Router<Weight>>;

    RouteDatabase fDatabase;
    Map fMap;
    RouterPtr fRouter;
    GraphPtr fGraph;

private:
    void initializeInternal();

public:
    RouteManager() noexcept;

    void uploadDatabase(std::istream& in);

    std::string getStops() const;

    std::string getBuses() const;

    std::string getMap() const;

    std::string getRoute(std::string_view from, std::string_view to);
};

class TravelTracker
{
private:
    std::unique_ptr<Json::Document> doc;
    StopId stop_id = 0;
    std::ostream& out;
    std::map<std::string, Json::Node> requests;
    std::map<std::string, Json::Node>::iterator queary_pos;

public:
    TravelTracker(std::istream& in = std::cin, std::ostream& out = std::cout);

    TravelTracker& ReadDataFromJson(RouteDatabase& data);

    TravelTracker& QueryProcessing(RouteDatabase& data);
};

void TravelManager(std::istream& in = std::cin, std::ostream& out = std::cout);
