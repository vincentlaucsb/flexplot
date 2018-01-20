# define CATCH_CONFIG_MAIN
# include "catch.hpp"
# include "flexplot.h"

using namespace Graphs;

TEST_CASE("Scatterplot Test", "[test_scatter]") {
    NumericData points = {
        std::vector<long double>({ 1, 2, 3, 4, 5 }),
        std::vector<long double>({ 1, 2, 3, 4, 5 }),
    };

    Graph<NumericData> plot;
    plot.plot(points);
    plot.make_point(points);
    plot.make_line(points);
    plot.to_svg("test_scatter.svg");
}

TEST_CASE("Bubble Plot Test", "[test_bubble]") {
    NumericData points = {
        std::vector<long double>({ 1, 2, 3, 4, 5 }),
        std::vector<long double>({ 1, 2, 3, 4, 5 }),
        std::vector<long double>({ 10, 20, 30, 40, 50 })
    };

    Graph<NumericData> plot;
    plot.plot(points);
    plot.make_point(points);
    plot.to_svg("test_bubble.svg");
}

TEST_CASE("Multi-Scatterplot Test", "[test_multiscatter]") {
    GraphOptions options = DEFAULT_GRAPH_LEGEND;
    options.title = "My Graph";
    options.x_label = "x-axis Label";
    options.y_label = "y-axis Label";

    DatasetCollection<NumericData> point_set;
    NumericData points1 = {
        std::vector<long double>({ 1, 2, 3, 4, 5 }),
        std::vector<long double>({ 1, 2, 3, 4, 5 })
    };

    NumericData points2 = {
        std::vector<long double>({ -5, 1, 2, 4, 5 }),
        std::vector<long double>({ 1, 2, 3, 4, 5 })
    };

    NumericData points3 = {
        std::vector<long double>({ 1, 5, 7, 10, 13 }),
        std::vector<long double>({ 2, 3, 5, 7, 9 })
    };

    NumericData points4 = {
        std::vector<long double>({ 1.5, 2.5, 3.5, 4.5, 5.5 }),
        std::vector<long double>({ 1.5, 2.5, 3.5, 4.5, 5.5 })
    };

    point_set = points1 + points2 + points3 + points4;

    MultiGraph<NumericData> plot(options);
    plot.plot(point_set);
    plot.make_point(points1);
    plot.to_svg("test_multiscatter.svg");
}

TEST_CASE("Bar Chart Test", "[test_bar]") { 
    CategoricalData data = {
        std::vector<std::string>{ "A", "B", "C", "D", "E" },
        std::vector<long double>({ 1, 2.5, 5, 10, 100 }),
    };

    Graph<CategoricalData> plot;
    plot.plot(data);
    plot.make_bar(data);
    plot.to_svg("test_bar.svg");
}

TEST_CASE("Radar Chart Test", "[test_radar]") {
    std::vector<float> pct = { (float)0.1, (float)0.2, (float)0.3, (float)0.4, (float)0.5 };

    RadarChart plot(5);
    plot.plot(pct);
    plot.to_svg("test_radar.svg");
}