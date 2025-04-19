#pragma once
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <Eigen/Dense>
/**
 * @brief simple function to return version of the release. (keep for good memories)
 */
int version();

class Classifier
{
public:
    using features_t = std::vector<float>;
    using probas_t = std::vector<float>;

    virtual ~Classifier() {}

    virtual size_t num_classes() const = 0;

    virtual size_t predict(const features_t &) const = 0;

    virtual probas_t predict_proba(const features_t &) const = 0;
};

class MlpClassifier : public Classifier
{
public:
    MlpClassifier(const Eigen::MatrixXf &, const Eigen::MatrixXf &);

    size_t num_classes() const override;

    size_t predict(const features_t &) const override;

    probas_t predict_proba(const features_t &) const override;

private:
    Eigen::MatrixXf w1_, w2_;
};

Eigen::MatrixXf read_mat_from_stream(size_t rows, size_t cols, std::istream &stream)
{
    Eigen::MatrixXf res(rows, cols);
    for (size_t i = 0; i < rows; ++i)
    {
        for (size_t j = 0; j < cols; ++j)
        {
            float val;
            stream >> val;
            res(i, j) = val;
        }
    }
    return res;
}

Eigen::MatrixXf read_mat_from_file(size_t rows, size_t cols, const std::string &filepath)
{
    std::ifstream stream{filepath};
    return read_mat_from_stream(rows, cols, stream);
}

bool read_features(std::istream &stream, Classifier::features_t &features, char delimiter = ' ')
{
    std::string line;
    std::getline(stream, line);

    features.clear();
    std::istringstream linestream{line};
    double value;
    std::string token;
    while (std::getline(linestream, token, delimiter))
    {
        value = stod(token);
        features.push_back(value);
    }
    return stream.good();
}

std::vector<float> read_vector(std::istream &stream)
{
    std::vector<float> result;

    std::copy(std::istream_iterator<float>(stream),
              std::istream_iterator<float>(),
              std::back_inserter(result));
    return result;
}
