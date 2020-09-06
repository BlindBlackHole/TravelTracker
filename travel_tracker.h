#pragma once
#include <memory>
#include <map>
#include <string>
#include <iostream>

#include "json.h"
#include "graph.h"
#include "router.h"
#include "structures.h"
#include "routes_database.h"
#include "map.h"
#include "queries.h"

class TravelTracker {
private:
	std::unique_ptr<Json::Document> doc;
	StopId stop_id = 0;
	std::ostream& out;
	std::map<std::string, Json::Node> requests;
	std::map<std::string, Json::Node>::iterator queary_pos;
public:

	TravelTracker(std::istream& in = std::cin, std::ostream& out = std::cout);

	TravelTracker& ReadDatabaseFromJson(RouteDatabase& data);

	TravelTracker& ReadQuearies(RouteDatabase& data);

};

void TravelManager(std::istream& in = std::cin, std::ostream& out = std::cout);