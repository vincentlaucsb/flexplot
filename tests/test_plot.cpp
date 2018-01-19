# define CATCH_CONFIG_MAIN
# include "catch.hpp"
# include "flexplot.h"

using namespace Graphs;

TEST_CASE("Scatterplot Test", "[test_scatter]") {
    NumericData points = {
        std::vector<long double>({ 1, 2, 3, 4, 5 }),
        std::vector<long double>({ 1, 2, 3, 4, 5 }),
    };

    Scatterplot plot;
    plot.plot(points);
    plot.to_svg("test_scatter.svg");
}

TEST_CASE("Bubble Plot Test", "[test_bubble]") {
    NumericData points = {
        std::vector<long double>({ 1, 2, 3, 4, 5 }),
        std::vector<long double>({ 1, 2, 3, 4, 5 }),
        std::vector<long double>({ 10, 20, 30, 40, 50 })
    };

    Scatterplot plot;
    plot.plot(points);
    plot.to_svg("test_bubble.svg");
}

TEST_CASE("Multi-BubblePlot Test", "[test_multibubble]") {
    NumericDataSet point_set;
    NumericData points1 = {
        std::vector<long double>({ 1, 2, 3, 4, 5 }),
        std::vector<long double>({ 1, 2, 3, 4, 5 }),
        std::vector<long double>({ 10, 20, 30, 40, 50 })
    };

    NumericData points2 = {
        std::vector<long double>({ 10, 20, 15, 2, 0 }),
        std::vector<long double>({ 1, 3, 4, 5, 6 }),
        std::vector<long double>({ 10, 20, 30, 40, 50 })
    };

    point_set = points1 + points2;

    MultiScatterplot plot;
    plot.plot(point_set);
    plot.to_svg("test_multibubble.svg");
}

TEST_CASE("Multi-Scatterplot Test", "[test_multiscatter]") {
    NumericDataSet point_set;
    NumericData points1 = {
        std::vector<long double>({ 1, 2, 3, 4, 5 }),
        std::vector<long double>({ 1, 2, 3, 4, 5 })
    };

    NumericData points2 = {
        std::vector<long double>({ 10, 20, 15, 2, 0 }),
        std::vector<long double>({ 1, 3, 4, 5, 6 })
    };

    point_set = points1 + points2;

    MultiScatterplot plot;
    plot.plot(point_set);
    plot.to_svg("test_multiscatter.svg");
}

TEST_CASE("Bar Chart Test", "[test_bar]") { 
    CategoricalData data = {
        std::vector<std::string>{ "A", "B", "C", "D", "E" },
        std::vector<long double>({ 1, 2.5, 5, 10, 100 }),
    };

    BarChart plot;
    plot.plot(data);
    plot.to_svg("test_bar.svg");
}

TEST_CASE("Radar Chart Test", "[test_radar]") {
    std::vector<float> pct = { (float)0.1, (float)0.2, (float)0.3, (float)0.4, (float)0.5 };

    RadarChart plot(5);
    plot.plot_points(pct);
    plot.to_svg("test_radar.svg");
}