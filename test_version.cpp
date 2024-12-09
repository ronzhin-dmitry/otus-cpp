#define BOOST_TEST_MODULE test_version

#include "lib.h"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(test_version)

BOOST_AUTO_TEST_CASE(test_valid_version)
{
	BOOST_CHECK(version() > 0);
}

BOOST_AUTO_TEST_CASE(test_split)
{
	std::string str = "127.0.0.1";
	auto res = split(str, '.');
	std::vector<std::string> base = {"127","0","0","1"};
	BOOST_CHECK_EQUAL_COLLECTIONS(res.begin(), res.end(), base.begin(), base.end());
}

BOOST_AUTO_TEST_CASE(test_sort)
{
	std::vector<std::array<int,4>> input = 
	{
		{1,1,1,1},
		{2,2,2,2},
		{4,3,5,6},
		{3,4,5,6},
		{7,7,7,7}
	};

	std::vector<std::array<int,4>> base = 
	{
		{7,7,7,7},
		{4,3,5,6},
		{3,4,5,6},
		{2,2,2,2},
		{1,1,1,1}
	};
	ip_sort(input);
	BOOST_TEST(base == input);
}

BOOST_AUTO_TEST_CASE(test_filter_all)
{
	std::vector<std::array<int,4>> input = 
	{
		{1,1,1,1},
		{2,2,2,2},
		{4,3,5,6},
		{3,4,5,6},
		{7,7,7,7}
	};

	std::vector<std::array<int,4>> base = 
	{
		{4,3,5,6},
		{3,4,5,6},
	};
	auto res = ip_filter_all(input, -1, -1, 5, 6);
	BOOST_TEST(base == res);
}

BOOST_AUTO_TEST_CASE(test_filter_any)
{
	std::vector<std::array<int,4>> input = 
	{
		{1,1,1,1},
		{2,2,2,2},
		{4,3,5,6},
		{3,4,5,6},
		{7,7,7,7}
	};

	std::vector<std::array<int,4>> base = 
	{
		{4,3,5,6},
		{3,4,5,6},
	};
	auto res = ip_filter_any(input, 4);
	BOOST_TEST(base == res);
}

BOOST_AUTO_TEST_SUITE_END()