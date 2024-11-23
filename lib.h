#pragma once
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <tuple>
#include <algorithm>


int version();

std::vector<std::string> split(const std::string &str, char d);

void ip_fill(std::vector<std::array<int, 4>>& ip_pool);

void ip_sort(std::vector<std::array<int, 4>>& ip_pool);

void ip_print_all(const std::vector<std::array<int, 4>>& ip_pool);

std::vector<std::array<int, 4>> ip_filter_all(std::vector<std::array<int, 4>>& ip_pool, int v1 = -1, int v2 = -1, int v3 = -1, int v4 = -1);

std::vector<std::array<int, 4>> ip_filter_any(std::vector<std::array<int, 4>>& ip_pool, int v = -1);