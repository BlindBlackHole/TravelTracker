#include "json.h"
#include <iostream>

using namespace std;

namespace Json {

    Document::Document(Node root) : root(move(root)) {}

    const Node& Document::GetRoot() const
    {
        return root;
    }

    Node LoadNode(istream& input);

    Node LoadArray(istream& input)
    {
        vector<Node> result;

        for (char c; input >> c && c != ']';) {
            if (c != ',') {
                input.putback(c);
            }
            result.push_back(LoadNode(input));
        }

        return Node(move(result));
    }

    Node LoadInt(istream& input)
    {
        int result = 0;
        while (isdigit(input.peek())) {
            result *= 10;
            result += input.get() - '0';
        }
        return Node(result);
    }

    Node LoadNumber(istream& input)
    {
        bool negative = false;
        if (input.peek() == '-') {
            negative = true;
            input.get();
        }
        double result = 0.0;
        while (isdigit(input.peek())) {
            result *= 10;
            result += input.get() - '0';
        }
        if (input.peek() == '.') {
            input.get();
            double frac = 1.0;
            while (isdigit(input.peek())) {
                frac *= 0.1;
                result += (input.get() - '0') * frac;
            }
        }
        if (negative)
            result *= -1.0;
        return Node(result);
    }

    Node LoadBool(istream& input)
    {
        string line;
        if (input.peek() == 't') {
            for (size_t i = 0; i < 4; ++i) {
                line.push_back(input.get());
            }
        }
        else {
            for (size_t i = 0; i < 5; ++i) {
                line.push_back(input.get());
            }
        }
        return Node(line == "true");
    }

    Node LoadString(istream& input)
    {
        string line;
        getline(input, line, '"');
        return Node(move(line));
    }

    Node LoadDict(istream& input)
    {
        map<string, Node> result;

        for (char c; input >> c && c != '}';) {
            if (c == ',') {
                input >> c;
            }

            string key = LoadString(input).AsString();
            input >> c;
            result.emplace(move(key), LoadNode(input));
        }

        return Node(move(result));
    }

    Node LoadNode(istream& input)
    {
        char c;
        input >> c;
        if (c == '[') {
            return LoadArray(input);
        }
        else if (c == '{') {
            return LoadDict(input);
        }
        else if (c == '"') {
            return LoadString(input);
        }
        else if (isdigit(c) || c == '-') {
            input.putback(c);
            return LoadNumber(input);
        }
        else
        // if (c == 't' || c == 'f')
        {
            input.putback(c);
            return LoadBool(input);
        }
    }

    Document Load(istream& input)
    {
        return Document{LoadNode(input)};
    }

}  // namespace Json

namespace JsonParser {
    void JsonBusOutput(BusFullInfo&& bus, ostream& out)
    {
        if (bus.isFound) {
            out << setprecision(6) << "{\n \"route_length\":" << bus.route_length << ",\n"
                << "\"request_id\":" << bus.id << ",\n"
                << "\"curvature\":" << bus.curvature << ",\n"
                << "\"stop_count\":" << bus.stop_count << ",\n"
                << "\"unique_stop_count\":" << bus.unic_stop_count << "\n}";
        }
        else {
            out << "{\n\"request_id\":" << bus.id << ",\n"
                << "\"error_message\": \"not found\"\n}";
        }
    }

    void JsonStopOutput(StopFullInfo&& stop, ostream& out)
    {
        if (stop.isFound) {
            if (stop.buses.size() == 0) {
                out << "{\n\"buses\": [],\n";
            }
            else {
                out << "{\n\"buses\": [\n";
                for (auto it = begin(stop.buses); it != end(stop.buses); ++it) {
                    if (it == prev(end(stop.buses))) {
                        out << "\"" << *it << "\"\n";
                        continue;
                    }
                    out << "\"" << *it << "\",\n";
                }
                out << "],\n";
            }
            out << setprecision(6) << "\"request_id\":" << stop.id << "\n}";
        }
        else {
            out << "{\n\"request_id\":" << stop.id << ",\n"
                << "\"error_message\": \"not found\"\n}";
        }
    }

    void JsonRouteOutput(
        Svg::Document& doc, const std::optional<vector<Weight>>& route, double time_in_road,
        double wait_time, size_t id, ostream& out
    )
    {
        if (!route) {
            out << "{\n\"request_id\":" << id << ",\n"
                << "\"error_message\": \"not found\"\n}";
            return;
        }

        double full_time = time_in_road;

        out << "{\n\"request_id\":" << id << ",\n"
            << "\"link\": \"" << "http://localhost:8080/route/svg" << "\",\n"
            << "\"map\": \"";
        doc.RenderJson(out);
        out << "\",\n"
            << "\"total_time\": " << full_time << ",\n\"items\": [\n";
        for (size_t i = 0; i < route->size(); ++i) {
            if (route->at(i).edge == EdgeType::WAIT) {
                out << "{\n \"time\": " << wait_time << ",\n";
                out << "\"stop_name\": \"" << route->at(i).bus_or_stop << "\",\n";
                out << "\"type\": \"Wait\"\n}";
            }
            else {
                out << "{\n \"time\": " << route->at(i).weight << ",\n";
                out << "\"bus\": \"" << route->at(i).bus_or_stop << "\",\n";
                out << "\"span_count\": " << route->at(i).stop_count << ",\n";
                out << "\"type\": \"Bus\"\n}";
            }

            if (i != route->size() - 1) {
                out << ",\n";
            }
            else {
                out << "\n";
            }
        }
        out << "]}";
    }

    void JsonMapOutput(const Svg::Document& doc, int id, ostream& out)
    {
        out << "{\n\"request_id\":" << id << ",\n";
        out << "\"map\": \"";
        doc.RenderJson(out);
        out << "\"\n}";
    }
}  // namespace JsonParser
