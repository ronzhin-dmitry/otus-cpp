/**
@file main.cpp
*/
#include "lib.h"
#define N_DEFAULT 3

/**
 * @brief Entry point
 *
 * Execution of the program
 * starts here.
 *
 * @param argc Number of arguments
 * @param argv List of arguments
 *
 * @return Program exit status
 */
int main(int argc, char* argv[])
{
    using namespace boost::program_options;
    try
    {
        options_description desc{"bayan tool options"};
        desc.add_options()
        ("help,h", "Help screen")
        ("directories,d", value<std::string>()->default_value("./"), "Directories to scan (may set several, space separated)")
        ("except,e", value<std::string>()->default_value(""), "Directories to skip during scan (may set several, space separated)")
        ("recursive,r", value<bool>()->default_value(false),"Make recursive search (same for all directories), false to stay in defined directories only")
        ("min-file-size,mfs",value<size_t>()->default_value(1),"Minimum file size in bytes")
        ("mask,m",value<std::string>()->default_value(".*"), "File name masks, allowed for comparison (case insetivie)")
        ("block-size,b",value<size_t>()->default_value(5),"Size of block during scan (bytes)")
        ("hash-algorithm,hsh", value<std::string>()->default_value("crc32"), "Hash algorithm to use (currently supported crc32, md5)");

        variables_map vm;   
        store(parse_command_line(argc, argv, desc), vm);

        if (vm.count("help"))
        std::cout << desc << '\n';
/* Just some sanity checks for debug
        if (vm.count("directories"))
        std::cout << "Directories: " << vm["directories"].as<std::string>() << '\n';
        if (vm.count("level"))
        std::cout << "Level: " << vm["level"].as<int>() << '\n';
        if (vm.count("except"))
        std::cout << "Except: " << vm["except"].as<std::string>() << '\n';
*/

        std::string directories_to_scan = vm["directories"].as<std::string>();
        std::string directories_to_skip = vm["except"].as<std::string>();
        std::string mask = vm["mask"].as<std::string>();
        bool recursive = vm["recursive"].as<bool>();
        Filter files_filter(vm["min-file-size"].as<size_t>());
        auto files_to_process = files_filter.process_query(directories_to_scan, directories_to_skip, mask, recursive);
        
        //for(auto f:files_to_process)
        //    std::cout << f << std::endl;
    }
    catch (const error &ex)
    {
        std::cerr << ex.what() << '\n';
    }
    return 0;
}
