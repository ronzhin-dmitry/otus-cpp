/**
@file lib.cpp
*/
#include "lib.h"
#include "version.h"
#include <unordered_set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <boost/crc.hpp>
#include <boost/uuid/detail/md5.hpp>
#include <cstdint>
using namespace boost::filesystem;
using namespace boost;
using namespace std;

/**
 * @brief function to produce release version in github workflows
 * @return current release version (incremented on git push)
 */
int version()
{
	return PROJECT_VERSION_PATCH;
}

/**
 * @brief processing a query with regex and predefined scan-level to collect filenames for check
 * @return list of filenames to process
 */
vector<string>& Filter::process_query(string& scan_directories, string& skip_directories, string& mask, bool recursive)
{
	stringstream ss_scan(scan_directories);
	stringstream ss_skip(skip_directories);
	string s;
	queue<string> directories_to_scan;
	unordered_set<string> directories_to_skip;
	unordered_set<string> scanned;
	regex file_mask(mask, boost::regex::icase); //case insensitive regex
	while(getline(ss_scan,s,' '))
		if(s != "")
			directories_to_scan.push(s); //collecting directories separated via space
	while(getline(ss_skip, s, ' '))
		if(s != "")
			directories_to_skip.insert(s);

	try
	{
		while(!directories_to_scan.empty())
		{
			auto dir = directories_to_scan.front();
			directories_to_scan.pop();
			if(scanned.find(dir) != scanned.end() || directories_to_skip.find(dir) != directories_to_skip.end())
				continue;
			scanned.insert(dir);
			path p(dir);
			if(exists(p) && is_directory(p))
			{
				for (directory_entry& x : directory_iterator(p))
				{
					auto x_path_string = x.path().string();
					if(is_directory(x) && recursive && (scanned.find(x_path_string) == scanned.end()))
						directories_to_scan.push(x_path_string);
					if(is_regular_file(x) && file_size(x) >= min_file_size)
					{
						//applying regex to the filename
						string name_stemmed = x.path().stem().string();
						if(regex_match(name_stemmed, file_mask)) {   
							filtered_files.push_back(x_path_string);
						}
					}
				}
			}
		}
	}
	catch (const filesystem_error& ex)
	{
	  cout << ex.what() << '\n';
	}
	return filtered_files;
}

/**
 * @brief using some pre-defined hashing strategy computes a uint32_t hash output from some data block
 * @return uint32_t hash for data block
 */
uint32_t Hasher::hash(void* data, size_t len)
{
	uint32_t res = 0;
	if(ht == HashType::CRC32)
	{
		boost::crc_32_type result;
		result.process_bytes(data, len);
		res = result.checksum();
	}
	else if(ht == HashType::MD5)
	{
		//Some strange version based on hints from web. Generally takes lower bytes of 128 bit hash from md5
		boost::uuids::detail::md5 md5_calculator;
		md5_calculator.process_bytes(data, len);
		boost::uuids::detail::md5::digest_type digest;
		md5_calculator.get_digest(digest);
		res = static_cast<uint32_t>(digest[0] & 0xFFFFFFFF);
	}
	return res;
}

Hasher::Hasher(std::string hash_name)
{
	if(hash_name == "crc32")
		ht = HashType::CRC32;
	else if(hash_name == "md5")
		ht = HashType::MD5;
	else
		throw "Not implemented";
}

/**
 * @brief main functions to search duplicates. Carefully reads some input and stores hash indices for comparison.
 * @return duplicates grouped in a vector of vectors. Each constituent vector is a group of equal files
 */
std::vector<std::vector<std::string>>& Scanner::find_duplicates(std::vector<std::string>& file_list)
{
	//Step 1 - we suppose equal file == equal sizes, filter out files of different size 
	//One could argue - file2 may be file1 with a lot of binary zeroes in the end. Such corner case this program will not catch, sorry
	//Maybe corner case will be fixed somewhen later in world's timeline
	for(auto& filename: file_list)
	{
		path p(filename);
		size_t fs = file_size(p);
		if(groups_by_size.find(fs) == groups_by_size.end())
			groups_by_size[fs] = vector<string>({filename}); //No files of such size were yet found
		else
			groups_by_size[fs].push_back(filename);
	}
	//Step 2 - for each group with more than 1 element we open filestreams
	//We will mantain a queue based on partial equality via hashes
	unordered_map<string, std::ifstream> file_streams;
	queue<vector<string>> groups_by_hash;
	for(auto & group: groups_by_size)
	{
		auto &cand_vec = group.second;
		if(cand_vec.size() == 1)
			continue; //skipping groups with single element
		for(auto& filename: cand_vec)
		{
			try
			{
				path p(filename);
				if(exists(p) && is_regular_file(p))
				{
					if(file_streams.find(filename) == file_streams.end())
					{
						file_streams[filename] = std::ifstream(filename, ios::binary);
					}
				}
			}
			catch (const filesystem_error& ex)
			{
				cout << ex.what() << '\n';
			}
		}
		groups_by_hash.push(cand_vec);
	}

	//Step 3 - we process queue with hash-based groups
	//For each group we read next chunk of bytes, perform hashing and possibly separate group
	vector<char> zero_block_buf(block_size, 0);
	vector<char> read_buf;
	while(!groups_by_hash.empty())
	{
		auto cur_group = groups_by_hash.front();
		groups_by_hash.pop();
		if(cur_group.size() == 1)
		{
			file_streams[cur_group[0]].close();
			continue;
		}
		unordered_map<uint32_t, vector<string>> subgroups;
		bool group_ended = true;
		for(auto & filename: cur_group)
		{
			if(!file_streams[filename].eof()) //if at least one file is ended - we know all group is done
			{
				read_buf = zero_block_buf; //will help zero-padding
				file_streams[filename].read(read_buf.data(), block_size); 
				uint32_t cur_hash = hasher.hash((void*)(read_buf.data()), block_size);
				if(subgroups.find(cur_hash) == subgroups.end())
					subgroups[cur_hash] = vector<string>({filename});
				else
					subgroups[cur_hash].push_back(filename);
				group_ended = false;
			}
		}
		if(group_ended)
		{
			scan_result.push_back(cur_group);
			for(auto & filename: cur_group)
				file_streams[filename].close();
		}
		for(auto &subgroup: subgroups)
		{
			if(subgroup.second.size() == 1)
			{
				file_streams[subgroup.second[0]].close();
				continue;
			}
			groups_by_hash.push(subgroup.second);
		}
	}
	return scan_result;
}