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
			std::cout << "allocated address " << data << std::endl;
        }
		if (!data)
			throw std::bad_alloc();
        auto prev_el = el_count;
        el_count += n;
		std::cout << "cur el count = " << el_count << std::endl;
		return reinterpret_cast<T *>(data + prev_el);
	}

	void deallocate(T *p, std::size_t n) {
		std::cout << "before dealloc el_count = " << el_count << std::endl;
		el_count -= n;
#ifndef USE_PRETTY
		std::cout << "deallocate: [n  = " << n << "], address " << p << " cur el count " << el_count << std::endl;
#else
		std::cout << __PRETTY_FUNCTION__ << "[n = " << n << "]" << std::endl;
#endif
        
        if(data != nullptr && el_count == 0)
        {
			std::cout << "Actual free address: " << data << std::endl;
		    std::free(data);
            data = nullptr;
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


// Custom vector container
template <class T, class Allocator = std::allocator<T>> class MyVec{
  public:
    // Type definitions for easier access to types used in the container
    // Type of elements stored in the container
    using value_type = T;

    // Allocator type used for memory management
    using allocator_type = Allocator;

    // Reference type to the container's elements
    using reference = value_type &;

    // Constant reference type to the container's elements
    using const_reference = const value_type &;

    // Type for size and capacity of the container
    using size_type = size_t;

    // Type for difference between iterator positions
    using difference_type = ptrdiff_t;

    // Iterator class for the custom container
    class iterator{
      public:
        // Type definitions for the iterator

        // Category tag for random access iterators
        using iterator_category = std::random_access_iterator_tag;

        // Type of elements the iterator points to
        using value_type = T;

        // Type for difference between iterators
        using difference_type = ptrdiff_t;

        // Pointer type to the element
        using pointer = T *;
        // Reference type to the element
        using reference = T &;

        // Constructor to initialize the iterator with a pointer to an element
        iterator(pointer ptr) : ptr_(ptr) {
        }

        // Dereference operator to access the element pointed to by the iterator
        reference operator*() const{
            return *ptr_;
        }

        // Arrow operator to access the element's members
        pointer operator->() const{
            return ptr_;
        }

        // Prefix increment operator to move the iterator to the next element
        iterator &operator++(){
            ++ptr_;
            return *this;
        }

        // Postfix increment operator to move the iterator to the next element and return the previous
        // position
        iterator operator++(int) {
            iterator temp = *this;
            ++ptr_;
            return temp;
        }

        // Equality operator to compare if two iterators are equal (point to the same element)
        bool operator==(const iterator &other) const{
            return ptr_ == other.ptr_;
        }

        // Inequality operator to compare if two iterators are not equal (point to different elements)
        bool operator!=(const iterator &other) const{
            return ptr_ != other.ptr_;
        }

      private:
        pointer ptr_; // Pointer to the element the iterator currently points to
    };

    // Default constructor to create an empty container
    MyVec() : data_(nullptr), size_(0), capacity_(0){
    }

	void reserve(size_t n)
	{
        std::cout << "Called reserve" << std::endl;
		data_ = my_alloc.allocate(n);
		std::cout << "Reserved data: " << data_ << std::endl;
		capacity_ = n;
	}


    // Destructor to clean up the allocated memory when the container is destroyed
    ~MyVec(){
        // Deallocate the allocated memory
		std::cout << "Call vector destructor of size " << size_ << " and capacity " << capacity_ << std::endl;
		std::cout << "Cur data_: " << data_ << std::endl;
        my_alloc.deallocate(data_, capacity_);
    }

    // Member function to add a new element at the end of the container
    void push_back(const T &value){
		if(size_ >= capacity_)
		{
			auto new_data_ = my_alloc.allocate(2*capacity_);
			capacity_ *= 2;
			for(size_t i = 0 ; i < size_; i++)
				new_data_[i] = data_[i];
			my_alloc.deallocate(data_, size_);
			data_ = new_data_;
		}
		data_[size_++] = value;
    }

    // Member function to remove the last element from the container
    void pop_back(){
        // Only remove if the container is not empty
        if (size_ > 0){
            --size_;
        }
    }

    // Member function to access an element at a given index with bounds checking
    reference at(size_type index){
        if (index >= size_){

            // Throw an exception if the index is invalid
            throw std::out_of_range("Index out of range");
        }
        return data_[index];
    }

    // Const version of the 'at' function to access elements in a read-only context
    const_reference at(size_type index) const{
        if (index >= size_){

            // Throw an exception if the index is invalid
            throw std::out_of_range("Index out of range");
        }
        return data_[index];
    }

    // Member function to get the number of elements currently in the container
    size_type size() const{
        return size_;
    }

    // Member function to check if the container is empty
    bool empty() const{
        return size_ == 0;
    }

    // Member function to get an iterator pointing to the first element of the container
    iterator begin(){
        return iterator(data_);
    }

    // Member function to get an iterator pointing to the end (one past the last element) of the container
    iterator end(){
        return iterator(data_ + size_);
    }

  private:
    // Pointer to the container's elements
    T *data_;

    // Number of elements in the container
    size_type size_;

    // Allocated capacity of the container
    size_type capacity_;

	allocator_type my_alloc;
};