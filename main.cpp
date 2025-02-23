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
        ("min-file-size",value<size_t>()->default_value(1),"Minimum file size in bytes")
        ("mask,m",value<std::string>()->default_value(".*"), "File name masks, allowed for comparison (case insensitive)")
        ("block-size,b",value<size_t>()->default_value(3),"Size of block during scan (bytes)")
        ("hash-algorithm", value<std::string>()->default_value("crc32"), "Hash algorithm to use (currently supported crc32, md5)")
        ("print-params,p",value<bool>()->default_value(false),"Display run parameters at startup");

        variables_map vm;   
        store(parse_command_line(argc, argv, desc), vm);

        if (vm.count("help"))
        std::cout << desc << '\n';

        //Getting some arguments from CLI
        std::string directories_to_scan = vm["directories"].as<std::string>();
        std::string directories_to_skip = vm["except"].as<std::string>();
        std::string mask = vm["mask"].as<std::string>();
        bool recursive = vm["recursive"].as<bool>();
        size_t block_size = vm["block-size"].as<size_t>();
        bool print_params = vm["print-params"].as<bool>();
        size_t min_file_size = vm["min-file-size"].as<size_t>();
        std::string hash_algo = vm["hash-algorithm"].as<std::string>();

        //Creating classes for request processing
        Filter files_filter(min_file_size);
        Hasher file_hasher(hash_algo);
        Scanner file_scanner(file_hasher, block_size);

        if(print_params)
        {
            std::cout << "Running BAYAN tool..." << std::endl;
            std::cout << "Directories to scan:" << directories_to_scan << std::endl;
            std::cout << "Directories to skip:" << directories_to_skip << std::endl;
            std::cout << "Mask:" << mask << std::endl;
            std::cout << "Recursive:" << recursive << std::endl;
            std::cout << "Block size:" << block_size << std::endl;
            std::cout << "Minimum file size:" << min_file_size << std::endl;
            std::cout << "Hashing algorithm:" << hash_algo << std::endl;
            std::cout << std::endl;
        }

        //Get full file list for further scan, based on regex condition
        auto files_to_process = files_filter.process_query(directories_to_scan, directories_to_skip, mask, recursive);

        //Search for duplicates
        auto final_res = file_scanner.find_duplicates(files_to_process);
        
        //Print final results
        for(auto& group: final_res)
        {
            for(auto& file: group)
            {
                std::cout << file << std::endl;
            }
            std::cout << std::endl;
        }
    }
    catch (const error &ex)
    {
        std::cerr << ex.what() << '\n';
    }
    return 0;
}
