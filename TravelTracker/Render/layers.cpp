#include "layers.h"
#include <algorithm>

using namespace std;

RenderMap::RenderMap(Svg::Document& doc, const RouteDatabase& data)
    : doc(doc),
      data(data),
      map_info(data.GetMapInfo()),
      buses(data.GetBusesInfo()),
      stops(data.GetStopsInfo())
{
    const auto& extremes = data.GetExtremes();
    coords = std::make_shared<Coords>(map_info, extremes);
    compressed_coords = std::make_shared<CompressedCoords>(coords, data);
}

const unordered_map<string, void (RenderMap::*)(void) const> RenderMap::LAYERS = {
    {"bus_lines",   &RenderMap::AddBusLines  },
    {"bus_labels",  &RenderMap::AddBusLabels },
    {"stop_points", &RenderMap::AddStopPoints},
    {"stop_labels", &RenderMap::AddStopLabels}
};

void RenderMap::AddBusLines() const
{
    size_t i = 0;
    size_t palette_size = map_info.color_palette.size();
    for (const auto& bus : data.GetBuses()) {
        // Bus lines
        Svg::Polyline line = Svg::Polyline{}
                                 .SetStrokeWidth(map_info.line_width)
                                 .SetStrokeLineCap("round")
                                 .SetStrokeLineJoin("round")
                                 .SetStrokeColor(map_info.color_palette[i % palette_size]);
        // Point to the line
        for (size_t ii = 0; ii < buses.at(bus).stops.size(); ++ii) {
            auto stop_info = stops.at(buses.at(bus).stops[ii]);
            auto [x, y] = compressed_coords->GetCoords(stop_info.name);
            line.AddPoint({x, y});
        }
        // If route is circle
        if (!buses.at(bus).circle && buses.at(bus).stops.size() > 1) {
            for (int j = buses.at(bus).stops.size() - 2; j >= 0; --j) {
                auto stop_info = stops.at(buses.at(bus).stops[j]);
                auto [x, y] = compressed_coords->GetCoords(stop_info.name);
                line.AddPoint({x, y});
            }
        }
        doc.Add(move(line));
        // Buses names
        ++i;
    }
}

void RenderMap::DrawBusLabel(
    const std::string& stop_name, const Svg::Color& color, const std::string& bus
) const
{
    auto [x, y] = compressed_coords->GetCoords(stop_name);
    Svg::Text bus_label = Svg::Text{}
                              .SetData(bus)
                              .SetOffset(map_info.bus_label_offset)
                              .SetFontSize(map_info.bus_label_font_size)
                              .SetFontFamily("Verdana")
                              .SetFontWeight("bold")
                              .SetFillColor(color)
                              .SetPoint({x, y});
    Svg::Text bus_sublabel = Svg::Text{}
                                 .SetData(bus)
                                 .SetOffset(map_info.bus_label_offset)
                                 .SetFontSize(map_info.bus_label_font_size)
                                 .SetFontFamily("Verdana")
                                 .SetFontWeight("bold")
                                 .SetFillColor(map_info.underlayer_color)
                                 .SetStrokeColor(map_info.underlayer_color)
                                 .SetStrokeWidth(map_info.underlayer_width)
                                 .SetStrokeLineCap("round")
                                 .SetStrokeLineJoin("round")
                                 .SetPoint({x, y});
    doc.Add(bus_sublabel);
    doc.Add(bus_label);
}

void RenderMap::AddBusLabels() const
{
    size_t i = 0;
    size_t palette_size = map_info.color_palette.size();
    for (const string& bus : data.GetBuses()) {
        const string& stop_name_first = buses.at(bus).stops.front();
        DrawBusLabel(stop_name_first, map_info.color_palette[i % palette_size], bus);
        if (!buses.at(bus).circle && buses.at(bus).stops.front() != buses.at(bus).stops.back()) {
            const string& stop_name_last = buses.at(bus).stops.back();
            DrawBusLabel(stop_name_last, map_info.color_palette[i % palette_size], bus);
        }
        ++i;
    }
}

void RenderMap::AddStopPoints() const
{
    for (const auto& stop : data.GetStopsInfo()) {
        auto [x, y] = compressed_coords->GetCoords(stop.first);
        doc.Add(
            Svg::Circle{}.SetFillColor("white").SetRadius(map_info.stop_radius).SetCenter({x, y})
        );
    }
}

void RenderMap::AddStopLabels() const
{
    for (const auto& stop : data.GetStopsInfo()) {
        auto [x, y] = compressed_coords->GetCoords(stop.first);
        doc.Add(Svg::Text{}
                    .SetFillColor(map_info.underlayer_color)
                    .SetStrokeColor(map_info.underlayer_color)
                    .SetStrokeWidth(map_info.underlayer_width)
                    .SetStrokeLineJoin("round")
                    .SetStrokeLineCap("round")
                    .SetFontFamily("Verdana")
                    .SetFontSize(map_info.stop_label_font_size)
                    .SetData(stop.first)
                    .SetOffset(map_info.stop_label_offset)
                    .SetPoint({x, y}));
        doc.Add(Svg::Text{}
                    .SetFillColor("black")
                    .SetFontFamily("Verdana")
                    .SetFontSize(map_info.stop_label_font_size)
                    .SetData(stop.first)
                    .SetOffset(map_info.stop_label_offset)
                    .SetPoint({x, y}));
    }
}

RenderRoute::RenderRoute(
    Svg::Document& doc, const RouteDatabase& data, const std::optional<std::vector<Weight>>& route,
    int from, int to
)
    : from(from), to(to), route(route), RenderMap(doc, data)
{}

const unordered_map<string, void (RenderRoute::*)(void) const> RenderRoute::LAYERS = {
    {"bus_lines",   &RenderRoute::AddBusLines  },
    {"bus_labels",  &RenderRoute::AddBusLabels },
    {"stop_points", &RenderRoute::AddStopPoints},
    {"stop_labels", &RenderRoute::AddStopLabels}
};

void RenderRoute::AddRectangle() const
{
    doc.Add(Svg::Rect{}
                .SetPoint({-map_info.outer_margin, -map_info.outer_margin})
                .SetFillColor(map_info.underlayer_color)
                .SetWidth(map_info.width + 2.0 * map_info.outer_margin)
                .SetHeight(map_info.height + 2.0 * map_info.outer_margin));
}

void RenderRoute::AddBusLines() const
{
    // Color problem
    const auto& stops = data.GetStopsInfo();
    const auto& buses = data.GetBusesInfo();
    const auto& stops_by_id = data.GetStopsById();
    const string& stop_from = stops_by_id[from];
    const string& stop_to = stops_by_id[to];
    string curr_stop;
    for (size_t i = 0; i < route->size(); ++i) {
        if (route->at(i).edge == EdgeType::WAIT) {
            curr_stop = route->at(i).bus_or_stop;
            continue;
        }
        const auto& bus = route->at(i).bus_or_stop;
        Svg::Polyline line = Svg::Polyline{}
                                 .SetStrokeWidth(map_info.line_width)
                                 .SetStrokeLineCap("round")
                                 .SetStrokeLineJoin("round")
                                 .SetStrokeColor(buses.at(bus).color);
        // Point to the line
        const auto& bus_info = buses.at(bus);
        const string& next_sup_stop
            = (i + 1) >= route->size() ? stop_to : route->at(i + 1).bus_or_stop;
        int curr_idx = 0;
        int next_sup_idx = 0;
        int stop_count = route->at(i).stop_count;
        for (int j = 0; j < bus_info.stops.size(); ++j) {
            if (bus_info.stops[j] == curr_stop) {
                curr_idx = j;
                if (curr_idx - stop_count >= 0
                    && bus_info.stops[curr_idx - stop_count] == next_sup_stop) {
                    next_sup_idx = curr_idx - stop_count;
                    break;
                }
                if (curr_idx + stop_count < route->size()
                    && bus_info.stops[curr_idx + stop_count] == next_sup_stop) {
                    next_sup_idx = curr_idx + stop_count;
                    break;
                }
            }
            if (bus_info.stops[j] == next_sup_stop) {
                next_sup_idx = j;
                if (next_sup_idx - stop_count >= 0
                    && bus_info.stops[next_sup_idx - stop_count] == curr_stop) {
                    curr_idx = next_sup_idx - stop_count;
                    break;
                }
                if (next_sup_idx + stop_count < route->size()
                    && bus_info.stops[next_sup_idx + stop_count] == curr_stop)
                {
                    curr_idx = next_sup_idx + stop_count;
                    break;
                }
            }
        }
        auto bounds = std::minmax(curr_idx, next_sup_idx);
        for (size_t j = bounds.first; j <= bounds.second; ++j) {
            auto stop_info = stops.at(buses.at(bus).stops[j]);
            auto [x, y] = compressed_coords->GetCoords(stop_info.name);
            line.AddPoint({x, y});
        }
        doc.Add(move(line));
    }
}

void RenderRoute::AddBusLabels() const
{
    const auto& stops_by_id = data.GetStopsById();
    const string& stop_from = stops_by_id[from];
    const string& stop_to = stops_by_id[to];
    for (size_t i = 0; i < route->size(); ++i) {
        if (route->at(i).edge == EdgeType::WAIT) {
            const string& bus = (i + 1) > route->size() ? route->back().bus_or_stop
                                                        : route->at(i + 1).bus_or_stop;
            const string& curr_stop = route->at(i).bus_or_stop;
            if (curr_stop == buses.at(bus).stops.front()) {
                DrawBusLabel(curr_stop, buses.at(bus).color, bus);
            }
            if (i >= 1
                && (curr_stop == buses.at(route->at(i - 1).bus_or_stop).stops.front()
                    || !buses.at(route->at(i - 1).bus_or_stop).circle
                           && curr_stop == buses.at(route->at(i - 1).bus_or_stop).stops.back()))
            {
                DrawBusLabel(
                    curr_stop, buses.at(route->at(i - 1).bus_or_stop).color,
                    route->at(i - 1).bus_or_stop
                );
            }
            if (!buses.at(bus).circle && curr_stop == buses.at(bus).stops.back()) {
                DrawBusLabel(curr_stop, buses.at(bus).color, bus);
            }
        }
    }
    // Last stop
    const auto& bus = route->back().bus_or_stop;
    if ((!buses.at(bus).circle && stop_to == buses.at(bus).stops.back())
        || stop_to == buses.at(bus).stops.front())
    {
        DrawBusLabel(stop_to, buses.at(bus).color, bus);
    }
}

void RenderRoute::AddStopPoints() const
{
    const auto& stops = data.GetStopsInfo();
    const auto& buses = data.GetBusesInfo();
    const auto& stops_by_id = data.GetStopsById();
    const string& stop_from = stops_by_id[from];
    const string& stop_to = stops_by_id[to];
    string curr_stop;
    for (size_t i = 0; i < route->size(); ++i) {
        if (route->at(i).edge == EdgeType::WAIT) {
            curr_stop = route->at(i).bus_or_stop;
            continue;
        }
        const string& bus = route->at(i).bus_or_stop;
        const BusInfo& bus_info = buses.at(bus);
        const string& next_sup_stop
            = (i + 1) >= route->size() ? stop_to : route->at(i + 1).bus_or_stop;
        int curr_idx = 0;
        int next_sup_idx = 0;
        int stop_count = route->at(i).stop_count;
        for (int j = 0; j < bus_info.stops.size(); ++j) {
            if (bus_info.stops[j] == curr_stop) {
                curr_idx = j;
                if (curr_idx - stop_count >= 0
                    && bus_info.stops[curr_idx - stop_count] == next_sup_stop) {
                    next_sup_idx = curr_idx - stop_count;
                    break;
                }
                if (curr_idx + stop_count < route->size()
                    && bus_info.stops[curr_idx + stop_count] == next_sup_stop) {
                    next_sup_idx = curr_idx + stop_count;
                    break;
                }
            }
            if (bus_info.stops[j] == next_sup_stop) {
                next_sup_idx = j;
                if (next_sup_idx - stop_count >= 0
                    && bus_info.stops[next_sup_idx - stop_count] == curr_stop) {
                    curr_idx = next_sup_idx - stop_count;
                    break;
                }
                if (next_sup_idx + stop_count < route->size()
                    && bus_info.stops[next_sup_idx + stop_count] == curr_stop)
                {
                    curr_idx = next_sup_idx + stop_count;
                    break;
                }
            }
        }
        auto bounds = std::minmax(curr_idx, next_sup_idx);
        for (int j = bounds.first; j <= bounds.second; ++j) {
            auto stop_info = stops.at(buses.at(bus).stops[j]);
            auto [x, y] = compressed_coords->GetCoords(stop_info.name);
            doc.Add(Svg::Circle{}
                        .SetFillColor("white")
                        .SetRadius(map_info.stop_radius)
                        .SetCenter({x, y}));
        }
    }
}

void RenderRoute::DrawStopLabel(const string& stop_name) const
{
    auto [x, y] = compressed_coords->GetCoords(stop_name);
    doc.Add(Svg::Text{}
                .SetFillColor(map_info.underlayer_color)
                .SetStrokeColor(map_info.underlayer_color)
                .SetStrokeWidth(map_info.underlayer_width)
                .SetStrokeLineJoin("round")
                .SetStrokeLineCap("round")
                .SetFontFamily("Verdana")
                .SetFontSize(map_info.stop_label_font_size)
                .SetData(stop_name)
                .SetOffset(map_info.stop_label_offset)
                .SetPoint({x, y}));
    doc.Add(Svg::Text{}
                .SetFillColor("black")
                .SetFontFamily("Verdana")
                .SetFontSize(map_info.stop_label_font_size)
                .SetData(stop_name)
                .SetOffset(map_info.stop_label_offset)
                .SetPoint({x, y}));
}

void RenderRoute::AddStopLabels() const
{
    const auto& stops_by_id = data.GetStopsById();
    const string& stop_to = stops_by_id[to];
    for (size_t i = 0; i < route->size(); ++i) {
        if (route->at(i).edge == EdgeType::WAIT) {
            DrawStopLabel(route->at(i).bus_or_stop);
        }
    }
    // Last stop
    DrawStopLabel(stop_to);
}
