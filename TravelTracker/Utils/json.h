#pragma once

#include <iomanip>
#include <iostream>
#include <istream>
#include <map>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "structures.h"

namespace Json {

    class Node : std::variant<
                     std::vector<Node>, std::map<std::string, Node>, int, double, bool, std::string>
    {
    public:
        using variant::variant;

        const auto& AsArray() const { return std::get<std::vector<Node>>(*this); }

        const auto& AsMap() const { return std::get<std::map<std::string, Node>>(*this); }

        int AsInt() const { return std::get<int>(*this); }

        double AsDouble() const { return std::get<double>(*this); }

        bool AsBool() const { return std::get<bool>(*this); }

        const auto& AsString() const { return std::get<std::string>(*this); }

        /*const auto& AsColor() const {
            return std::get<Svg::Color>(*this);
        }*/
    };

    class Document
    {
    public:
        explicit Document(Node root);

        const Node& GetRoot() const;

    private:
        Node root;
    };

    Document Load(std::istream& input);

}  // namespace Json

namespace JsonParser {
    void JsonBusOutput(BusFullInfo&& bus, std::ostream& out = std::cout);

    void JsonStopOutput(StopFullInfo&& stop, std::ostream& out = std::cout);

    void JsonRouteOutput(
        Svg::Document& doc, const std::optional<std::vector<Weight>>& route, double time_in_road,
        double wait_time, size_t id, std::ostream& out = std::cout
    );

    void JsonMapOutput(const Svg::Document& doc, int id, std::ostream& out = std::cout);
}
