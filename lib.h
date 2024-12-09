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


int version();

std::vector<std::string> split(const std::string &str, char d);

void ip_fill(std::vector<std::array<int, 4>>& ip_pool);

void ip_sort(std::vector<std::array<int, 4>>& ip_pool);

void ip_print_all(const std::vector<std::array<int, 4>>& ip_pool);

std::vector<std::array<int, 4>> ip_filter_all(std::vector<std::array<int, 4>>& ip_pool, int v1 = -1, int v2 = -1, int v3 = -1, int v4 = -1);

std::vector<std::array<int, 4>> ip_filter_any(std::vector<std::array<int, 4>>& ip_pool, int v = -1);

//Here goes SFINAE
//Printing all integer types
template <typename T>
typename std::enable_if<std::is_integral<T>::value, void>::type
print_ip(T value)
{
    unsigned char* p = (unsigned char*)&value;
    std::cout << static_cast<int>(p[sizeof(T) - 1]);
    for(size_t i = 2 ; i < sizeof(T); i++)
        std::cout << "." << static_cast<int>(p[sizeof(T) - i]);
    std::cout << std::endl;
}

//Strings
template <typename T>
typename std::enable_if<std::is_same<T, std::string>::value, void>::type
print_ip(T value)
{
    std::cout << value << std::endl;
}


// A type trait to determine if a type is an STL container
template <typename T> struct is_stl_container : std::false_type{
};

// Specializations for specific STL container types
template <typename... Args> struct is_stl_container<std::vector<Args...>> : std::true_type{
};

template <typename... Args> struct is_stl_container<std::list<Args...>> : std::true_type{
};

//SFINAE for stl containers that we can iterate through
template <typename T>
typename std::enable_if<is_stl_container<T>::value, void>::type
print_ip(T value)
{
    auto it = value.begin();
    std::cout << *it;
    it++;
    for(; it != value.end(); it++ )
    {
        std::cout << "." << *it;
    }
    std::cout << std::endl;
}
