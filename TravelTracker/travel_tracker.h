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
    std::string currentMap;

    std::unordered_map<std::string, std::string> maps;

private:
    void initializeInternal();

public:
    RouteManager() noexcept;

    void uploadDatabase(std::istream& in);

    void initMap(const std::string& mapFile);

    auto addMap(std::string filename, std::string mapData) -> std::pair<std::string, std::string>;

    std::string getStops() const;

    std::string getBuses() const;

    std::string renderMap(const std::string& mapName);

    std::string getRoute(std::string_view from, std::string_view to);

    std::string getMaps() const;

    std::string getMapName(const std::string& mapId) const;

    const std::string& getCurrentMap() const;

    void setCurrentMap(const std::string& map);

    void loadSavedMaps();

    void reset();
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
