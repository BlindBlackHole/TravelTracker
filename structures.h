#pragma once
#include <string>
#include <unordered_set>
#include <vector>
#include <set>
#include <unordered_map>

const double PI = 3.1415926535;

enum EdgeType {
	BUS,
	WAIT,
};

using StopId = size_t;

struct StopInternalData
{
	size_t id;
	int dis;
};

struct StopInfo {
	std::string name;
	StopId id;
	double latitude;
	double longtitude;
	std::set<std::string> busses;
	std::unordered_map<std::string, StopInternalData> distance;
};

struct BusInfo {
	std::string name;
	std::unordered_set<std::string> unic_stops;
	std::vector<std::string> stops;
	bool circle;
};

struct RouteInfo {
	double bus_wait_time;
	double bus_velocity;
};

struct BusFullInfo {
	bool isFound;
	std::string bus_name;
	int id;
	size_t unic_stop_count;
	double route_length;
	double curvature;
	size_t stop_count;
};

struct StopFullInfo {
	bool isFound;
	std::set<std::string> buses;
	int id;
};

struct BusesRoute {
	std::string bus_name;
	double time;
	size_t span_count;
	std::vector<std::string> stops;
};

struct Weight {
	EdgeType edge;
	double weight = 0.0;
	std::string bus_or_stop;
	size_t stop_count;

	Weight(EdgeType edge, double weight, std::string bus_or_stop, size_t stop_count);

	Weight(double value);

	Weight& operator=(int64_t value);
};

bool operator > (const Weight& lhs, const Weight& rhs);

bool operator >= (const Weight& lhs, int64_t value);
