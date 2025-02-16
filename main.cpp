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
        ("directories,d", value<std::string>(), "Directories to scan (may set several)")
        ("except,e", value<std::string>()->default_value(""), "Directories to skip during scan")
        ("level,l", value<int>()->default_value(0),"Level of scanning (same for all directories), 0 to stay in directories only")
        ("min-file-size,mfs",value<int>()->default_value(1),"Minimum file size in bytes")
        ("mask,m",value<std::string>()->default_value(".*"), "File name masks, allowed for comparison (case insetivie)")
        ("block-size,b",value<int>()->default_value(5),"Size of block during scan")
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
    }
    catch (const error &ex)
    {
        std::cerr << ex.what() << '\n';
    }
    return 0;
}
