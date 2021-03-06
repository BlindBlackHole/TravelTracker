#pragma once

#include <iostream>
#include <memory>
#include <map>
#include <string>

#include "json.h"
#include "graph.h"
#include "router.h"
#include "structures.h"
#include "routes_database.h"
#include "map.h"
#include "queries.h"

class Reader {
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

class TravelTracker {
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