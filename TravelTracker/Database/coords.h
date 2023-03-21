#pragma once
#include "structures.h"
#include "routes_database.h"

class Coords {
private:
	const MapInfo& map_info;
	const Extremes& extremes;
	double zoom_coef = 0.0;
private:
	double GetZoomCoef() const;
public:
	Coords(const MapInfo& map_info, const Extremes& ex);

	std::pair<double, double> GetCoords(double lon, double lat) const;

	const MapInfo& GetMapInfo() const;
};

class CompressedCoords {
private:
	mutable int max_lat_idx = 0;
	mutable int max_lon_idx = 0;
	std::shared_ptr<Coords> coords;
	const std::unordered_map<std::string, StopsCoords>& stops_geograf_coords;	//lon, lat
	std::pair<std::vector<InternalCoords>,
		std::vector<InternalCoords>> digital_coords;							//x, y
	std::unordered_map<std::string, StopsCoords> compressed_coords;				//compressed x, y
	const RouteDatabase& data;
private:
	std::pair<bool, int> isNeighbor(const InternalCoords& prev_stop, const InternalCoords& curr_stop) const;
	void RecalculateIndexes(std::vector<InternalCoords>& coord) const;
	int RecalculateIndexesAdvance(std::vector<InternalCoords>& coords) const;
	std::unordered_map<std::string, std::vector<SupportStop>> GetSupportsStops() const;
	std::unordered_map<std::string, StopsCoords> RecalculateGeographicCoords(
		const std::unordered_map<std::string, StopsCoords>& stops_geograf_coords) const;
	std::pair<std::vector<InternalCoords>, std::vector<InternalCoords>> ComputeDigitalCoords() const;
	std::unordered_map<std::string, StopsCoords> ComputeCompressedCoords() const;
public:
	CompressedCoords(std::shared_ptr<Coords> coords, const RouteDatabase& data);

	StopsCoords GetCoords(const std::string& stop_name) const;
};