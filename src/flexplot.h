#pragma once
#include <iostream>
#include <algorithm> // min, max
#include <fstream>   // ofstream
#include <math.h>    // NAN
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

        inline void line_to(std::pair<float, float> coord) {
            this->line_to(coord.first, coord.second);
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
        Text(float x, float y, std::string _content) {
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
        Line(float x1, float x2, float y1, float y2) : Element("line", {
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
        Rect() {};
        Rect(
            float x, float y, float width, float height) :
            Element("rect", {
                { "x", std::to_string(x) },
                { "y", std::to_string(y) },
                { "width", std::to_string(width) },
                { "height", std::to_string(height) }
            }) {};
    };

    class Circle : public Element {
    public:
        Circle() {};

        Circle(float cx, float cy, float radius) :
            Element("circle", {
                { "cx", std::to_string(cx) },
                { "cy", std::to_string(cy) },
                { "r", std::to_string(radius) }
            }) {
        };

        Circle(std::pair<float, float> xy, float radius) : Circle(xy.first, xy.second, radius) {
        };
    };
}

namespace Graphs {
    const std::vector<std::string> QUALITATIVE_COLORS = {
        "#a6cee3", "#1f78b4", "#b2df8a", "#33a02c",
        "#fb9a99", "#e31a1c", "#fdbf6f", "#ff7f00",
        "#cab2d6", "#6a3d9a", "#ffff99", "#b1592"
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

    const GraphOptions DEFAULT_GRAPH = { 800, 400, 100, 50, 100, 50, "", "", "" };
    const GraphOptions DEFAULT_GRAPH_LEGEND = { 800, 400, 100, 200, 100, 50, "", "", "" };

    /** Abstract base class for Dataset* */
    class DatasetBase {
    public:
        virtual const size_t size() = 0;
        virtual long double x_min() = 0;
        virtual long double x_max() = 0;
        virtual long double y_min() = 0;
        virtual long double y_max() = 0;
        virtual std::vector<std::string> x_labels(size_t max_labels = 20);
        std::vector<std::string> y_labels(size_t labels=5);
    };

    template <class T>
    struct Dataset: public DatasetBase {
        Dataset() {};
        Dataset(const std::vector<T> x, const std::vector<long double> y) :
            x_values(x), y_values(y) {
            if (x.size() != y.size()) {
                throw std::runtime_error("Number of labels does not match number of heights.");
            }
        };

        inline const size_t size() override { return x_values.size(); }
        inline long double x_min() override { return NAN; }
        inline long double x_max() override { return NAN; }
        inline long double y_min() override {
            /** Return the lowest y value */
            return *(std::min_element(y_values.begin(), y_values.end()));
        }

        inline long double y_max() override {
            /** Return the highest y value */
            return *(std::max_element(y_values.begin(), y_values.end()));
        }

        std::vector<T> x_values;
        std::vector<long double> y_values;
    };

    /** A collection of Dataset */
    template<class T>
    class DatasetCollection: public DatasetBase {
    public:
        DatasetCollection() {};
        std::vector<T> datasets;

        DatasetCollection<T>& operator+ (T& data) {
            /** Append data to the set */
            datasets.push_back(data);
            return *this;
        }

        inline const size_t size() override { 
            if (datasets.empty())
                return 0;
            return datasets.begin()->size();
        }

        inline long double x_min() override { return NAN; }
        inline long double x_max() override { return NAN; }
        inline long double y_min() override {
            /** Return the lowest y value in the collection of data */
            long double min = NAN;
            for (auto it = datasets.begin(); it != datasets.end(); ++it)
                if (isnan(min) || it->y_min() < min) min = it->y_min();

            return min;
        }

        inline long double y_max() override {
            /** Return the largest y value in the collection of data */
            long double max = NAN;
            for (auto it = datasets.begin(); it != datasets.end(); ++it)
                if (isnan(max) || it->y_max() > max) max = it->y_max();

            return max;
        }
    };

    /** Data used to plot bar plots, histograms, etc. */
    class CategoricalData : public Dataset<std::string> {
    public:
        using Dataset<std::string>::Dataset;
        using Dataset<std::string>::size;

        inline std::vector<std::string> x_labels(size_t max_labels=20) override { return this->x_values;  }
        inline long double min_height() { return this->y_min(); }
        inline long double max_height() { return this->y_max(); }
    };

    class NumericData : public Dataset<long double> {
    public:
        NumericData(
            const std::vector<long double> x,
            const std::vector<long double> y,
            const std::vector<long double> z
        );

        using Dataset<long double>::Dataset;
        using Dataset<long double>::size;

        DatasetCollection<NumericData> operator+ (NumericData& other);

        inline long double x_min() override {
            /** Return the lowest x value */
            return *(std::min_element(x_values.begin(), x_values.end()));
        }

        inline long double x_max() override {
            /** Return the highest x value */
            return *(std::max_element(x_values.begin(), x_values.end()));
        }

        std::vector<long double> z_values = {};
    };

    /** Defines a mapping from the data space to the SVG coordinate space
     *  Having multiple data sets on the same plot involves adjusting the coordinate system
     */
    class CartesianCoordinates {
    public:
        CartesianCoordinates() {};
        CartesianCoordinates(const GraphOptions& options, DatasetBase& data) :
            x1((float)options.margin_left),
            x2((float)options.width - (float)options.margin_right),
            y1((float)options.margin_top),
            y2((float)options.height - (float)options.margin_bottom) {
            domain_min = data.x_min();
            domain_max = data.x_max();
            range_min = data.y_min();
            range_max = data.y_max();
        }

        template<typename T>
        inline float map_x(T x) {
            return (float)(x1 + (x2 - x1) * ((x - domain_min) / (domain_max - domain_min)));
        };

        template<typename T>
        inline float map_y(T y) {
            return (float)(y2 - (y2 - y1) * ((y - range_min) / (range_max - range_min)));
        };

        template<typename T>
        inline std::pair<float, float> map(T x, T y) {
            float ret_x = map_x(x), ret_y = map_y(y);
            return std::make_pair(ret_x, ret_y);
        };

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
        float radius;
    private:
        float x;
        float y;
    };

    /** Base class for all plots */
    class PlotBase {
    public:
        PlotBase(GraphOptions _options = DEFAULT_GRAPH) : options(_options) {};
        void to_svg(const std::string filename);

    protected:
        SVG::SVG root;
        GraphOptions options;
    };

    /** Container for a vertically-oriented legend */
    class Legend {
    public:
        Legend() {};

        std::vector<std::string> fills;
        std::vector<std::string> labels;

        void generate();
        float get_height();

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
    template<typename T>
    class Graph: public PlotBase {
    public:
        Graph(GraphOptions _options = DEFAULT_GRAPH);
        
        SVG::Group make_bar(T& data, const std::string color = QUALITATIVE_COLORS[0]);
        SVG::Group make_point(T& data, const std::string color = QUALITATIVE_COLORS[0]);
        SVG::Path make_line(T& data, const std::string color = QUALITATIVE_COLORS[0]);

        inline void plot(T& data) {
            this->rect = CartesianCoordinates(this->options, data);
            this->make_x_axis(data);
            this->make_y_axis(data);
        }

    protected:
        CartesianCoordinates rect; /*< Used to map stuff onto the drawing area */

        void make_x_axis(DatasetBase &data);
        void make_y_axis(DatasetBase &data);

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

    template<class T>
    Graph<T>::Graph(GraphOptions _options) : PlotBase(_options) {
        int width = _options.width;
        int height = _options.height;

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

        // So we can dynamically change text content later
        this->title = title_wrapper.add_child(title);
        this->xlab = xlab_wrapper.add_child(xlab);
        this->ylab = ylab_wrapper.add_child(ylab);
        this->root.add_child(title_wrapper, xlab_wrapper, ylab_wrapper);

        this->title->content = options.title;
        this->xlab->content = options.x_label;
        this->ylab->content = options.y_label;
    }

    template<class T>
    void Graph<T>::make_x_axis(DatasetBase &data) {
        /** plot the x-axis (lines, ticks, labels)
        *
        *  @param[out] align Specifies whether labels should be
        *                    left or center aligned wrt the bars
        */

        this->x_axis_group = (SVG::Group*)this->root.add_child(SVG::Group());
        this->x_axis = (SVG::Line*)x_axis_group->add_child(
            SVG::Line(rect.x1, rect.x2, rect.y2, rect.y2));
        x_axis->set_attr("stroke", "#cccccc").set_attr("stroke-width", 1);

        std::vector<std::string> x_labels = data.x_labels();
        SVG::Group ticks, tick_text;
        std::pair<float, float> coord;

        ticks.set_attr("stroke-width", 1).set_attr("stroke", "#000000");
        tick_text.set_attr("style", "font-family: sans-serif; font-size: 12px;")
            .set_attr("text-anchor", "left");

        // Labels for numeric data always have n + 1 labels
        // Use offset to center text
        float offset = offset = 0.5/(float)data.size();
        if (x_labels.size() > data.size())
            offset = 0;

        // Add tick marks
        for (float i = 0, n = (float)data.size(); i <= n; i++) {
            if (x_labels.size() == i)
                break;

            coord = x_axis->along(i / n + offset);
            ticks.add_child(SVG::Line(
                coord.first, coord.first,
                coord.second, coord.second + (float)tick_size
            ));

            // Use translate() to set location rather than x, y
            // attributes so rotation works properly
            SVG::Text label(0, 0, x_labels[i]);
            label.set_attr("transform", "translate(" +
                std::to_string(coord.first) + "," +
                std::to_string(rect.y2 + tick_size + 10) // Space label further south from ticks
                + ") rotate(75)");
            tick_text.add_child(label);
        }

        this->x_axis_group->add_child(ticks, tick_text);
    }

    template <class T>
    void Graph<T>::make_y_axis(DatasetBase &data) {
        const size_t num_labels = 10;
        this->y_axis_group = (SVG::Group*)this->root.add_child(SVG::Group());
        this->y_axis = (SVG::Line*)y_axis_group->add_child(
            SVG::Line(rect.x1, rect.x1, rect.y1, rect.y2));
        y_axis->set_attr("stroke", "#cccccc").set_attr("stroke-width", 1);

        std::vector<std::string> y_labels = data.y_labels(10);
        SVG::Group ticks, tick_text;
        std::pair<float, float> coord;

        ticks.set_attr("stroke-width", 1).set_attr("stroke", "#000000");
        tick_text.set_attr("style", "font-family: sans-serif;"
            "font-size: 12px;").set_attr("text-anchor", "end");

        // Add 10 y-axis tick marks starting from the bottom, moving upwards
        // Ticks are represented as tiny lines
        for (size_t i = 0; i < num_labels; i++) {
            coord = y_axis->along((float)(num_labels - i) / num_labels);
            ticks.add_child(SVG::Line(
                coord.first - 5, coord.first, coord.second, coord.second));
            tick_text.add_child(SVG::Text(coord.first - 5, coord.second, y_labels[i]));
        }

        this->y_axis_group->add_child(ticks, tick_text);
    }

    template<>
    inline SVG::Group Graph<CategoricalData>::make_bar(
        CategoricalData& data, const std::string color) {
        /** Distribute bars evenly across graph canvas */
        SVG::Group bars;
        bars.set_attr("fill", color);

        std::pair<float, float> coord;
        const float max_height = rect.y2 - rect.y1;
        float x_tick_space = (rect.x2 - rect.x1) / data.size();
        float temp_x1 = rect.x1;
        float bar_height;

        // Add a bar for every bin
        for (auto it = data.y_values.begin(); it != data.y_values.end(); ++it) {
            bar_height = (*it / (rect.range_max - rect.range_min)) * (rect.y2 - rect.y1);
            bars.add_child(SVG::Rect(temp_x1, rect.y2 - bar_height,
                x_tick_space - bar_spacing, bar_height));
            temp_x1 += x_tick_space;
        }

        // Temporary
        this->root.add_child(bars);
        return bars;
    }

    template<class T>
    inline SVG::Group Graph<T>::make_point(T& data, const std::string color) {
        std::pair<float, float> coord;
        float dot_radius = 2;
        SVG::Group dots;
        dots.set_attr("fill", color);

        // Add each dot
        for (size_t i = 0, ilen = data.size(); i < ilen; i++) {
            if (!data.z_values.empty())
                dot_radius = data.z_values[i];

            coord = rect.map(data.x_values[i], data.y_values[i]);
            dots.add_child(SVG::Circle(coord.first, coord.second, (float)dot_radius));
        }

        // Temporary
        this->root.add_child(dots);
        return dots;
    }

    template<class T>
    inline SVG::Path Graph<T>::make_line(T& data, const std::string color) {
        SVG::Path line;
        std::pair<float, float> coord;

        for (size_t i = 0, ilen = data.size(); i < ilen; i++) {
            coord = rect.map(data.x_values[i], data.y_values[i]);
            line.line_to(coord);
        }

        // Temporary
        this->root.add_child(line);
        return line;
    }

    template<class T>
    class MultiGraph : public Graph<T> {
    public:
        MultiGraph(GraphOptions options = DEFAULT_GRAPH_LEGEND)
            : Graph<T>(options) {};
        void plot(DatasetCollection<T>& data);

    protected:
        std::vector<SVG::Group*> dot_groups;
        void make_legend();
    };

    template<class T>
    void MultiGraph<T>::plot(DatasetCollection<T>& data) {
        SVG::Group* grp_ptr;
        auto color = QUALITATIVE_COLORS.begin();
        rect = CartesianCoordinates(this->options, data);
        make_x_axis(data);
        make_y_axis(data);

        for (auto it = data.datasets.begin(); it != data.datasets.end(); ++it) {
            if (color == QUALITATIVE_COLORS.end())
                color = QUALITATIVE_COLORS.begin();

            grp_ptr = (SVG::Group*)this->root.add_child(make_point(*it, *color));
            this->dot_groups.push_back(grp_ptr);
            ++color;
        }

        make_legend();
    }

    template<class T>
    void MultiGraph<T>::make_legend() {
        // TODO: Make this less atrocious
        Legend legend;

        std::vector<std::string> labels;
        std::vector<std::string> fills;

        for (size_t i = 1; i <= dot_groups.size(); i++) {
            labels.push_back("Group " + std::to_string(i));
            fills.push_back(dot_groups[i - 1]->attr["fill"]);
        }

        legend.labels = labels;
        legend.fills = fills;
        legend.generate();

        legend.root.set_attr("x", rect.x2 + 10);
        legend.root.set_attr("y", (rect.y2 - legend.get_height()) / 2);
        this->root.add_child(legend.root);
    }

    class RadarChart : public PlotBase {
    public:
        RadarChart(size_t axes);
        void plot(vector<float> percentages);
    private:
        PolarCoordinates polar = { 250, 250, 250 };
        void make_grid(size_t lines=10);

        size_t n_axes;
        std::vector<SVG::Line*> axes;
    };

    /**
     * Class for multi-graph SVGs
    class Matrix: public Plot {
    public:
        Matrix(int _cols) : cols(_cols) {};
        void add_graph(Graph graph);
        void plot();

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