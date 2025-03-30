/**
@file lib.cpp
*/
#include "lib.h"
#include "version.h"

/**
 * @brief function to produce release version in github workflows
 * @return current release version (incremented on git push)
 */
int version()
{
	return PROJECT_VERSION_PATCH;
}

namespace database
{
    using namespace std;
    string Database::intersection()
	{
		shared_lock lock(mtx);
		string res = "";
		for(auto it = table_A.begin(); it != table_A.end(); it++)
		{
			auto it2 = table_B.find(it->first);
			if(it2 != table_B.end())
				res += to_string(it->first) + "," + it->second + "," + it2->second + "\n"; 
		}
		return res + string("OK\n");
	}

    string Database::symmetric_difference()
	{
		shared_lock lock(mtx);
		string res = "";
		for(auto it = table_A.begin(); it != table_A.end(); it++)
		{
			auto it2 = table_B.find(it->first);
			if(it2 == table_B.end())
				res += to_string(it->first) + "," + it->second + ",\n"; 
		}
		for(auto it = table_B.begin(); it != table_B.end(); it++)
		{
			auto it2 = table_A.find(it->first);
			if(it2 == table_A.end())
				res += to_string(it->first) + ",," + it->second + "\n"; 
		}
		return res + string("OK\n");
	}
    
	string Database::truncate_A()
	{
		unique_lock lock(mtx);
		table_A.clear();
		return string("OK\n");
	}
    
	string Database::truncate_B()
	{
		unique_lock lock(mtx);
		table_B.clear();
		return string("OK\n");
	}

	string Database::insert_A(int id, string val)
	{
		unique_lock lock(mtx);
		if(table_A.find(id) != table_A.end())
			return string("ERR duplicate ") + to_string(id) + "\n"; 
		table_A[id] = val;
		return string("OK\n");
	}
    
	string Database::insert_B(int id, string val)
	{
		unique_lock lock(mtx);
		if(table_B.find(id) != table_B.end())
			return string("ERR duplicate ") + to_string(id) + "\n"; 
		table_B[id] = val;
		return string("OK\n");
	}

	string Database::execute(string message)
	{
		//TODO: separate command parser may be considered - currently overkill
		//Currently all parsing logic is here
		//No tolerance for incorrect spacing in commands
		if(message.substr(0,6) == "INSERT")
		{
			if(message.substr(7,1) == "A")
			{
				auto end_pos = message.find(" ",9);
				if(end_pos == string::npos)
					return "ERR unknown command\n";
				return insert_A(stoi(message.substr(9, end_pos)), message.substr(end_pos+1, message.size()));
			}
			else if(message.substr(7, 1) == "B")
			{
				auto end_pos = message.find(" ",9);
				if(end_pos == string::npos)
					return "ERR unknown command\n";
				return insert_B(stoi(message.substr(9, end_pos)), message.substr(end_pos+1, message.size()));
			}
		}
		else if(message.substr(0,8) == "TRUNCATE")
		{
			if(message.substr(9,1) == "A")
			{
				return truncate_A();
			}
			else if(message.substr(9, 1) == "B")
			{
				return truncate_B();
			}
		}
		else if(message == "INTERSECTION")
		{
			return intersection();
		}
		else if(message == "SYMMETRIC_DIFFERENCE")
		{
			return symmetric_difference();
		}
		return "ERR unknown command - " + message + "\n";
	}
}

namespace join_server
{
	
}