#pragma once
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <list>
#include <string>
#include <memory>
#include <ctime>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/regex.hpp>

/**
 * @brief simple function to return version of the release.
 */
int version();

class Filter
{
private:
    std::vector<std::string> filtered_files;
    size_t min_file_size;
public:
    Filter(int mfs): min_file_size(mfs) {};
    std::vector<std::string>& process_query(std::string& scan_directories, std::string& skip_directories, std::string& mask, bool recursive);
};

class Scanner
{
    std::unordered_map<std::string, size_t> read_bytes; //keeps amount of bytes that were already read
    std::vector<std::unordered_map<uint32_t, std::set<std::string>>> hash_to_files; //chunk hash to set of files
public:
    Scanner() {};
};

class Hasher
{
public:
    Hasher() {};
    uint32_t hash(void* data, size_t len);
};