#pragma once

#include "ktxdeque.h"


namespace ktx {



// public

// TODO:
template<typename T, typename Allocator>
deque<T, Allocator>::deque(
        const std::initializer_list<value_type> items, Allocator alloc
        ) {
}

// TODO:
template<typename T, typename Allocator>
deque<T, Allocator>::deque(size_type n, const T& val, Allocator a) 
    : sz_{n} {
}

// TODO:
template<typename T, typename Allocator>
deque<T, Allocator>::deque(const deque<T, Allocator>& other) {
}

// TODO:
template <typename T, typename Allocator>
template <typename... Args>
void deque<T, Allocator>::emplace_back(Args&&... args) {
}

// TODO:
template <typename T, typename Allocator>
template <typename... Args>
void deque<T, Allocator>::emplace_front(Args&&... args) {
}

template<typename T, typename Allocator>
void deque<T, Allocator>::push_back(value_type value) { 
    emplace_back(std::move(value));
}

template<typename T, typename Allocator>
void deque<T, Allocator>::push_front(value_type value) { 
    emplace_front(std::move(value));
}

// TODO:
template<typename T, typename Allocator>
void deque<T, Allocator>::clear() {
}

// TODO:
template<typename T, typename Allocator>
deque<T, Allocator>::~deque() {
}

template<typename T, typename Allocator>
template <typename Self>
constexpr auto deque<T, Allocator>::at(this Self&& self,
                                        size_type index) -> 
std::conditional_t<
    std::is_const_v<std::remove_reference_t<Self>>,
    const_reference,
    reference> {
        if (index >= self.sz_) {
            throw std::out_of_range{"Index is out of range of deque"};
        }
        return std::forward<Self>(self).data_[index];
}

// TODO:
template<typename T, typename Allocator>
void deque<T, Allocator>::resize(size_type count, const value_type& val) {
}

template<typename T, typename Allocator>
void deque<T, Allocator>::resize(size_type count) {
    resize(count, T());
}

// TODO:
template<typename T, typename Allocator>
void deque<T, Allocator>::shrink_to_fit() {
}

// TODO:
template <typename T, typename Allocator>
void deque<T, Allocator>::pop_back() {
}

// TODO:
template <typename T, typename Allocator>
void deque<T, Allocator>::pop_front() {
}

// private


// TODO:
template<typename T, typename Allocator>
void deque<T, Allocator>::destroy_all() {
}


template <typename T, typename Allocator>
template <std::input_iterator InputIt,
         std::forward_iterator NoThrowForwardIt,
         std::predicate<T&&> UnaryPred>
auto deque<T, Allocator>::uninitialized_move_if(
       InputIt first,
       InputIt last,
       NoThrowForwardIt d_first,
       UnaryPred P) -> NoThrowForwardIt {
    auto current = d_first;
    try {
        for(; first != last; ++first, ++current) {
            if (P(*first)) {
                ++current;
            }
            alloc_traits::construct(alloc_,
                            std::addressof(*current),
                            std::move_if_noexcept(*first));
        }
        return current;
    } catch (...) {
        for (; d_first != current; ++d_first) {
            alloc_traits::destroy(alloc_, d_first);
        }
        throw;
    }
}

template <typename T, typename Allocator>
template <std::input_iterator InputIt,
         std::forward_iterator NoThrowForwardIt>
auto deque<T, Allocator>::uninitialized_move(
        InputIt first,
        InputIt last,
        NoThrowForwardIt d_first) -> NoThrowForwardIt {
    auto current = d_first;
    try {
        for(; first != last; ++first, ++current) {
            alloc_traits::construct(alloc_,
                            std::addressof(*current),
                            std::move_if_noexcept(*first));
        }
        return current;
    } catch (...) {
        for (; d_first != current; ++d_first) {
            alloc_traits::destroy(alloc_, d_first);
        }
        throw;
    }
}


// TODO:
template<typename T, typename Allocator>
template<typename... Args>
void deque<T, Allocator>::emplace(const_iterator pos, Args&&... args) {
}

template<typename T, typename Allocator>
void deque<T, Allocator>::insert(const_iterator pos, value_type value) {
    emplace(pos, std::move(value));
}

template<typename T, typename Allocator>
deque<T, Allocator>::iterator deque<T, Allocator>::erase(
        const_iterator pos) {
    iterator p = begin() + std::distance(cbegin(), pos);
    for (auto it = p; it != end() - 1; ++it) {
        *it = std::move(*(it + 1));
    }
    alloc_traits::destroy(alloc_, std::addressof(*(cend() - 1)));
    --sz_;

    return p;
}

// friend

// TODO:
template<typename T, typename Allocator>
void swap(deque<T, Allocator>& to, deque<T, Allocator>& from) {
}

// global

template<typename T, typename Allocator>
std::ostream& operator<<(std::ostream& os,
        const deque<T, Allocator>& vec) {
    for (auto it = std::begin(vec); it != std::end(vec); ++it) {
        os << *it;
        if (it != std::end(vec) - 1) {
            os << ' ';
        }
    }

    return os;
}


};


