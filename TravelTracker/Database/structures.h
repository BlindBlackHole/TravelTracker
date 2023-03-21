#pragma once
#include "Utils/svg.h"
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

const double PI = 3.1415926535;

enum class EdgeType {
    BUS,
    WAIT,
};

using StopId = size_t;

struct Extremes
{
    double max_lat = 0.0, min_lat = -1, max_lon = 0.0, min_lon = -1;
};

struct InternalCoords
{
    double coord = 0.0;
    std::string stop_name;
    size_t index = 0;
};

bool operator<(const InternalCoords& lhs, const InternalCoords& rhs);

struct StopsCoords
{
    double x = 0.0;
    double y = 0.0;
};

struct SupportStop
{
    int index;
    int next_sup_index;
    std::string stop_name;
};

struct StopInternalData
{
    size_t id;
    int dis;
};

struct StopInfo
{
    std::string name;
    StopId id{};
    double latitude{};
    double longtitude{};
    std::set<std::string> busses;
    std::unordered_map<std::string, StopInternalData> distance;
};

struct BusInfo
{
    std::string name;
    std::unordered_set<std::string> unic_stops;
    std::vector<std::string> stops;
    bool circle;
    Svg::Color color;
};

struct RouteInfo
{
    double bus_wait_time = 0.0;
    double bus_velocity = 0.0;
};

struct MapInfo
{
    double width = 0.0;
    double height = 0.0;
    double padding = 0.0;
    double stop_radius = 0.0;
    double line_width = 0.0;
    uint32_t stop_label_font_size = 0;
    Svg::Point stop_label_offset;
    Svg::Color underlayer_color;
    double underlayer_width = 0.0;
    std::vector<Svg::Color> color_palette;
    int bus_label_font_size = 0;
    Svg::Point bus_label_offset;
    std::vector<std::string> layers;
    double outer_margin = 0.0;
};

struct BusFullInfo
{
    bool isFound;
    std::string bus_name;
    int id;
    size_t unic_stop_count;
    double route_length;
    double curvature;
    size_t stop_count;
};

struct StopFullInfo
{
    bool isFound;
    std::set<std::string> buses;
    int id;
};

struct BusesRoute
{
    std::string bus_name;
    double time;
    size_t span_count;
    std::vector<std::string> stops;
};

struct Weight
{
    EdgeType edge;
    double weight = 0.0;
    std::string bus_or_stop;
    size_t stop_count;

    Weight(EdgeType edge, double weight, std::string bus_or_stop, size_t stop_count);

    Weight(double value);

    Weight& operator=(int64_t value);
};

bool is_equal(double x, double y, double epsilon = 1e-6);

bool operator>(const Weight& lhs, const Weight& rhs);

bool operator>=(const Weight& lhs, int64_t value);
