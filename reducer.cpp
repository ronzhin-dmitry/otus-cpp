#include <iostream>
#include <string>
#include <sstream>


int main(int argc, char ** argv)
{
    size_t count = 0;
    double sum = 0;
    double sum_sq = 0;
    std::string line;
    while (std::getline(std::cin, line))
    {
        std::istringstream iss(line);
        std::string token;
        std::vector<std::string> parsed;
        while (std::getline(iss, token, ',')) {
            parsed.push_back(token);
        }
        if(parsed.size() < 2)
            continue; //bad data
        count += 1;
        sum += std::stoi(parsed[0]);
        sum_sq += std::stoi(parsed[1]);
    }
    std::cout << "Mean: " << sum / count << std::endl;
    std::cout << "Variance: " << sum_sq / count - (sum / count) * (sum / count) << std::endl;
    double variance_selective = (sum_sq - (sum * sum) / count) / (count - 1);
    std::cout << "Selective variance: " << variance_selective << std::endl;
    return 0;
}