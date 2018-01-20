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
    void PlotBase::to_svg(const std::string filename) {
        /** plot an SVG */
        std::ofstream svg_file(filename, std::ios_base::binary);
        svg_file << this->root.to_string();
        svg_file.close();
    }

    float Legend::get_height() {
        return this->fills.size() * 30;
    }

    void Legend::generate() {
        // TODO: Make this nicer (or not)
        this->root.set_attr("font-size", "14.5px")
            .set_attr("font-family", "sans-serif");

        SVG::SVG label_container;
        SVG::Rect guide;
        SVG::Text label;
        float y = 0;

        for (size_t i = 0; i < this->fills.size(); i++) {
            label_container = SVG::SVG();
            label_container.set_attr("y", y);

            guide = SVG::Rect(0, 5, 20, 20);
            guide.set_attr("fill", fills[i]);
            label = SVG::Text(30, 15, this->labels[i]);
            label.set_attr("dominant-baseline", "central");

            label_container.add_child(guide, label);
            this->root.add_child(label_container);
            y += 30;
        }
    }

    /**
    void Matrix::add_graph(Graph graph) {
        this->graphs.push_back(graph);
    }

    void Matrix::plot() {
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
            it->plot();
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
            line = SVG::Line(polar.center().first, coord.first,
                polar.center().second, coord.second);
            line.set_attr("stroke-width", 2).set_attr("stroke", "black");
            this->axes.push_back((SVG::Line*)root.add_child(line));
        }

        this->make_grid();
    }

    void RadarChart::make_grid(size_t lines) {
        /** Add a circular "grid" */
        SVG::Group grid;
        SVG::Circle line;
        grid.set_attr("fill", "none")
            .set_attr("stroke", "#cccccc")
            .set_attr("stroke-width", 1);

        for (float i = 0; i <= lines; i++)
            grid.add_child(SVG::Circle(polar.center(), polar.radius * i / lines));

        root.add_child(grid);
    }

    void RadarChart::plot(vector<float> percentages) {
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