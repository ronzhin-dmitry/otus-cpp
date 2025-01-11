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
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_matrix_correctness)
BOOST_AUTO_TEST_CASE(basic_operations)
{
    Matrix<int, -1> matrix;
    BOOST_CHECK(matrix.size() == 0); // free matrix
    auto a = matrix[0][0];
    BOOST_CHECK(a == -1);
    BOOST_CHECK(matrix.size() == 0);
    matrix[100][100] = 314;
    BOOST_CHECK(matrix[100][100] == 314);
    BOOST_CHECK(matrix.size() == 1);
    // single line expected
    // 100100314
    std::stringstream s1;
    for(auto c: matrix)
    {
        int x;
        int y;
        int v;
        std::tie(x, y, v) = c;
        s1 << x << y << v;
    }
    BOOST_CHECK(s1.str() == "100100314");
	matrix[100][100] = -1; //assigning default value == erase
	BOOST_CHECK(matrix.size() == 0);
}
BOOST_AUTO_TEST_SUITE_END()