/**
@file lib.cpp
*/
#include "lib.h"
#include "version.h"
#include <unordered_set>
#include <iostream>
#include <sstream>
#include <algorithm>
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
uint32_t Hasher::hash(void* , size_t )
{
	return 0;
}
