#pragma once
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <array>
#include <tuple>
#include <algorithm>
#include <type_traits>
#include <unordered_map>

/**
 * @brief simple function to return version of the release
 */
int version();

/**
 * @brief class to store sparse matrix
 * no constructor will be publicly available
 * only possible access point would be proxy-class Matrix
 */
template <typename T, int D>
class Matrix
{
protected:
    std::unordered_map<uint64_t, std::unordered_map<uint64_t, T>> index;
public:
    /**
     * @brief proxy-class for matrix to support default-value on empty cells
     */
    class MatrixProxy
    {
        Matrix *mat;
        int i, j;
    public:
        MatrixProxy(Matrix* m, int i_, int j_):mat(m), i(i_), j(j_) {};
        operator T() const { 
            if(mat->index.find(i) != mat->index.end() && mat->index[i].find(j) != mat->index[i].end())
                return mat->index[i][j];
            else
                return T(D); 
        };

        void operator= (T value) {
            if(value != D)
                mat->index[i][j] = value;
            else
            {
                if(mat->index.find(i) != mat->index.end() && mat->index[i].find(j) != mat->index[i].end())
                    mat->index[i].erase(mat->index[i].find(j));
            }
        };
    };
    /**
     * @brief helper class to represent matrix row (and allow indexing via mat[][])
     */
    class MatrixRow
    {
    private:
        Matrix *parent;
        int row_num;
    public:
        MatrixRow(Matrix* par, int R): parent(par), row_num(R){};
        MatrixProxy operator[](int col_num) {
            return MatrixProxy{parent, row_num, col_num};
        };
        const MatrixProxy operator[](int col_num) const {
            return MatrixProxy{parent, row_num,col_num};
        };
    };

    /**
     * @brief returns matrix row class which allows for double brackets
     */
    MatrixRow operator[](int row_num) {return MatrixRow(this, row_num);};
     /**
     * @brief returns matrix row class which allows for double brackets
     */
    const MatrixRow operator[](int row_num) const {return MatrixRow(this, row_num);};
     
     /**
     * @brief just some helper function to print container (without iterators of matrix)
     */
    void print_container()
    {
        for(auto it1 = index.begin(); it1 != index.end() ; it1++)
        {
            for(auto it2 = it1->second.begin(); it2 != it1->second.end(); it2++)
            {
                std::cout  << "Matrix[" << it1->first << "][" << it2->first << "] = " << it2->second << std::endl;
            }
        }
    }
    Matrix(){};

    /**
     * @brief helper function to retrieve container size (iterates over inner container)
     * Currently not optimal, probably can be done better via some internatl counters for non-default elements
     */
    size_t size()
    {
        if(index.size() == 0)
            return 0;
        else
        {
            size_t res = 0;
            for(auto r: index)
                res += r.second.size();
            return res;
        }
    }
    /**
     * @brief iterator class for sparse matrix
     * has to deal with double-indexed container inside
     */
    class ItemIterator {
        typename std::unordered_map<uint64_t, std::unordered_map<uint64_t, T>>::iterator i_outer;
        typename std::unordered_map<uint64_t, T>::iterator i_inner;
        Matrix <T,D> &mat;
    public:
        ItemIterator(Matrix<T,D> &mat_, typename std::unordered_map<uint64_t, std::unordered_map<uint64_t, T>>::iterator i): i_outer(i), mat(mat_) {
            if(i_outer  != mat.index.end())
                i_inner = i_outer->second.begin();
        }

        // incrementing
        ItemIterator & operator ++() { 
            if(i_outer  != mat.index.end() && i_inner != i_outer->second.end())
                ++i_inner; 
            if(i_inner ==  i_outer->second.end())
            {
                ++i_outer;
                if(i_outer  != mat.index.end())
                {
                    i_inner = i_outer->second.begin();
                }
            }
            return *this; 
        }
        ItemIterator operator ++(int) { 
            auto old(*this);
            ++(*this);
            return old; }

        // comparison
        bool operator!=(const ItemIterator &o) const { 
            if(i_outer  != mat.index.end())
                return (i_outer != o.i_outer) || (i_inner != o.i_inner); 
            else
                return i_outer != o.i_outer;
        }

        // dereferencing
        const auto operator*() const { 
            return std::make_tuple(i_outer->first, i_inner->first, i_inner->second); 
        }
        friend class Matrix;
    };

    ItemIterator begin() { return ItemIterator(*this, index.begin()); }
    ItemIterator end()   { return ItemIterator(*this, index.end()  ); }

    friend class MatrixRow;
    friend class MatrixProxy;
    friend class ItemIterator;
};
