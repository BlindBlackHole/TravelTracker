#include "queries.h"
#include "layers.h"

#include <cmath>

using namespace std;

using GraphPtr = shared_ptr<Graph::DirectedWeightedGraph<Weight>>;
using RouterPtr = shared_ptr<Graph::Router<Weight>>;

//--QueryStop
QueryStop::QueryStop(RouteDatabase& data, const string& stop_name, int id, ostream& out)
    : data(data), out(out), stop_name(stop_name), id(id)
{}

QueryStop& QueryStop::Procces()
{
    const auto& stops_info = data.GetStopsInfo();
    auto stop = stops_info.find(stop_name);
    if (stop == end(stops_info)) {
        stop_info = StopFullInfo({false, {}, id});
        return *this;
    }
    stop_info = StopFullInfo({true, stop->second.busses, id});
    return *this;
}

QueryStop& QueryStop::GetResult()
{
    JsonParser::JsonStopOutput(move(stop_info), out);
    return *this;
}

// QueryStop--

//--QueryBus
QueryBus::QueryBus(RouteDatabase& data, const string& bus_name, int id, ostream& out)
    : data(data), out(out), bus_name(bus_name), id(id)
{}

double QueryBus::ConvertCoords(double coord) const
{
    return coord * PI / 180;
}

double QueryBus::GetLength(const StopInfo& lhs, const StopInfo& rhs) const
{
    return acos(
               sin(ConvertCoords(lhs.latitude)) * sin(ConvertCoords(rhs.latitude))
               + cos(ConvertCoords(lhs.latitude)) * cos(ConvertCoords(rhs.latitude))
                     * cos(abs(ConvertCoords(lhs.longtitude) - ConvertCoords(rhs.longtitude)))
           )
         * 6371000;
}

QueryBus& QueryBus::Procces()
{
    const auto& busses_info = data.GetBusesInfo();
    auto bus = busses_info.find(bus_name);
    if (bus == busses_info.end()) {
        bus_info = BusFullInfo({false, bus_name, id, unic_stop_count, 0.0, 0.0, stop_count});
        return *this;
    }
    Route route = GetRoute<1>(bus->second.stops);
    if (bus->second.circle) {
        stop_count = bus->second.stops.size();
    }
    else {
        stop_count = bus->second.stops.size() * 2 - 1;
        route += GetRoute<-1>(bus->second.stops);
    }

    double curvature = route.GetCur();

    unic_stop_count = bus->second.unic_stops.size();

    bus_info
        = BusFullInfo({true, bus_name, id, unic_stop_count, route.route_full, curvature, stop_count}
        );
    return *this;
}

QueryBus& QueryBus::GetResult()
{
    JsonParser::JsonBusOutput(move(bus_info), out);
    return *this;
}

//--QueryBus

//--QueryRoute
QueryRoute::QueryRoute(
    RouteDatabase& data, Map& map, GraphPtr graph, RouterPtr router, Graph::VertexId from,
    Graph::VertexId to, int id, ostream& out
)
    : data(data), out(out), map_(map), from(from), to(to), id(id)
{
    graph_ = graph;
    router_ = router;
}

QueryRoute& QueryRoute::Procces()
{
    route_ = map_.CreateRoute(graph_, router_, from, to);
    // Firstly map rendering
    RenderMap routes_map(doc, data);
    const vector<string>& render_layers = data.GetMapInfo().layers;
    for (const string& render_layer : render_layers) {
        auto method_ptr = routes_map.LAYERS.at(render_layer);
        (routes_map.*method_ptr)();
    }
    // Secondly roure rendering
    RenderRoute route(doc, data, route_, from, to);
    route.AddRectangle();
    for (const string& render_layer : render_layers) {
        auto method_ptr = route.LAYERS.at(render_layer);
        (route.*method_ptr)();
    }
    return *this;
}

QueryRoute& QueryRoute::GetResult()
{
    ofstream out("route" + to_string(id) + ".svg");
    // ofstream outdata("data" + to_string(id) + ".txt");
    doc.Render(out);
    JsonParser::JsonRouteOutput(
        doc, route_, map_.GetTimeInRoad(), data.GetRouteInfo().bus_wait_time, id, this->out
    );
    return *this;
}

// QueryRoute--

//--QueryMap
QueryMap::QueryMap(const RouteDatabase& data, int id, std::ostream& out) : data(data), id(id), out(out) {}

QueryMap& QueryMap::Procces()
{
    RenderMap layers(doc, data);
    const vector<string>& render_layers = data.GetMapInfo().layers;
    for (const string& render_layer : render_layers) {
        auto method_ptr = layers.LAYERS.at(render_layer);
        (layers.*method_ptr)();
    }
    return *this;
}

QueryMap& QueryMap::GetResult()
{
    ofstream out("map" + to_string(id) + ".svg");
    doc.Render(out);
    JsonParser::JsonMapOutput(doc, id, this->out);
    return *this;
}

// QueryMap--
