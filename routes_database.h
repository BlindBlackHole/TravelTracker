#pragma once

#include "structures.h"

class RouteDatabase {
private:
	std::unordered_map<std::string, BusInfo> busses_info;
	std::unordered_map<std::string, StopInfo> stops_info;
	std::vector<std::string> stops_by_id;
	RouteInfo route_info;
	std::unordered_set<std::string> unique_substops;
public:

	RouteDatabase() = default;

	void SetRouteInfo(const RouteInfo& r_i);

	void SetBusInfo(const BusInfo& b_i);

	std::vector<std::string>& GetStopsById();

	std::unordered_set<std::string>& GetUniqueSubstops();

	const std::unordered_map<std::string, BusInfo>& GetBusesInfo() const;

	void SetStopInfo(const StopInfo& s_i);

	const std::unordered_map<std::string, StopInfo>& GetStopsInfo() const;

	const RouteInfo& GetRouteInfo() const;

	StopId AddSubstop(const std::string& stop_name);

	StopId GetStopIdByName(const std::string& stop_name);
};