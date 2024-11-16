#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include "lib.h"
#include <tuple>
#include <algorithm>

// ("",  '.') -> [""]
// ("11", '.') -> ["11"]
// ("..", '.') -> ["", "", ""]
// ("11.", '.') -> ["11", ""]
// (".11", '.') -> ["", "11"]
// ("11.22", '.') -> ["11", "22"]
std::vector<std::string> split(const std::string &str, char d)
{
    std::vector<std::string> r;

    std::string::size_type start = 0;
    std::string::size_type stop = str.find_first_of(d);
    while(stop != std::string::npos)
    {
        r.push_back(str.substr(start, stop - start));

        start = stop + 1;
        stop = str.find_first_of(d, start);
    }

    r.push_back(str.substr(start));

    return r;
}

int main()
{
    //std::cerr << "IP filter -- ";
    //std::cerr << "Version: " << version() << std::endl;
    //test commit
    try
    {
        std::vector<std::array<int, 4>> ip_pool;

        for(std::string line; std::getline(std::cin, line);)
        {
            std::vector<std::string> v = split(line, '\t');
            std::vector<std::string> v1 = split(v.at(0), '.');
            try{
            ip_pool.push_back({std::stoi(v1[0]), std::stoi(v1[1]), std::stoi(v1[2]), std::stoi(v1[3])});
            }
            // Standard exceptions for stoi
            catch (const std::invalid_argument & e) {
                std::cout << e.what() << "\n";
                std::cout << "Invalid input line: " << line << std::endl;
            }
            catch (const std::out_of_range & e) {
                std::cout << e.what() << "\n";
                std::cout << "Invalid input line: " << line << std::endl;
            }
        }
        /*
        auto rev_lex = [](std::vector<std::string>& a, std::vector<std::string>& b) { 
                 std::tuple<int, int, int, int> tuple1(std::stoi(a[0]), std::stoi(a[1]), std::stoi(a[2]),std::stoi(a[3]));
                 std::tuple<int, int, int, int> tuple2(std::stoi(b[0]), std::stoi(b[1]), std::stoi(b[2]),std::stoi(b[3]));
                 return tuple1 >= tuple2;
            };*/

        auto rev_lex = [](std::array<int, 4>& a, std::array<int, 4>& b) { 
                 return a >= b;
            };
        //rev lex via sort
        std::sort(ip_pool.begin(), ip_pool.end(), rev_lex);

        for(auto ip = ip_pool.cbegin(); ip != ip_pool.cend(); ++ip)
        {
            for(auto ip_part = ip->cbegin(); ip_part != ip->cend(); ++ip_part)
            {
                if (ip_part != ip->begin())
                {
                    std::cout << ".";

                }
                std::cout << *ip_part;
            }
            std::cout << std::endl;
        }

        // 222.173.235.246
        // 222.130.177.64
        // 222.82.198.61
        // ...
        // 1.70.44.170
        // 1.29.168.152
        // 1.1.234.8

        // filter by first byte and output
        auto filter_first = [](const std::array<int, 4>& a) {
            if(a[0] == 1)
            {
                std::cout << a[0] << "." << a[1] << "." << a[2] << "." << a[3] << std::endl;
            }
        };
        std::for_each(ip_pool.begin(), ip_pool.end(), filter_first);
        // ip = filter(1)

        // 1.231.69.33
        // 1.87.203.225
        // 1.70.44.170
        // 1.29.168.152
        // 1.1.234.8

        //  filter by first and second bytes and output
        auto filter_first_second = [](const std::array<int, 4>& a) {
            if(a[0] == 46 && a[1] == 70)
            {
                std::cout << a[0] << "." << a[1] << "." << a[2] << "." << a[3] << std::endl;
            }
        };
        std::for_each(ip_pool.begin(), ip_pool.end(), filter_first_second);
        // ip = filter(46, 70)

        // 46.70.225.39
        // 46.70.147.26
        // 46.70.113.73
        // 46.70.29.76

        // filter by any byte and output
        auto filter_any = [](const std::array<int, 4>& a) {
            if(a[0] == 46 || a[1] == 46 || a[2] == 46 || a[3] == 46)
            {
                std::cout << a[0] << "." << a[1] << "." << a[2] << "." << a[3] << std::endl;
            }
        };
        std::for_each(ip_pool.begin(), ip_pool.end(), filter_any);
        // ip = filter_any(46)

        // 186.204.34.46
        // 186.46.222.194
        // 185.46.87.231
        // 185.46.86.132
        // 185.46.86.131
        // 185.46.86.131
        // 185.46.86.22
        // 185.46.85.204
        // 185.46.85.78
        // 68.46.218.208
        // 46.251.197.23
        // 46.223.254.56
        // 46.223.254.56
        // 46.182.19.219
        // 46.161.63.66
        // 46.161.61.51
        // 46.161.60.92
        // 46.161.60.35
        // 46.161.58.202
        // 46.161.56.241
        // 46.161.56.203
        // 46.161.56.174
        // 46.161.56.106
        // 46.161.56.106
        // 46.101.163.119
        // 46.101.127.145
        // 46.70.225.39
        // 46.70.147.26
        // 46.70.113.73
        // 46.70.29.76
        // 46.55.46.98
        // 46.49.43.85
        // 39.46.86.85
        // 5.189.203.46
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
