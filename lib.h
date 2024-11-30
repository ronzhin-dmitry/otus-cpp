#pragma once
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <array>
#include <tuple>
#include <algorithm>


int version();

template <typename T, int MAX_SIZE>
struct linear_allocator {
	using value_type = T;

	using pointer = T *;
	using const_pointer = const T *;
	using reference = T &;
	using const_reference = const T &;

    pointer data = nullptr;
    int el_count = 0;
	template <typename U>
	struct rebind {
		using other = linear_allocator<U, MAX_SIZE>;
	};

	linear_allocator() = default;
	~linear_allocator() = default;

	template <typename U>
	linear_allocator(const linear_allocator<U, MAX_SIZE> &) {
	}

	T *allocate(std::size_t n) {
#ifndef USE_PRETTY
		std::cout << "allocate: [n = " << n << "], cur el_count = " << el_count << std::endl;
#else
		std::cout << __PRETTY_FUNCTION__ << "[n = " << n << "]" << std::endl;
#endif
        if(el_count + n > MAX_SIZE)
        {
            std::cout << "Not enough memory" << std::endl;
            throw std::bad_alloc();
        }
        std::cout << "expected size = " << el_count + n << std::endl;
        if(data == nullptr)
        {
            std::cout << "real allocation here" << std::endl;
		    data = (pointer)(std::malloc(MAX_SIZE * sizeof(T)));
        }
		if (!data)
			throw std::bad_alloc();
        el_count += n;
		return reinterpret_cast<T *>(data + el_count);
	}

	void deallocate(T *p, std::size_t n) {
#ifndef USE_PRETTY
		std::cout << "deallocate: [n  = " << n << "], address " << p << std::endl;
#else
		std::cout << __PRETTY_FUNCTION__ << "[n = " << n << "]" << std::endl;
#endif
        el_count -= n;
        if(data != nullptr && el_count == 0)
        {
		    std::free(data);
            data = nullptr;
            std::cout << "Actual free" << std::endl;
        }
	}

	template <typename U, typename... Args>
	void construct(U *p, Args &&...args) {
#ifndef USE_PRETTY
		std::cout << "construct" << std::endl;
#else
		std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
		new (p) U(std::forward<Args>(args)...);
	}

	template <typename U>
	void destroy(U *p) {
#ifndef USE_PRETTY
		std::cout << "destroy" << std::endl;
#else
		std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
		p->~U();
	}
};

unsigned int iter_factorial(unsigned int n);