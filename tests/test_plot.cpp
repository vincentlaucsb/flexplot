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

    MultiGraph<NumericData> plot;
    plot.set_title("My Graph");
    plot.set_x_label("x-axis Label");
    plot.set_y_label("y-axis Label");

    plot.plot(point_set);
    plot.make_point(point_set);
    plot.make_legend(point_set);
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

TEST_CASE("Multi-Bar Chart Test", "[test_multi_bar]") {
    CategoricalData tb12 = {
        std::vector<std::string>{ "2013", "2014", "2015", "2016", "2017" },
        std::vector<long double>({ 4343, 4109, 4770, 3554, 4577 }),
    };
    tb12.name = "Tom Brady";

    CategoricalData ar12 = {
        std::vector<std::string>{ "A", "B", "C", "D", "E" },
        std::vector<long double>({ 2536, 4381, 3821, 4428, 1675 }),
    };
    ar12.name = "A-Aron Rodgers";

    CategoricalData keenum = {
        { "2013", "2014", "2015", "2016", "2017" },
        {  1760, 435, 828, 2201, 3547 }
    };
    keenum.name = "Case Keenum";

    CategoricalData foles = {
        { "2013", "2014", "2015", "2016", "2017" },
        { 2891, 2163, 2052, 410, 527 }
    };
    foles.name = "Nick Foles";

    CategoricalData breesus = {
        { "2013", "2014", "2015", "2016", "2017" },
        { 5162, 4952, 4870, 5208, 4334 }
    };
    breesus.name = "Drew Brees";

    auto dataset = tb12 + ar12 + keenum + foles + breesus;
    REQUIRE(dataset.y_max() == 5208);

    MultiGraph<CategoricalData> plot;
    plot.set_title("Passing Yards");
    plot.set_y_label("Yards");
    plot.plot(dataset);
    plot.make_bar(dataset);
    plot.make_legend(dataset);
    plot.to_svg("test_multibar.svg");
}

TEST_CASE("Radar Chart Test", "[test_radar]") {
    std::vector<float> pct = { (float)0.1, (float)0.2, (float)0.3, (float)0.4, (float)0.5 };

    RadarChart plot(5);
    plot.plot(pct);
    plot.to_svg("test_radar.svg");
}