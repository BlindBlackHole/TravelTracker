#pragma once

#include <iostream>
#include <string_view>
#include <vector>
#include <string>
#include <variant>
#include <fstream>
#include <memory>
#include <sstream>
#include <iomanip>
#include <limits>

namespace Svg {

    template<typename FloatingPointType>
    std::string convert(FloatingPointType value)
    {
        std::stringstream ss;
        ss << std::setprecision(std::numeric_limits<FloatingPointType>::digits10 + 1);
        ss << value;
        return ss.str();
    }

    struct Point {
        double x = 0.0;
        double y = 0.0;

        Point() = default;

        Point(double x, double y) : x(x), y(y) {};
    };

    struct Rgb {
        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;

        Rgb() = default;

        Rgb(uint8_t red, uint8_t green, uint8_t blue)
            : red(red), green(green), blue(blue) {}
    };

    struct Rgba {
        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;
        double alpha = 0.0;

        Rgba() = default;

        Rgba(uint8_t red, uint8_t green, uint8_t blue, double alpha)
            : red(red), green(green), blue(blue), alpha(alpha) {}
    };

    using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;
    const Color NoneColor{};

    inline std::string RenderColor(std::monostate) {
        return "none";
    }

    inline std::string RenderColor(const std::string& value) {
        return value;
    }

    inline std::string RenderColor(Rgb rgb) {
        std::string answer;
        answer = "rgb(" + std::to_string(static_cast<int>(rgb.red))
            + "," + std::to_string(static_cast<int>(rgb.green))
            + "," + std::to_string(static_cast<int>(rgb.blue)) + ")";
        return answer;
    }

    inline std::string RenderColor(Rgba rgba) {
        std::string answer;
        answer = "rgba(" + std::to_string(static_cast<int>(rgba.red))
            + "," + std::to_string(static_cast<int>(rgba.green))
            + "," + std::to_string(static_cast<int>(rgba.blue))
            + "," + convert<double>(rgba.alpha) + ")";
        return answer;
    }

    inline std::string RenderColor(const Color& color) {
        return visit([](const auto& value) { return RenderColor(value); },
            color);
    }

    class Object {
    public:
        virtual ~Object() = default;
        virtual std::string GetSvg() const = 0;
        virtual std::string GetSvgJson() const = 0;
    };

    template<typename Owner>
    class PathProps {
    public:
        Owner& SetFillColor(const Color&);
        Owner& SetStrokeColor(const Color&);
        Owner& SetStrokeWidth(double);
        Owner& SetStrokeLineCap(const std::string&);
        Owner& SetStrokeLineJoin(const std::string&);
    private:
        Color fillColor = NoneColor;
        Color strokeColor = NoneColor;
        double strokeWidth = 1.0;
        std::string strokeLineCap;
        std::string strokeLineJoin;

        Owner& AsOwner();
    protected:
        std::string GetDefaultParams() const;
        std::string GetDefaultParamsJson() const;
    };

    template<typename Owner>
    std::string PathProps<Owner>::GetDefaultParams() const {
        std::string answer;
        answer += "fill=\"" + RenderColor(fillColor) + "\" ";
        answer += "stroke=\"" + RenderColor(strokeColor) + "\" ";
        answer += "stroke-width=\"" + std::to_string(strokeWidth) + "\" ";
        if (strokeLineCap != "") {
            answer += "stroke-linecap=\"" + strokeLineCap + "\" ";
        }
        if (strokeLineJoin != "") {
            answer += "stroke-linejoin=\"" + strokeLineJoin + "\" ";
        }
        return answer;
    }

    template<typename Owner>
    std::string PathProps<Owner>::GetDefaultParamsJson() const {
        std::string answer;
        answer += "fill=\\\"" + RenderColor(fillColor) + "\\\" ";
        answer += "stroke=\\\"" + RenderColor(strokeColor) + "\\\" ";
        answer += "stroke-width=\\\"" + convert<double>(strokeWidth) + "\\\" ";
        if (strokeLineCap != "") {
            answer += "stroke-linecap=\\\"" + strokeLineCap + "\\\" ";
        }
        if (strokeLineJoin != "") {
            answer += "stroke-linejoin=\\\"" + strokeLineJoin + "\\\" ";
        }
        return answer;
    }

    template <typename Owner>
    Owner& PathProps<Owner>::AsOwner() {
        return static_cast<Owner&>(*this);
    }

    template <typename Owner>
    Owner& PathProps<Owner>::SetFillColor(const Color& color) {
        fillColor = color;
        return AsOwner();
    }

    template <typename Owner>
    Owner& PathProps<Owner>::SetStrokeColor(const Color& color) {
        strokeColor = color;
        return AsOwner();
    }

    template <typename Owner>
    Owner& PathProps<Owner>::SetStrokeWidth(double value) {
        strokeWidth = value;
        return AsOwner();
    }

    template <typename Owner>
    Owner& PathProps<Owner>::SetStrokeLineCap(const std::string& value) {
        strokeLineCap = value;
        return AsOwner();
    }

    template <typename Owner>
    Owner& PathProps<Owner>::SetStrokeLineJoin(const std::string& value) {
        strokeLineJoin = value;
        return AsOwner();
    }

    class Circle : public Object, public PathProps<Circle> {
    private:
        Point point;
        double radius = 1.0;
        std::string answer = "<circle ";
    public:
        Circle() = default;

        Circle& SetCenter(Point p) {
            point = p;
            return *this;
        }

        Circle& SetRadius(double r) {
            radius = r;
            return *this;
        }

        std::string GetSvg() const override {
            std::string answer = "<circle ";
            answer += "cx=\"" + std::to_string(point.x) + "\" ";
            answer += "cy=\"" + std::to_string(point.y) + "\" ";
            answer += "r=\"" + std::to_string(radius) + "\" ";
            answer += GetDefaultParams();
            answer += "/>";
            return answer;
        }

        std::string GetSvgJson() const override {
            std::string answer = "<circle ";
            answer += "cx=\\\"" + convert<double>(point.x) + "\\\" ";
            answer += "cy=\\\"" + convert<double>(point.y) + "\\\" ";
            answer += "r=\\\"" + convert<double>(radius) + "\\\" ";
            answer += GetDefaultParamsJson();
            answer += "/>";
            return answer;
        }

    };

    class Polyline : public Object, public PathProps<Polyline> {
    private:
        std::vector<Point> points;
    public:
        Polyline() = default;

        Polyline& AddPoint(Point p) {
            points.push_back(p);
            return *this;
        }

        std::string GetSvg() const override {
            std::string answer = "<polyline ";
            answer += "points=\"";
            for (const auto& p : points) {
                answer += std::to_string(p.x) + "," + std::to_string(p.y) + " ";
            }
            answer += "\" ";
            answer += GetDefaultParams();
            answer += "/>";
            return answer;
        }

        std::string GetSvgJson() const override {
            std::string answer = "<polyline ";
            answer += "points=\\\"";
            for (const auto& p : points) {
                answer += convert<double>(p.x) + "," + convert<double>(p.y) + " ";
            }
            answer += "\\\" ";
            answer += GetDefaultParamsJson();
            answer += "/>";
            return answer;
        }

    };

    class Text : public Object, public PathProps<Text> {
    private:
        Point point;
        Point offset;
        uint32_t fontSize = 1;
        std::string fontFamily;
        std::string data;
        std::string fontWeight;
    public:
        Text() = default;

        Text& SetPoint(Point p) {
            point = p;
            return *this;
        }
        Text& SetOffset(Point ofs) {
            offset = ofs;
            return *this;
        }
        Text& SetFontSize(uint32_t fs) {
            fontSize = fs;
            return *this;
        }
        Text& SetFontFamily(const std::string& font) {
            fontFamily = font;
            return *this;
        }
        Text& SetData(const std::string& d) {
            data = d;
            return *this;
        }

        Text& SetFontWeight(const std::string& f_w) {
            fontWeight = f_w;
            return *this;
        }

        std::string GetSvg() const override {
            std::string answer = "<text ";
            answer += "x=\"" + std::to_string(point.x) + "\" ";
            answer += "y=\"" + std::to_string(point.y) + "\" ";
            answer += "dx=\"" + std::to_string(offset.x) + "\" ";
            answer += "dy=\"" + std::to_string(offset.y) + "\" ";
            answer += "font-size=\"" + std::to_string(fontSize) + "\" ";
            if (fontFamily != "") {
                answer += "font-family=\"" + fontFamily + "\" ";
            }
            if (fontWeight != "") {
                answer += "font-weight=\"" + fontWeight + "\" ";
            }
            answer += GetDefaultParams();
            answer += ">" + data + "</text>";
            return answer;
        }

        std::string GetSvgJson() const override {
            std::string answer = "<text ";
            answer += "x=\\\"" + convert<double>(point.x) + "\\\" ";
            answer += "y=\\\"" + convert<double>(point.y) + "\\\" ";
            answer += "dx=\\\"" + convert<double>(offset.x) + "\\\" ";
            answer += "dy=\\\"" + convert<double>(offset.y) + "\\\" ";
            answer += "font-size=\\\"" + convert<double>(fontSize) + "\\\" ";
            if (fontFamily != "") {
                answer += "font-family=\\\"" + fontFamily + "\\\" ";
            }
            if (fontWeight != "") {
                answer += "font-weight=\\\"" + fontWeight + "\\\" ";
            }
            answer += GetDefaultParamsJson();
            answer += ">" + data + "</text>";
            return answer;
        }
    };


    class Rect : public Object, public PathProps<Rect> {
    private:
        Point point;
        double width, height;
    public:
        Rect() = default;

        Rect& SetPoint(Point p) {
            point = p;
            return *this;
        }

        Rect& SetWidth(double w) {
            width = w;
            return *this;
        }

        Rect& SetHeight(double h) {
            height = h;
            return *this;
        }

        std::string GetSvg() const override {
            std::string answer = "<rect ";
            answer += "x=\"" + std::to_string(point.x) + "\" ";
            answer += "y=\"" + std::to_string(point.y) + "\" ";
            answer += "width=\"" + std::to_string(width) + "\" ";
            answer += "height=\"" + std::to_string(height) + "\" ";
            answer += GetDefaultParams();
            answer += "/>";
            return answer;
        }

        std::string GetSvgJson() const override {
            std::string answer = "<rect ";
            answer += "x=\\\"" + convert<double>(point.x) + "\\\" ";
            answer += "y=\\\"" + convert<double>(point.y) + "\\\" ";
            answer += "width=\\\"" + convert<double>(width) + "\\\" ";
            answer += "height=\\\"" + convert<double>(height) + "\\\" ";
            answer += GetDefaultParamsJson();
            answer += "/>";
            return answer;
        }
    };

    class Document {
    private:
        std::vector<std::unique_ptr<Object>> data;
    public:
        Document() = default;

        template<typename ObjectType>
        void Add(ObjectType obj) {
            data.push_back(std::make_unique<ObjectType>(std::move(obj)));
        }

        void Render(std::ostream& out) const {
            std::string result;
            result = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>";
            result += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">";

            for (auto& d : data) {
                result += d->GetSvg();
            }

            result += "</svg>";
            out << result;
        }

        void RenderJson(std::ostream& out) const {
            std::string result;
            result = "<?xml version=\\\"1.0\\\" encoding=\\\"UTF-8\\\" ?>";
            result += "<svg xmlns=\\\"http://www.w3.org/2000/svg\\\" version=\\\"1.1\\\">";

            for (auto& d : data) {
                result += d->GetSvgJson();
            }

            result += "</svg>";
            out << result;
        }
        double a = 0.0;
    };

}
