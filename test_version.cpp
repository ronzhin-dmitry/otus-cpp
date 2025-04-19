/**
@file test_version.cpp
*/
#define BOOST_TEST_MODULE test_version

#include "lib.h"
#include <sstream>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(test_version)

BOOST_AUTO_TEST_CASE(test_valid_version)
{
	BOOST_CHECK(version() > 0);
}

BOOST_AUTO_TEST_CASE(test_mlp)
{
	auto w1 = read_mat_from_file(784, 128, "w1.txt");
    auto w2 = read_mat_from_file(128, 10, "w2.txt");

    auto clf = MlpClassifier{w1.transpose(), w2.transpose()};

    auto features = MlpClassifier::features_t{};


    std::ifstream test_data{"test_data_mlp.txt"};
	BOOST_REQUIRE_NO_THROW(test_data.is_open());
    for (;;) {
        size_t y_true;
        test_data >> y_true;
		if (test_data.peek() == ' ') test_data.ignore();
        if (!read_features(test_data, features)) {
            break;
        }
        auto y_pred = clf.predict(features);
        BOOST_CHECK_EQUAL(y_true, y_pred);
    }
}
BOOST_AUTO_TEST_SUITE_END()