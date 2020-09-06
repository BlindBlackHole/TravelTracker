#include "routes_database.h"

using namespace std;

void RouteDatabase::SetRouteInfo(const RouteInfo& r_i) {
	route_info = r_i;
}

void RouteDatabase::SetBusInfo(const BusInfo& b_i) {
	busses_info.insert({ b_i.name, b_i });
	for (const auto& stop_name : b_i.unic_stops) {
		auto stop = stops_info.find(stop_name);
		if (stop == end(stops_info)) {
			stops_info.insert({ stop_name, StopInfo({ stop_name, GetStopIdByName(stop_name) }) });
			stops_info[stop_name].busses.insert(b_i.name);
		}
		else {
			stops_info[stop_name].busses.insert(b_i.name);
		}
	}
}

vector<string>& RouteDatabase::GetStopsById() {
	return stops_by_id;
}

unordered_set<string>& RouteDatabase::GetUniqueSubstops() {
	return unique_substops;
}

const unordered_map<string, BusInfo>& RouteDatabase::GetBusesInfo() const {
	return busses_info;
}

void RouteDatabase::SetStopInfo(const StopInfo& s_i) {
	auto stop = stops_info.find(s_i.name);
	if (stop != end(stops_info)) {
		stop->second.latitude = s_i.latitude;
		stop->second.longtitude = s_i.longtitude;
		stop->second.distance = s_i.distance;
	}
	else {
		stops_info.insert({ s_i.name, s_i });
	}
}

const unordered_map<string, StopInfo>& RouteDatabase::GetStopsInfo() const {
	return stops_info;
}

const RouteInfo& RouteDatabase::GetRouteInfo() const {
	return route_info;
}

StopId RouteDatabase::AddSubstop(const string& stop_name) {
	unique_substops.insert(stop_name);
	stops_by_id.push_back(stop_name);
	return stops_by_id.size() - 1;
}

StopId RouteDatabase::GetStopIdByName(const string& stop_name) {
	auto stop = stops_info.find(stop_name);
	if (stop == end(stops_info)) {
		stops_info.insert({ stop_name, StopInfo({
			stop_name,
			stops_info.size()
		}) });
		stops_by_id.push_back(stop_name);
		return stops_info.size() - 1;
	}
	else {
		return stop->second.id;
	}
}