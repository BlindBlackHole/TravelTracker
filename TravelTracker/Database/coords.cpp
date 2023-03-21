#include "coords.h"
#include <algorithm>
#include <assert.h>

using namespace std;


//--Coords
Coords::Coords(const MapInfo& map_info, const Extremes& ex)
	: map_info(map_info), extremes(ex) {
	zoom_coef = GetZoomCoef();
};

double Coords::GetZoomCoef() const {
	double zoom_coef = 0.0;
	double width_zoom_coef = 0.0;
	double height_zoom_coef = 0.0;
	if ((std::abs(extremes.max_lon - extremes.min_lon) < 1e-6) && (std::abs(extremes.max_lat - extremes.min_lat) < 1e-6)) {
		zoom_coef = 0.0;
	}
	else if ((std::abs(extremes.max_lon - extremes.min_lon) < 1e-6) && !(std::abs(extremes.max_lat - extremes.min_lat) < 1e-6)) {
		height_zoom_coef = (map_info.height - 2.0 * map_info.padding) / (extremes.max_lat - extremes.min_lat);
		zoom_coef = height_zoom_coef;
	}
	else if (!(std::abs(extremes.max_lon - extremes.min_lon) < 1e-6) && (std::abs(extremes.max_lat - extremes.min_lat) < 1e-6)) {
		width_zoom_coef = (map_info.width - 2.0 * map_info.padding) / (extremes.max_lon - extremes.min_lon);
		zoom_coef = width_zoom_coef;
	}
	else if (!(std::abs(extremes.max_lat - extremes.min_lat) < 1e-6) && !(std::abs(extremes.max_lon - extremes.min_lon) < 1e-6)) {
		height_zoom_coef = (map_info.height - 2.0 * map_info.padding) / (extremes.max_lat - extremes.min_lat);
		width_zoom_coef = (map_info.width - 2.0 * map_info.padding) / (extremes.max_lon - extremes.min_lon);
		zoom_coef = std::min(width_zoom_coef, height_zoom_coef);
	}
	return zoom_coef;
}

const MapInfo& Coords::GetMapInfo() const {
	return map_info;
}

pair<double, double> Coords::GetCoords(double lon, double lat) const {
	double x = (lon - extremes.min_lon) * zoom_coef + map_info.padding;
	double y = (extremes.max_lat - lat) * zoom_coef + map_info.padding;
	return { x, y };
}
//Coords--

//--CompressedCoords
unordered_map<string, vector<SupportStop>> CompressedCoords::GetSupportsStops() const {
	unordered_map<string, vector<SupportStop>> sup_stops;
	const auto& stops_info = data.GetStopsInfo();
	const auto& buses = data.GetBusesInfo();
	for (const auto& [bus_name, bus_info] : buses) {
		//final stops
		vector<SupportStop> v;
		v.push_back({ 0, 0, bus_info.stops[0] });
		//transfer stops
		for (int i = 1; i < bus_info.stops.size() - 1; ++i) {
			int c = count(bus_info.stops.begin(), bus_info.stops.end(), bus_info.stops[i]);
			if (!bus_info.circle && c > 1) {
				v.push_back({ i, -1, bus_info.stops[i] });
				v[v.size() - 2].next_sup_index = i;
			}else if (stops_info.at(bus_info.stops[i]).busses.size() > 1 || c > 2) {
				v.push_back({ i, -1, bus_info.stops[i] });
				v[v.size() - 2].next_sup_index = i;
			}
		}
		v.push_back({ (int)(bus_info.stops.size() - 1), -1, bus_info.stops.back() });
		v[v.size() - 2].next_sup_index = (int)(bus_info.stops.size() - 1);
		sup_stops.insert({ bus_name, v });
	}
	return sup_stops;
}

unordered_map<string, StopsCoords> 
CompressedCoords::RecalculateGeographicCoords(const unordered_map<string, StopsCoords>& stops_geograf_coords) const {
	unordered_map<string, StopsCoords> g_coords;
	//get supports stops
	unordered_map<string, vector<SupportStop>> sup_stops = GetSupportsStops();
	//recalculating
	const auto& stops_info = data.GetStopsInfo();
	const auto& buses = data.GetBusesInfo();
	for (const auto& [stop_name, stop_coords] : stops_geograf_coords) {
		const auto& busses = stops_info.at(stop_name).busses;
		if (busses.size() == 0) {
			g_coords.insert({ stop_name, stop_coords });
		}
	}


	for (const auto& [bus_name, bus_info] : buses) {
		const auto& stops = bus_info.stops;
		auto support_stops = sup_stops.find(bus_name)->second;
		if (support_stops.size() == 1) {
			for (const auto& stop : stops) {
				const auto& stop_name = support_stops[0].stop_name;
				const StopsCoords& coords = stops_geograf_coords.at(stop_name);
				g_coords.insert({ stop, coords });
			}
			return g_coords;
		}
		for (const auto& stop : support_stops) {
			/*name is not supporting stop*/
			int curr_index = stop.index;
			int next_index = stop.next_sup_index;
			for (size_t i = curr_index + 1; next_index != -1 && i < next_index; ++i) {
				const StopsCoords& coords = stops_geograf_coords.at(stops[i]);
				double lon_step = //x
					(stops_info.at(stops[next_index]).longtitude -
						stops_info.at(stops[curr_index]).longtitude) /
					(double)(next_index - curr_index);
				double x = stops_info.at(stops[curr_index]).longtitude + lon_step * (double)(i - curr_index);
				double lat_step = //y
					(stops_info.at(stops[next_index]).latitude -
						stops_info.at(stops[curr_index]).latitude) /
					(double)(next_index - curr_index);
				double y = stops_info.at(stops[curr_index]).latitude + lat_step * (double)(i - curr_index);
				g_coords[stops[i]] = { x, y };
			}
			const StopsCoords& coords = stops_geograf_coords.at(stops[stop.index]);
			g_coords[stops[stop.index]] = coords;
		}
	}
	return g_coords;
}

void CompressedCoords::RecalculateIndexes(vector<InternalCoords>& coords) const {
	const auto& stops_info = data.GetStopsInfo();
	const auto& buses = data.GetBusesInfo();
	if (coords.size() == 0) {
		throw runtime_error("coords.size() == 0");
	}
	vector<vector<string>> indexes(coords.size(), vector<string>());
	indexes[0].push_back(coords[0].stop_name);
	size_t index = 0;
	for (size_t i = 1; i < coords.size(); ++i) {
		const StopInfo& curr_stop_info = stops_info.at(coords[i].stop_name);
		const StopInfo& prev_stop_info = stops_info.at(coords[i - 1].stop_name);
		const auto& curr_buses = curr_stop_info.busses;
		const auto& prev_buses = prev_stop_info.busses;
		size_t curr_idx = i;
		size_t prev_idx = index;
		coords[i].index = i;
		//Is stops are neighbors key = true;
		bool key = false;
		for (auto& prev_index_stop : indexes[prev_idx]) {
			if (key)
				break;
			const StopInfo& prev_stop = stops_info.at(prev_index_stop);
			for (const auto& bus : prev_stop.busses) {
				if (key)
					break;
				const auto& buses_stops = buses.at(bus).stops;
				for (int j = 0; j < buses_stops.size(); ++j) {
					if (buses_stops[j] == prev_stop.name) {
						if ((j - 1) >= 0) {
							if (buses_stops[j - 1] == curr_stop_info.name) {
								key = true;
							}
						}
						if ((j + 1) < buses_stops.size()) {
							if (buses_stops[j + 1] == curr_stop_info.name) {
								key = true;
							}
						}
					}
				}
			}
		}

		if (!key) {
			coords[i].index = prev_idx;
		}
		else {
			coords[i].index = prev_idx + 1;
			index++;
		}
		indexes[index].push_back(curr_stop_info.name);
	}
}

pair<bool, int> CompressedCoords::isNeighbor(const InternalCoords& prev_stop, const InternalCoords& curr_stop) const {
	bool key = false;
	int index = -1;
	const auto& stops_info = data.GetStopsInfo();
	const auto& buses = data.GetBusesInfo();
	const auto& stop_buses = stops_info.at(curr_stop.stop_name).busses;
	for (const auto& bus : stop_buses) {
		const auto& buses_stops = buses.at(bus).stops;
		for (int j = 0; j < buses_stops.size(); ++j) {
			if (buses_stops[j] == prev_stop.stop_name) {
				if ((j - 1) >= 0) {
					if (buses_stops[j - 1] == curr_stop.stop_name) {
						key = true;
						index = prev_stop.index;
					}
				}
				if ((j + 1) < buses_stops.size()) {
					if (buses_stops[j + 1] == curr_stop.stop_name) {
						key = true;
						index = prev_stop.index;
					}
				}
			}
		}
	}
	return { key, index };
}

int CompressedCoords::RecalculateIndexesAdvance(vector<InternalCoords>& coords) const {
	const auto& stops_info = data.GetStopsInfo();
	const auto& buses = data.GetBusesInfo();
	int max_global_index = 0;
	if (coords.size() == 0) {
		throw runtime_error("coords.size() == 0");
	}
	for (size_t i = 1; i < coords.size(); ++i) {
		bool key = false;
		int max_index = -1;
		for (int j = 0; j < i; ++j) {
			auto [neigh, index] = isNeighbor(coords[j], coords[i]);
			if(!key)
				key = neigh;
			max_index = std::max(max_index, index);
		}
		if (key) {
			coords[i].index = ++max_index;
		}
		max_global_index = std::max(max_global_index, max_index);
	}
	return max_global_index;
}

pair<vector<InternalCoords>, vector<InternalCoords>> CompressedCoords::ComputeDigitalCoords() const {
	vector<InternalCoords> x_coords;
	vector<InternalCoords> y_coords;
	size_t idx = 0;
	for (const auto [stop_name, coords] : RecalculateGeographicCoords(stops_geograf_coords)) {
		auto [x, y] = this->coords->GetCoords(coords.x, coords.y);
		x_coords.push_back({ x, stop_name, idx });
		y_coords.push_back({ y, stop_name, idx });
	}
	std::sort(x_coords.begin(), x_coords.end());
	std::sort(y_coords.rbegin(), y_coords.rend());
	max_lon_idx = RecalculateIndexesAdvance(x_coords);
	max_lat_idx = RecalculateIndexesAdvance(y_coords);
	return { move(x_coords), move(y_coords) };
}

std::unordered_map<std::string, StopsCoords> CompressedCoords::ComputeCompressedCoords() const {
	std::unordered_map<std::string, StopsCoords> c_coords;

	const auto& [xs, ys] = digital_coords;
	assert(xs.size() == ys.size());
	const MapInfo& map_info = coords->GetMapInfo();
	if (xs.size() == 1) {
		c_coords.insert({ xs.begin()->stop_name, { map_info.padding, map_info.height - map_info.padding } });
		return c_coords;
	}
	double x_step = (map_info.width - 2.0 * map_info.padding) / (max_lon_idx);
	double y_step = (map_info.height - 2.0 * map_info.padding) / (max_lat_idx);
	for (auto it = ys.begin(); it != ys.end(); ++it) {
		double comp_coord = map_info.height - map_info.padding - it->index * y_step;
		c_coords.insert({ it->stop_name, { 0.0, comp_coord } });
	}
	for (const auto& x : xs) {
		double comp_coord = x.index * x_step + map_info.padding;
		auto y = c_coords[x.stop_name].y;
		c_coords[x.stop_name] = { comp_coord, y };
	}
	return c_coords;
}

CompressedCoords::CompressedCoords(std::shared_ptr<Coords> coords, const RouteDatabase& data)
	: coords(coords)
	, data(data)
	, stops_geograf_coords(data.GetStopsCoords())
{
	digital_coords = ComputeDigitalCoords();
	compressed_coords = ComputeCompressedCoords();
}

StopsCoords CompressedCoords::GetCoords(const std::string& stop_name) const {
	return compressed_coords.at(stop_name);
}
//CompressedCoords--