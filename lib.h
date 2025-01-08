#pragma once
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <array>
#include <tuple>
#include <algorithm>
#include <type_traits>
#include <unordered_map>

/**
 * @brief simple function to return version of the release
 */
int version();

/**
 * @brief class to store sparse matrix
 * no constructor will be publicly available
 * only possible access point would be proxy-class Matrix
 */
template <typename T>
class IMatrix
{
private:
    std::unordered_map<std::pair<uint64_t,uint64_t>, T> index;
public:

};

template <typename T, int D>
class Matrix
{
private:
    IMatrix<T> matrix;
public:
};