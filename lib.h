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

class Hasher
{
private:
    enum class HashType{CRC32, MD5};
    HashType ht;
public:
    Hasher(std::string hash_name);
    uint32_t hash(void* data, size_t len);
};

class Scanner
{
    std::unordered_map<size_t, std::vector<std::string>> groups_by_size;
    Hasher& hasher;
    std::vector<std::vector<std::string>> scan_result;
    size_t block_size;
public:
    Scanner(Hasher& hsh, size_t bs):hasher(hsh), block_size(bs) {};
    std::vector<std::vector<std::string>>& find_duplicates(std::vector<std::string>& file_list);
};