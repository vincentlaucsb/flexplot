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
    ar12.name = "Aaron Rodgers";

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
    CategoricalData harden = {
        { "Points", "Rebounds", "Assists", "PIE" },
        { 31.5, 4.8, 9.0, 18.8 }
    };
    harden.name = "James Harden";

    CategoricalData greek_freak = {
        { "Points", "Rebounds", "Assists", "PIE" },
        { 28.2, 10.1, 4.6, 19.6 }
    };
    greek_freak.name = "Giannis Antetokounmpo";

    CategoricalData lebron = {
        { "Points", "Rebounds", "Assists", "PIE" },
        { 26.8, 7.8, 8.7, 18.8 }
    };
    lebron.name = "Lebron James";

    CategoricalData davis = {
        { "Points", "Rebounds", "Assists", "PIE" },
        { 26.5, 10.6, 2.3, 18.5 }
    };
    davis.name = "Anthony Davis";

    CategoricalData kd = {
        { "Points", "Rebounds", "Assists", "PIE" },
        { 26.2, 6.9, 5.4, 16.6 }
    };
    kd.name = "Kevin Durant";

    auto dataset = harden + greek_freak + lebron + davis + kd;
    /*
    std::vector<std::string> fill_colors = { "#CE1141", "#00471B", "#6F263D",
        "#002B5C", "#006BB6" };
    std::vector<std::string> stroke_colors = { "#C4CED4", "#EEE1C6", "#FFB81C",
        "#B4975A", "#FDB927" };
    */

    // dataset.set_fill(fill_colors);
    //dataset.set_stroke(stroke_colors);

    RadarChart plot;
    plot.grid_lines = 5;
    plot.set_title("NBA Top Scorers");
    plot.plot(dataset);
    plot.make_legend(dataset);
    plot.to_svg("test_radar.svg");
}

TEST_CASE("Radar Chart Test - 3 Categories", "[test_radar2]") {
    CategoricalData harden = {
        { "Points", "Rebounds", "Assists" },
        { 31.5, 4.8, 9.0 }
    };
    harden.name = "James Harden";

    CategoricalData greek_freak = {
        { "Points", "Rebounds", "Assists" },
        { 28.2, 10.1, 4.6 }
    };
    greek_freak.name = "Giannis Antetokounmpo";

    CategoricalData lebron = {
        { "Points", "Rebounds", "Assists" },
        { 26.8, 7.8, 8.7 }
    };
    lebron.name = "Lebron James";

    CategoricalData davis = {
        { "Points", "Rebounds", "Assists" },
        { 26.5, 10.6, 2.3 }
    };
    davis.name = "Anthony Davis";

    CategoricalData kd = {
        { "Points", "Rebounds", "Assists" },
        { 26.2, 6.9, 5.4 }
    };
    kd.name = "Kevin Durant";

    auto dataset = harden + greek_freak + lebron + davis + kd;
    /*
    std::vector<std::string> fill_colors = { "#CE1141", "#00471B", "#6F263D",
    "#002B5C", "#006BB6" };
    std::vector<std::string> stroke_colors = { "#C4CED4", "#EEE1C6", "#FFB81C",
    "#B4975A", "#FDB927" };
    */

    // dataset.set_fill(fill_colors);
    //dataset.set_stroke(stroke_colors);

    RadarChart plot;
    plot.grid_lines = 5;
    plot.set_title("NBA Top Scorers");
    plot.plot(dataset);
    plot.make_legend(dataset);
    plot.to_svg("test_radar.svg");
}