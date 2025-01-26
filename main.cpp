/**
@file main.cpp
*/
#include "lib.h"

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
int main()
{
    //Usage of sparse matrix made with Proxy-pattern
    Matrix<int, 0> mat;
    for(int i = 0 ; i < 10 ; i++)
    {
        mat[i][i] = i;
        mat[i][9-i] = 9-i;
    }
    for(int i = 1; i < 9; i++)
    {
        for(int j = 1; j < 9; j++)
            std::cout << mat[i][j] << " ";
        std::cout << std::endl;
    }
    std::cout << "Amount of empty cells = " << mat.size() << std::endl;
    assert(mat.size() == 18);
    std::cout << "Range based for iteration over non-zero cells:" << std::endl;
    for(auto c: mat)
    {
        int x;
        int y;
        int v;
        std::tie(x, y, v) = c;
        std::cout << "mat[" << x << "][" << y << "] = " << v << std::endl;
    }
    return 0;
}
