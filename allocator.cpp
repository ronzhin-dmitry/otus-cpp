#include "lib.h"

int main()
{
    //Custom allocator for std::map - max size 10
    {
        auto m = std::map<int, int, std::less<int>, linear_allocator<std::pair<const int, int>, 10>>{};
        for(int i = 0 ; i < 10; i++)
            m[i] = iter_factorial(i);
        for(int i = 0 ; i < 10; i++)
            std::cout << "m[" << i << "] = " << m[i] << std::endl;
        
        //m[10] = iter_factorial(10); // here we get abort, idk how to catch it
    }
    auto m = std::map<int, int, std::less<int>, linear_allocator<std::pair<const int, int>, 10>>{};
    for(int i = 0 ; i < 11; i++)
         m[i] = iter_factorial(i); //get alloc error, idk how to catch

    return 0;
}
