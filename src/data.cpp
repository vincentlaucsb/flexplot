# include "flexplot.h"

// Statistical preprocessing on data

namespace Graphs {
    std::string to_string(float number, size_t n) {
        /** Return a float as a string, cutting it off at 
         *  n decimal places
         */

        std::string temp = std::to_string(number);
        size_t dec_pos = temp.find_first_of('.');
        return temp.substr(0, dec_pos + 1 + n);
    }

    NumericData::NumericData(
        const std::vector<long double> x,
        const std::vector<long double> y,
        const std::vector<long double> z
    ): NumericData(x, y) {
        if (y.size() != z.size())
            throw std::runtime_error("y and z values have different lengths");
        this->z_values = z;
    }

    class CategoricalDataSet : public DatasetCollection<CategoricalData> {
        std::vector<std::string> x_labels(size_t) {
            return this->datasets.begin()->x_labels();
        }
    };

    std::vector<std::string> DatasetBase::x_labels(size_t max_labels) {
        /** Generate x-axis labels for numeric data
         *  Should have one more label than data size
         */

        std::vector<std::string> labels;
        max_labels = std::min(size() + 1, max_labels);

        // Set bin labels to left-hand boundary values
        for (size_t i = 0; i <= max_labels; i++) {
            labels.push_back(std::to_string(
                x_min() + i*(x_max() - x_min()) / (max_labels - 1)));
        }

        return labels;
    }

    std::vector<std::string> DatasetBase::y_labels(const size_t labels) {
        std::vector<std::string> ret_labels;

        // Set bin labels to left-hand boundary values
        for (size_t i = 0; i <= labels; i++) {
            ret_labels.push_back(Graphs::to_string(
                y_min() + i*(y_max() - y_min()) / labels));
        }

        return ret_labels;
    }

    long double DatasetCollection<NumericData>::x_min() {
        /** Return the largest x value in the collection of data */
        long double min = NAN;
        for (auto it = datasets.begin(); it != datasets.end(); ++it)
            if (isnan(min) || it->x_min() < min) min = it->x_min();

        return min;
    }

    long double DatasetCollection<NumericData>::x_max() {
        /** Return the largest x value in the collection of data */
        long double max = NAN;
        for (auto it = datasets.begin(); it != datasets.end(); ++it)
            if (isnan(max) || it->x_max() > max) max = it->x_max();

        return max;
    }

    DatasetCollection<NumericData> NumericData::operator+ (NumericData& other) {
        /** Append data to the set */
        DatasetCollection<NumericData> ret;
        ret.datasets.push_back(*this);
        ret.datasets.push_back(other);
        return ret;
    }

    DatasetCollection<CategoricalData> CategoricalData::operator+ (CategoricalData& other) {
        DatasetCollection<CategoricalData> ret;
        ret.datasets.push_back(*this);
        ret.datasets.push_back(other);
        return ret;
    }

    std::vector<std::string> DatasetCollection<NumericData>::x_labels(size_t max_labels) {
        return DatasetBase::x_labels(max_labels);
    }

    std::vector<std::string> DatasetCollection<CategoricalData>::x_labels(size_t max_labels) {
        // Assumes all CategoricalData objects have the same labels
        return this->datasets.begin()->x_values;
    }
}