# include "flexplot.h"

// Statistical preprocessing on data

namespace Graphs {
    std::vector<std::string> NumericData::x_labels(size_t max_labels) {
        /** Generate x-axis labels for numeric data */
        std::vector<std::string> labels;
        max_labels = std::min(size(), max_labels);

        // Set bin labels to left-hand boundary values
        for (size_t i = 0; i <= max_labels; i++) {
            labels.push_back(std::to_string(
                x_min() + i*(x_max() - x_min()) / max_labels));
        }

        return labels;
    }
}