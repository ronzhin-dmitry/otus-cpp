#include "lib.h"

int main()
{
    std::cout << "--------------------------------------------" << std::endl;
    std::cout << "STD container + STD allocator" << std::endl;
    //STD allocator for std::map
    {
        auto m_std = std::map<int, int>{};
        for(int i = 0 ; i < 10; i++)
            m_std[i] = iter_factorial(i);
        for(int i = 0 ; i < 10; i++)
            std::cout << "m_std[" << i << "] = " << m_std[i] << std::endl;
    }

    std::cout << "--------------------------------------------" << std::endl;
    std::cout << "STD container + Custom allocator" << std::endl;
    //Custom allocator for std::map - max size 10
    {
        auto m_custom = std::map<int, int, std::less<int>, linear_allocator<std::pair<const int, int>, 10>>{};
        for(int i = 0 ; i < 10; i++)
            m_custom[i] = iter_factorial(i);
        for(int i = 0 ; i < 10; i++)
            std::cout << "m_custom[" << i << "] = " << m_custom[i] << std::endl;
    }

    std::cout << "--------------------------------------------" << std::endl;
    std::cout << "Custom container + custom allocator" << std::endl;

    //Custom container with custom allocator
    {
        auto v_custom = MyVec<int, linear_allocator<int, 10>>{};
        v_custom.reserve(10);
        //Custom allocator for std::map - max size 10
        for(int i = 0 ; i < 10; i++)
        {
            v_custom.push_back(iter_factorial(i));
        }
        std::cout << "custom container with custom allocator elements:" << std::endl;
        for(auto i = v_custom.begin() ; i != v_custom.end(); i++)
            std::cout << (*i) << std::endl;
        
    }
    //Bad range - throw error
    try {
    std::cout << "--------------------------------------------" << std::endl;
    std::cout << "STD container + custom allocator + memory overflow" << std::endl;
    auto m = std::map<int, int, std::less<int>, linear_allocator<std::pair<const int, int>, 10>>{};
    for(int i = 0 ; i < 11; i++)
         m[i] = iter_factorial(i); //get alloc error, idk how to catch
    } catch (const std::bad_alloc& e) {
        std::cout << "Memory allocator bad alloc: " << e.what() << std::endl; //catched badalloc
    }
    return 0;
}
