#include "routes_database.h"

using namespace std;

void RouteDatabase::SetRouteInfo(const RouteInfo& r_i) {
    route_info = r_i;
}

void RouteDatabase::SetMapInfo(const MapInfo& m_i) {
    map_info = m_i;
}

void RouteDatabase::SetBusesColors() {
    size_t i = 0;
    size_t palette_size = map_info.color_palette.size();
    if (palette_size == 0)
        return;
    for (const string& bus_name : buses) {
        busses_info[bus_name].color = map_info.color_palette[i % palette_size];
        ++i;
    }
}

void RouteDatabase::SetBusInfo(const BusInfo& b_i) {
    busses_info.insert({ b_i.name, b_i });
    buses.insert(b_i.name);
    for (const auto& stop_name : b_i.unic_stops) {
        auto stop = stops_info.find(stop_name);
        if (stop == end(stops_info)) {
            StopInfo stopInfo;
            stopInfo.name = stop_name;
            stopInfo.id = GetStopIdByName(stop_name);
            stops_info.emplace(stop_name, std::move(stopInfo));
            stops_info[stop_name].busses.insert(b_i.name);
        }
        else {
            stops_info[stop_name].busses.insert(b_i.name);
        }
    }
}

const vector<string>& RouteDatabase::GetStopsById() const {
    return stops_by_id;
}

unordered_set<string>& RouteDatabase::GetUniqueSubstops() {
    return unique_substops;
}

const unordered_map<string, BusInfo>& RouteDatabase::GetBusesInfo() const {
    return busses_info;
}

const set<string>& RouteDatabase::GetBuses() const {
    return buses;
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
    stops_geograf_coords.insert({ s_i.name, {s_i.longtitude, s_i.latitude} });
}

const map<string, StopInfo>& RouteDatabase::GetStopsInfo() const {
    return stops_info;
}

const unordered_map<string, StopsCoords>& RouteDatabase::GetStopsCoords() const {
    return stops_geograf_coords;
}

const RouteInfo& RouteDatabase::GetRouteInfo() const {
    return route_info;
}

const MapInfo& RouteDatabase::GetMapInfo() const {
    return map_info;
}

void RouteDatabase::SetExtremes(Extremes ex) {
    extremes = ex;
}

const Extremes& RouteDatabase::GetExtremes() const {
    return extremes;
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
