#pragma once

#include <iostream>
#include "structures.h"
#include "queries.h"
#include "coords.h"

class RenderMap {
protected:
	Svg::Document& doc;
	const RouteDatabase& data;
	std::shared_ptr<Coords> coords;
	std::shared_ptr<CompressedCoords> compressed_coords;
	const MapInfo& map_info;
	const std::unordered_map<std::string, BusInfo>& buses;
	const std::map<std::string, StopInfo>& stops;
public:
	void DrawBusLabel(const std::string& stop_name, const Svg::Color& color, const std::string& bus) const;
	//”казатель на метод класса в контейнере
	static const std::unordered_map<std::string, void (RenderMap::*)(void) const> LAYERS;
public:
	RenderMap(Svg::Document& doc, const RouteDatabase& data);

	void AddBusLines() const;
	void AddBusLabels() const;
	void AddStopPoints() const;
	void AddStopLabels() const;
};

class RenderRoute : public RenderMap {
private:
	const std::optional<std::vector<Weight>>& route;
	int from;
	int to;
	void DrawStopLabel(const std::string& stop_name) const;
public:
	static const std::unordered_map<std::string, void (RenderRoute::*)(void) const> LAYERS;
public:
	RenderRoute(Svg::Document& doc, const RouteDatabase& data, 
		const std::optional<std::vector<Weight>>& route, int from, int to);

	void AddRectangle() const;

	void AddBusLines() const;
	void AddBusLabels() const;
	void AddStopPoints() const;
	void AddStopLabels() const;
};

