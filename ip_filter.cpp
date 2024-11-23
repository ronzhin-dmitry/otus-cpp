#include "lib.h"

int main()
{
    try
    {
        std::vector<std::array<int, 4>> ip_pool;
        ip_pool.reserve(100);
        
        ip_fill(ip_pool);

        ip_sort(ip_pool);
        ip_print_all(ip_pool);
 
        ip_print_all(ip_filter_all(ip_pool, 1));

        ip_print_all(ip_filter_all(ip_pool,46,70));

        ip_print_all(ip_filter_any(ip_pool,46));

    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
