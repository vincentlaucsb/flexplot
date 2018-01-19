# define CATCH_CONFIG_MAIN
# include "catch.hpp"
# include "flexplot.h"

using namespace Graphs;

TEST_CASE("Scatterplot Test", "[test_scatter]") {
    NumericData points = {
        std::vector<long double>({ 1, 2, 3, 4, 5 }),
        std::vector<long double>({ 1, 2, 3, 4, 5 }),
    };

    Scatterplot plot(points);
    plot.generate(points);
    plot.to_svg("test_scatter.svg");
}

TEST_CASE("Bar Chart Test", "[test_bar]") { 
    CategoricalData data = {
        std::vector<std::string>{ "A", "B", "C", "D", "E" },
        std::vector<long double>({ 1, 2.5, 5, 10, 100 }),
    };

    BarChart plot(data);
    plot.generate(data);
    plot.to_svg("test_bar.svg");
}

TEST_CASE("Radar Chart Test", "[test_radar]") {
    std::vector<float> pct = { (float)0.1, (float)0.2, (float)0.3, (float)0.4, (float)0.5 };

    RadarChart plot(5);
    plot.plot_points(pct);
    plot.to_svg("test_radar.svg");
}