#define PI 3.14159265
#include "flexplot.h"
// #include "str.h"

using std::deque;
using std::vector;
using std::string;

namespace SVG {
    float Line::get_slope() {
        return (y2() - y1()) / (x2() - x1());
    }

    float Line::get_length() {
        return std::sqrt(pow(get_width(), 2) + pow(get_height(), 2));
    }

    float Line::get_width() {
        return std::abs(x2() - x1());
    }

    float Line::get_height() {
        return std::abs(y2() - y1());
    }

    std::pair<float, float> Line::along(float percent) {
        /** Return the coordinates required to place an element along
         *   this line
         */

        float x_pos, y_pos;

        if (x1() != x2()) {
            float length = percent * this->get_length();
            float discrim = std::sqrt(4 * pow(length, 2) * (1 / (1 + pow(get_slope(), 2))));

            float x_a = (2 * x1() + discrim) / 2;
            float x_b = (2 * x1() - discrim) / 2;
            x_pos = x_a;

            if ((x_a > x1() && x_a > x2()) || (x_a < x1() && x_a < x2()))
                x_pos = x_b;

            y_pos = get_slope() * (x_pos - x1()) + y1();

            std::cout << x_a << " " << x_b << std::endl;

            std::cout << "Length: " << length << " Discrim: " << discrim <<
                " x: " << x_pos << " y: " << y_pos << " Slope: " << get_slope() << std::endl;
        }
        else { // Edge case:: Completely vertical lines
            x_pos = x1();

            if (y1() > y2()) // Downward pointing
                y_pos = y1() - percent * this->get_length();
            else
                y_pos = y1() + percent * this->get_length();
        }

        return std::make_pair(x_pos, y_pos);
    }

# define to_string_attrib for (auto it = attr.begin(); it != attr.end(); ++it) \
    ret += " " + it->first + "=" + "\"" + it->second += "\""

    std::string Element::to_string() {
        std::string ret = "<" + tag;

        // Set attributes
        to_string_attrib;
        
        if (!this->children.empty()) {
            ret += ">\n";

            // Recursively get strings for child elements
            for (auto it = children.begin(); it != children.end(); ++it)
                ret += "\t" + (*it)->to_string() + "\n";

            return ret += "</" + tag + ">";
        }

        return ret += " />";
    }

    std::string Text::to_string() {
        std::string ret = "<text";
        to_string_attrib;
        return ret += ">" + this->content + "</text>";
    }
}

namespace Graphs {
    void CartesianCoordinates::set_data(CategoricalData& data) {
        /** Specify the min and max of the data set */
        this->range_min = data.y_min();
        this->range_max = data.y_max();
    }

    void CartesianCoordinates::set_data(NumericData& data) {
        /** Specify the min and max of the data set */
        this->domain_min = data.x_min();
        this->domain_max = data.x_max();
        this->range_min = data.y_min();
        this->range_max = data.y_max();
    }

    void Plot::to_svg(const std::string filename) {
        /** Generate an SVG */
        std::ofstream svg_file(filename, std::ios_base::binary);
        svg_file << this->root.to_string();
        svg_file.close();
    }

    void Graph<NumericData>::make_x_axis(NumericData data) {
        /** Generate the x-axis (lines, ticks, labels)
         *
         *  @param[out] align Specifies whether labels should be
         *                    left or center aligned wrt the bars
         */

        std::vector<std::string> x_labels = data.x_labels();
        SVG::Group ticks, tick_text;
        std::pair<float, float> coord;

        ticks.set_attr("stroke-width", 1).set_attr("stroke", "#000000");
        tick_text.set_attr("style", "font-family: sans-serif; font-size: 12px;")
            .set_attr("text-anchor", "left");

        // Add tick marks
        for (float i = 0, n = data.size(); i <= n; i++) {
            coord = x_axis->along(i/n);
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

    void Graph<CategoricalData>::make_x_axis(CategoricalData data) {
        /** Generate the x-axis (lines, ticks, labels)
        *
        *  @param[out] align Specifies whether labels should be
        *                    left or center aligned wrt the bars
        */

        SVG::Group ticks, tick_text;
        std::pair<float, float> coord;

        ticks.set_attr("stroke-width", 1).set_attr("stroke", "#000000");
        tick_text.set_attr("style", "font-family: sans-serif; font-size: 12px;")
            .set_attr("text-anchor", "left");

        // Add tick marks
        float n = data.size() + 1;
        float x_tick_space = (rect.x2 - rect.x1) / data.size();
        float tick_offset = 0; // Used for alignment

        n--;
        tick_offset += x_tick_space / 2;

        for (float i = 0; i < n; i++) {
            coord = x_axis->along(i / n);
            ticks.add_child(SVG::Line(
                coord.first + tick_offset, coord.first + tick_offset,
                coord.second, coord.second + tick_size
            ));

            // Use translate() to set location rather than x, y
            // attributes so rotation works properly
            SVG::Text label(0, 0, data.x_values[i]);
            label.set_attr("transform", "translate(" +
                std::to_string(coord.first + tick_offset) + "," +
                std::to_string(rect.y2 + tick_size + 10) // Space label further south from ticks
                + ") rotate(75)");
            tick_text.add_child(label);
        }

        this->x_axis_group->add_child(ticks, tick_text);
    }

    template<typename T>
    void Graph<T>::make_y_axis(T data) {
        std::vector<std::string> y_labels = data.y_labels(6);
        SVG::Group ticks, tick_text;
        std::pair<float, float> coord;

        ticks.set_attr("stroke-width", 1).set_attr("stroke", "#000000");
        tick_text.set_attr("style", "font-family: sans-serif;"
            "font-size: 12px;").set_attr("text-anchor", "end");

        // Add 6 y-axis tick marks starting from the bottom, moving upwards
        // Ticks are represented as tiny lines
        for (size_t i = 0; i < 6; i++) {
            coord = y_axis->along((float)i/6);
            ticks.add_child(SVG::Line(
                coord.first - 5, coord.first, coord.second, coord.second));
            tick_text.add_child(SVG::Text(coord.first - 5, coord.second, y_labels[i]));
        }

        this->y_axis_group->add_child(ticks, tick_text);
    }

    BarChart::BarChart(CategoricalData data, GraphOptions options) :
        Graph(options) {
        /** Set up the graph */
        this->rect.set_data(data);

        if (options.title.empty())
            this->title->content = "Chart for " + options.x_label + " vs. " + options.y_label;
        else
            this->title->content = options.title;

        this->xlab->content = options.x_label;
        this->ylab->content = options.y_label;
    }

    void BarChart::generate(CategoricalData data) {
        this->make_x_axis(data);
        this->make_y_axis(data);
        this->root.add_child(make_bars(data));
    }

    SVG::Group BarChart::make_bars(CategoricalData& data) {
        /** Distribute bars evenly across graph canvas */
        SVG::Group bars;
        bars.set_attr("fill", "#004777");
        
        std::pair<float, float> coord;
        float x_tick_space = (rect.x2 - rect.x1) / data.size();
        const int max_height = rect.y2 - rect.y1;
        int temp_x1 = rect.x1;
        double bar_height;

        // Add a bar for every bin
        for (auto it = data.y_values.begin(); it != data.y_values.end(); ++it) {
            bar_height = (*it / (rect.range_max - rect.range_min)) * (rect.y2 - rect.y1);
            bars.add_child(SVG::Rect(temp_x1, rect.y2 - bar_height,
                x_tick_space - bar_spacing, bar_height));
            temp_x1 += x_tick_space;
        }

        return bars;
    }

    Scatterplot::Scatterplot(NumericData _data, GraphOptions options) :
        data(_data), Graph(options) {
        this->rect.set_data(data);
        if (options.title.empty())
            this->title->content = "Scatterplot for " +
            options.x_label + " vs. " + options.y_label;
        else
            this->title->content = options.title;

        this->xlab->content = options.x_label;
        this->ylab->content = options.y_label;
    }

    void Scatterplot::generate(NumericData data) {
        make_x_axis(data);
        make_y_axis(data);
        this->root.add_child(make_dots());
    }

    SVG::Group Scatterplot::make_dots(size_t dot_radius) {
        std::pair<float, float> coord;
        SVG::Group dots;
        dots.set_attr("fill", "#004777");

        // Add each dot
        for (size_t i = 0, ilen = data.size(); i < ilen; i++) {
            coord = rect.map(data.x_values[i], data.y_values[i]);
            dots.add_child(SVG::Circle(coord.first, coord.second, dot_radius));
        }

        return dots;
    }
    
    /**
    void Matrix::add_graph(Graph graph) {
        this->graphs.push_back(graph);
    }

    void Matrix::generate() {
        // Set final width & height of SVG
        root.set_attr("width", width_per_graph * cols);
        root.set_attr("height", height_per_graph *
            ceil((float)graphs.size())/(float)cols);

        size_t current_col = 0;
        size_t current_row = 0;

        // Adjust graphs
        for (auto it = graphs.begin(); it != graphs.end(); ++it) {
            it->root.set_attr("x", current_col * width_per_graph);
            it->root.set_attr("y", current_row * height_per_graph);
            it->width = width_per_graph;
            it->height = height_per_graph;
            it->generate();
            root.add_child(it->root);

            current_col++;
            if (current_col == cols) {
                current_row++;
                current_col = 0;
            }
        } 
    }
    **/

    RadarChart::RadarChart(size_t axes) : n_axes(axes) {
        SVG::Line line;
        std::pair<double, double> coord;
        root.set_attr("width", 500);
        root.set_attr("height", 500);

        // Draw up axes
        for (double i = 0; i < axes; i++) {
            coord = polar.map((i / axes) * 2 * PI);
            line = SVG::Line((int)polar.center().first, coord.first,
                (int)polar.center().second, coord.second);
            line.set_attr("stroke-width", 2).set_attr("stroke", "black");

            root.add_child(SVG::Circle(std::get<0>(coord), std::get<1>(coord), 2));
            this->axes.push_back((SVG::Line*)root.add_child(line));
        }

        // this->make_axis(axes);
    }

    void RadarChart::make_axis(size_t axes) {
        /** Add tick marks to all axes */
        SVG::Line tick;

        for (float i = 0; i < axes; i++) {
            for (float rad = 0.1; rad < 1; rad += 0.1) {
                auto left_coord = polar.map(((i / axes) * 2 * PI) - 0.5, rad);
                auto right_coord = polar.map(((i / axes) * 2 * PI) + 0.5, rad);

                tick = SVG::Line(
                    left_coord.first, right_coord.first,
                    left_coord.second, right_coord.second
                );

                tick.set_attr("stroke", "black");
                tick.set_attr("stroke-width", 2);
                root.add_child(tick);
            }
        }
    }

    void RadarChart::plot_points(vector<float> percentages) {
        /** Plot each of the percentages on an axis on the radar chart */
        if (percentages.size() != n_axes)
            throw std::runtime_error("Expected " + std::to_string(n_axes) + "data points"
                + " but got " + std::to_string(percentages.size()));

        // Add line connecting end of each axis
        SVG::Path data_line;
        std::pair<float, float> coord;

        for (size_t i = 0; i < percentages.size(); i++) {
            coord = axes[i]->along(percentages[i]); // Map percentage to SVG space
            data_line.line_to(coord.first, coord.second);
        }

        data_line.to_origin();
        data_line.set_attr("stroke-width", 2)
            .set_attr("fill", "none").set_attr("stroke", "blue");

        root.add_child(data_line);
    }

    std::pair<float, float> PolarCoordinates::center() {
        return std::make_pair(this->x, this->y);
    }

    std::pair<float, float> PolarCoordinates::map(float degrees, float percent) {
        float x = percent * radius * cos(degrees) + this->x;
        float y = percent * radius * sin(degrees) + this->y;
        return std::make_pair(x, y);
    }
}