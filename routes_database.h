#pragma once

#include "structures.h"

class RouteDatabase {
private:
	std::unordered_map<std::string, BusInfo> busses_info;
	std::set<std::string> buses;
	std::map<std::string, StopInfo> stops_info;
	std::vector<std::string> stops_by_id;
	RouteInfo route_info;
	MapInfo map_info;
	Extremes extremes;
	std::unordered_set<std::string> unique_substops;
public:
	RouteDatabase() = default;

	void SetRouteInfo(const RouteInfo& r_i);
	void SetBusInfo(const BusInfo& b_i);
	void SetMapInfo(const MapInfo& m_i);
	void SetStopInfo(const StopInfo& s_i);

	std::vector<std::string>& GetStopsById();
	std::unordered_set<std::string>& GetUniqueSubstops();
	Extremes& GetExtremes();
	void SetExtremes(Extremes ex);
	const std::unordered_map<std::string, BusInfo>& GetBusesInfo() const;
	const std::set<std::string>& GetBuses() const;
	const std::map<std::string, StopInfo>& GetStopsInfo() const;
	StopId GetStopIdByName(const std::string& stop_name);
	const RouteInfo& GetRouteInfo() const;
	const MapInfo& GetMapInfo() const;

	StopId AddSubstop(const std::string& stop_name);
};