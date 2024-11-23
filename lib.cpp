#include "lib.h"
#include "version.h"

int version()
{
	return PROJECT_VERSION_PATCH;
}

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
	while (stop != std::string::npos)
	{
		r.push_back(str.substr(start, stop - start));

		start = stop + 1;
		stop = str.find_first_of(d, start);
	}

	r.push_back(str.substr(start));

	return r;
}

void ip_sort(std::vector<std::array<int, 4>> &ip_pool)
{
	auto rev_lex = [](std::array<int, 4> &a, std::array<int, 4> &b)
	{
		return a >= b;
	};
	// rev lex via sort
	std::sort(ip_pool.begin(), ip_pool.end(), rev_lex);
	return;
}

void ip_print_all(const std::vector<std::array<int, 4>> &ip_pool)
{
	for (auto ip = ip_pool.cbegin(); ip != ip_pool.cend(); ++ip)
	{
		for (auto ip_part = ip->cbegin(); ip_part != ip->cend(); ++ip_part)
		{
			if (ip_part != ip->begin())
			{
				std::cout << ".";
			}
			std::cout << *ip_part;
		}
		std::cout << std::endl;
	}
	return;
}

std::vector<std::array<int, 4>> ip_filter_all(std::vector<std::array<int, 4>> &ip_pool, int v1, int v2, int v3, int v4)
{
	if (v1 == -1 && v2 == -1 && v3 == -1 && v4 == -1)
		return ip_pool;
	std::vector<std::array<int, 4>> res;
	auto filter_func = [v1, v2, v3, v4, &res](const std::array<int, 4> &a)
	{
		if ((v1 == -1 || (v1 != -1 && a[0] == v1)) &&
			(v2 == -1 || (v2 != -1 && a[1] == v2)) &&
			(v3 == -1 || (v3 != -1 && a[2] == v3)) &&
			(v4 == -1 || (v4 != -1 && a[3] == v4)))
		{
			res.push_back(a);
		}
		return;
	};
	std::for_each(ip_pool.begin(), ip_pool.end(), filter_func);
	return res; // In RVO and Move We Trust
}

std::vector<std::array<int, 4>> ip_filter_any(std::vector<std::array<int, 4>> &ip_pool, int v)
{
	if (v == -1)
		return ip_pool;
	std::vector<std::array<int, 4>> res;
	auto filter_func = [v, &res](const std::array<int, 4> &a)
	{
		if ((a[0] == v) || (a[1] == v) || (a[2] == v) || (a[3] == v))
		{
			res.push_back(a);
		}
		return;
	};
	std::for_each(ip_pool.begin(), ip_pool.end(), filter_func);
	return res; // In RVO and Move We Trust
}

void ip_fill(std::vector<std::array<int, 4>> &ip_pool)
{
	for (std::string line; std::getline(std::cin, line);)
	{
		std::vector<std::string> v = split(line, '\t');
		std::vector<std::string> v1 = split(v.at(0), '.');
		try
		{
			ip_pool.push_back({std::stoi(v1[0]), std::stoi(v1[1]), std::stoi(v1[2]), std::stoi(v1[3])});
		}
		// Standard exceptions for stoi
		catch (const std::invalid_argument &e)
		{
			std::cout << e.what() << "\n";
			std::cout << "Invalid input line: " << line << std::endl;
		}
		catch (const std::out_of_range &e)
		{
			std::cout << e.what() << "\n";
			std::cout << "Invalid input line: " << line << std::endl;
		}
	}
	return;
}