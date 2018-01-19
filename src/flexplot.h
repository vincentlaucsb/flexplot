#pragma once
#include <iostream>
#include <algorithm> // min, max
#include <fstream> // ofstream
#include <math.h>  // NAN
#include <unordered_map>
#include <map>
#include <deque>
#include <vector>
#include <string>
#include <memory>

using std::vector;
using std::string;

namespace SVG {
    class Element {
    public:
        Element() {};
        Element(
            std::string _tag,
            std::map < std::string, std::string > _attr) :
            attr(_attr),
            tag(_tag) {};

        template<typename T>
        inline Element& set_attr(std::string key, T value) {
            this->attr[key] = std::to_string(value);
            return *this;
        }

        template<typename T, typename... Args>
        inline void add_child(T node, Args... args) {
            add_child(node);
            add_child(args...);
        }

        template<typename T>
        inline Element* add_child(T node) {
            /** Also return a pointer to the element added */
            this->children.push_back(std::make_shared<T>(node));
            return this->children.back().get();
        }

        inline virtual float get_width() {
            if (attr.find("width") != attr.end())
                return std::stof(attr["width"]);
            else
                return NAN;
        }

        inline virtual float get_height() {
            if (attr.find("height") != attr.end())
                return std::stof(attr["height"]);
            else
                return NAN;
        }

        virtual std::string to_string();
        std::map < std::string, std::string > attr;
        std::string content;

    protected:
        std::string tag;
        std::vector<std::shared_ptr<Element>> children;
    };

    template<>
    inline Element& Element::set_attr(std::string key, const char * value) {
        this->attr[key] = value;
        return *this;
    }

    template<>
    inline Element& Element::set_attr(std::string key, const std::string value) {
        this->attr[key] = value;
        return *this;
    }

    class SVG : public Element {
    public:
        SVG(std::map < std::string, std::string > _attr =
        { { "xmlns", "http://www.w3.org/2000/svg" } }
        ) : Element("svg", _attr) {};
    };

    class Path : public Element {
    public:
        Path() : Element("path", {}) {};

        template<typename T>
        inline void start(T x, T y) {
            /** Start line at (x, y)
            *  This function overwrites the current path if it exists
            */
            this->attr["d"] = "M " + std::to_string(x) + " " + std::to_string(y);
            this->x_start = x;
            this->y_start = y;
        }

        template<typename T>
        inline void line_to(T x, T y) {
            /** Draw a line to (x, y)
            *  If line has not been initialized by setting a starting point,
            *  then start() will be called with (x, y) as arguments
            */

            if (this->attr.find("d") == this->attr.end())
                start(x, y);
            else
                this->attr["d"] += " L " + std::to_string(x) +
                " " + std::to_string(y);
        }

        inline void to_origin() {
            /** Draw a line back to the origin */
            this->line_to(x_start, y_start);
        }

    private:
        float x_start;
        float y_start;
    };

    class Text : public Element {
    public:
        Text() { tag = "text"; };
        Text(int x, int y, std::string _content) {
            tag = "text";
            set_attr("x", std::to_string(x));
            set_attr("y", std::to_string(y));
            content = _content;
        }

        std::string to_string() override;
    };

    class Group : public Element {
    public:
        Group() : Element("g", {}) {};
    };

    class Line : public Element {
    public:
        Line() {};
        Line(int x1, int x2, int y1, int y2) : Element("line", {
            { "x1", std::to_string(x1) },
            { "x2", std::to_string(x2) },
            { "y1", std::to_string(y1) },
            { "y2", std::to_string(y2) }
        }) {};

        inline float x1() { return std::stof(this->attr["x1"]); }
        inline float x2() { return std::stof(this->attr["x2"]); }
        inline float y1() { return std::stof(this->attr["y1"]); }
        inline float y2() { return std::stof(this->attr["y2"]); }

        inline float get_width() override;
        inline float get_height() override;
        inline float get_length();
        inline float get_slope();

        std::pair<float, float> along(float percent);
    };

    class Rect : public Element {
    public:
        Rect(
            int x, int y, double width, double height) :
            Element("rect", {
                { "x", std::to_string(x) },
                { "y", std::to_string(y) },
                { "width", std::to_string(width) },
                { "height", std::to_string(height) }
            }) {};
    };

    class Circle : public Element {
    public:
        Circle(int cx, int cy, int radius) :
            Element("circle", {
                { "cx", std::to_string(cx) },
                { "cy", std::to_string(cy) },
                { "r", std::to_string(radius) }
            }) {};
    };
}

namespace Graphs {
    template <class T>
    struct GraphData {
        GraphData() {};
        GraphData(const std::vector<T> x, const std::vector<long double> y) :
            x_values(x), y_values(y) {
            if (x.size() != y.size()) {
                throw std::runtime_error("Number of labels does not match number of heights.");
            }
        };

        inline const size_t GraphData<T>::size() { return x_values.size(); }
        inline long double GraphData<T>::y_min() {
            /** Return the lowest y value */
            return *(std::min_element(y_values.begin(), y_values.end()));
        }

        inline long double GraphData<T>::y_max() {
            /** Return the highest y value */
            return *(std::max_element(y_values.begin(), y_values.end()));
        }

        std::vector<std::string> y_labels(size_t labels=5) {
            std::vector<std::string> ret_labels;

            // Set bin labels to left-hand boundary values
            for (size_t i = 0; i <= labels; i++) {
                ret_labels.push_back(std::to_string(
                    y_min() + i*(y_max() - y_min()) / labels));
            }

            return ret_labels;
        }

        std::vector<T> x_values;
        std::vector<long double> y_values;
    };

    /** Data used to generate bar plots, histograms, etc. */
    class CategoricalData : public GraphData<std::string> {
    public:
        using GraphData<std::string>::GraphData;
        inline long double min_height() { return this->y_min(); }
        inline long double max_height() { return this->y_max(); }
    };

    class NumericData : public GraphData<long double> {
    public:
        using GraphData<long double>::GraphData;
        std::vector<std::string> x_labels(size_t max_labels = 20);

        inline long double x_min() {
            /** Return the lowest x value */
            return *(std::min_element(x_values.begin(), x_values.end()));
        }

        inline long double x_max() {
            /** Return the highest x value */
            return *(std::max_element(x_values.begin(), x_values.end()));
        }
    };


    /** Defines a mapping from the data space to the SVG coordinate space */
    class CartesianCoordinates {
    public:
        CartesianCoordinates() {};
        CartesianCoordinates(float _x1, float _x2, float _y1, float _y2) :
            x1(_x1), x2(_x2), y1(_y1), y2(_y2) {};

        template<typename T>
        inline float map_x(T x) {
            return x1 + (x2 - x1) * ((x - domain_min) / (domain_max - domain_min));
        };

        template<typename T>
        inline float map_y(T y) {
            return y2 - (y2 - y1) * ((y - range_min) / (range_max - range_min));
        };

        template<typename T>
        inline std::pair<float, float> map(T x, T y) {
            float ret_x = map_x(x), ret_y = map_y(y);
            return std::make_pair(ret_x, ret_y);
        };

        void set_data(CategoricalData& data);
        void set_data(NumericData& data);

        float x1;
        float x2;
        float y1;
        float y2;

        long double domain_min = NAN;
        long double domain_max = NAN;
        long double range_min = NAN;
        long double range_max = NAN;
    };

    /** Defines a mapping from polar coordinates to the SVG coordinate space */
    class PolarCoordinates {
    public:
        PolarCoordinates(float cx, float cy, float cr) :
            x(cx), y(cy), radius(cr) {};
        std::pair<float, float> map(float degrees, float percent = 1);
        std::pair<float, float> center();
    private:
        float x;
        float y;
        float radius;
    };

    struct GraphOptions {
        int width;
        int height;
        int margin_left;    /*< Also includes space for y-axis labels */
        int margin_right;
        int margin_bottom;  /*< Also includes space for x-axis labels */
        int margin_top;     /*< Also includes space for title */
        std::string title;
        std::string x_label;
        std::string y_label;
    };

    const GraphOptions DEFAULT_GRAPH = { 800, 400, 50, 50, 100, 50, "", "", "" };

    /** Base class for all plots */
    class Plot {
    public:
        void to_svg(const std::string filename);
        SVG::SVG root;
    };

    /** This provides a base class for any plot that displays objects on an XY grid.
     *
     *  For the purposes of this library, the "margin" of a graph is defined as the
     *  area not containing the main graphical element, e.g. bars or points. The margin
     *  is the area which contains the title, subtitles, tick marks, etc. Thus, the main
     *  drawing area is the area not inside the margins. For convienience, any methods
     *  which create SVG elements may use the methods x1(), x2(), y1(), and y2()
     *  to get the boundaries of the drawing area.
     *
     *  T: Should be either CategoricalData or NumericData
     */
    template<class T>
    class Graph: public Plot {
    public:
        Graph(GraphOptions options = DEFAULT_GRAPH) :
            width(options.width), height(options.height) {
            this->rect = CartesianCoordinates(
                options.margin_left,            // x1
                width - options.margin_right,   // x2
                options.margin_top,             // y1
                height - options.margin_bottom  // y2
            );
            this->root.set_attr("width", width).set_attr("height", height);

            // Make title
            SVG::SVG title_wrapper;
            title_wrapper.set_attr("width", width)
                .set_attr("height", options.margin_top);

            SVG::Text title;
            title.set_attr("x", "50%").set_attr("y", "50%")
                .set_attr("style", "font-family: sans-serif; font-size: 24px;")
                .set_attr("dominant-baseline", "central")
                .set_attr("text-anchor", "middle");

            // Make x-axis label;
            SVG::SVG xlab_wrapper;
            xlab_wrapper.set_attr("width", width).set_attr("height", 25)
                .set_attr("x", 0).set_attr("y", height - 25);

            SVG::Text xlab;
            xlab.set_attr("x", "50%").set_attr("y", "50%")
                .set_attr("style", "font-family: sans-serif; font-size: 16px;")
                .set_attr("dominant-baseline", "central")
                .set_attr("text-anchor", "middle");

            // Make y-axis label
            SVG::SVG ylab_wrapper;
            ylab_wrapper.set_attr("width", height).set_attr("height", 25)
                .set_attr("x", 0).set_attr("y", 0)
                .set_attr("transform", "translate(" +
                    std::to_string(0) + "," + std::to_string(height) +
                    ") rotate(-90)");

            SVG::Text ylab;
            ylab.set_attr("x", "50%").set_attr("y", "50%")
                .set_attr("style", "font-family: sans-serif; font-size: 16px;")
                .set_attr("dominant-baseline", "central")
                .set_attr("text-anchor", "middle");

            // Make axis lines
            this->x_axis_group = (SVG::Group*)this->root.add_child(SVG::Group());
            this->y_axis_group = (SVG::Group*)this->root.add_child(SVG::Group());

            this->x_axis = (SVG::Line*)x_axis_group->add_child(
                SVG::Line(rect.x1, rect.x2, rect.y2, rect.y2));
            x_axis->set_attr("stroke", "#cccccc").set_attr("stroke-width", 1);
            this->y_axis = (SVG::Line*)y_axis_group->add_child(
                SVG::Line(rect.x1, rect.x1, rect.y1, rect.y2));
            y_axis->set_attr("stroke", "#cccccc").set_attr("stroke-width", 1);

            // So we can dynamically change text content later
            this->title = title_wrapper.add_child(title);
            this->xlab = xlab_wrapper.add_child(xlab);
            this->ylab = ylab_wrapper.add_child(ylab);
            this->root.add_child(title_wrapper, xlab_wrapper, ylab_wrapper);
        }

        int width;
        int height;

    protected:
        CartesianCoordinates rect; /*< Used to map stuff onto the drawing area */
        void make_x_axis(T data);
        void make_y_axis(T data);

        int bar_spacing = 2;
        int tick_size = 5;

        SVG::Element* title = nullptr; /*< Pointer set by constructor */
        SVG::Element* xlab = nullptr;
        SVG::Element* ylab = nullptr;

        SVG::Group* x_axis_group = nullptr;
        SVG::Group* y_axis_group = nullptr;

        SVG::Line* x_axis = nullptr;
        SVG::Line* y_axis = nullptr;
    };

    class BarChart : public Graph<CategoricalData> {
    public:
        BarChart(CategoricalData Data, GraphOptions options = DEFAULT_GRAPH);
        void generate(CategoricalData data);

    protected:
        SVG::Group make_bars(CategoricalData& data);
        SVG::Group bars;
    };

    class Scatterplot : public Graph<NumericData> {
    public:
        Scatterplot(NumericData data, GraphOptions options = DEFAULT_GRAPH);
        void generate(NumericData data);

    protected:
        NumericData data;
        SVG::Group make_dots(const size_t dot_radius=2);
    };

    class RadarChart : public Plot {
    public:
        RadarChart(size_t axes);
        void plot_points(vector<float> percentages);
    private:
        PolarCoordinates polar = { 250, 250, 250 };
        void make_axis(size_t axes);

        size_t n_axes;
        std::vector<SVG::Line*> axes;
    };

    /**
     * Class for multi-graph SVGs
    class Matrix: public Plot {
    public:
        Matrix(int _cols) : cols(_cols) {};
        void add_graph(Graph graph);
        void generate();

    private:
        std::deque<Graph> graphs;
        int cols;
        int width_per_graph = 500;
        int height_per_graph = 400;
    };
    */

    class ColumnNotFoundError : public std::runtime_error {
    public:
        ColumnNotFoundError(const std::string& col_name) : std::runtime_error(
            "Couldn't find a column named " + col_name) {};
    };
}