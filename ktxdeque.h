#pragma once


#include <initializer_list>
#include <memory>
#include <iterator>
#include <iostream>
#include "../ktxvector/ktxvector.h"

namespace ktx {



template <typename T, typename Allocator = std::allocator<T>>
class deque {
private:
    template <bool isConst>
    class base_iterator;

public:
    using value_type = T;
    using size_type = std::size_t;
    using allocator_type = Allocator;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = typename std::allocator_traits<Allocator>::pointer;
    using const_pointer = typename 
        std::allocator_traits<Allocator>::const_pointer;
    using iterator = base_iterator<false>;
    using const_iterator = base_iterator<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using alloc_traits = std::allocator_traits<allocator_type>;
    

private:
    size_type ai_;
    size_type sz_;

#if defined(KTXDEBUG)
    using rebinded = std::allocator<pointer>;
#else 
    using rebinded = typename alloc_traits::template rebind_alloc<pointer>;
#endif
    vector<pointer, rebinded> outer_;
    [[no_unique_address]] allocator_type alloc_;

    static constexpr int BlockSize = 8;

public:
    // constructors and assign
    deque() : ai_{0}, sz_{0}, outer_{}, alloc_{} {}

    deque(std::initializer_list<value_type> list, 
            Allocator alloc = Allocator());

    explicit deque(Allocator a) 
        : ai_{}, sz_{0}, outer_{}, alloc_{a} {}

    explicit deque(size_type n, const T& val = T(), Allocator a = Allocator());

    deque(const deque& other);

    deque(deque&& other) noexcept : deque{} { swap(*this, other); }

    deque& operator=(deque other) {
        swap(*this, other);
        return *this;
    }

    // dtor
    ~deque();

    // modifiers

    template<typename... Args>
    void emplace(const_iterator, Args&&... args);

    void insert(const_iterator pos, value_type value);

    iterator erase(const_iterator pos);

    void clear();

    void push_back(value_type value);

    void push_front(value_type value);

    template <typename... Args>
    void emplace_back(Args&&... args); 

    template <typename... Args>
    void emplace_front(Args&&... args); 

    void pop_back();

    void pop_front();

    void shrink_to_fit();

    void resize(size_type count);

    void resize(size_type count, const value_type& value);

    template <typename U, typename A>
    friend void swap(deque<U, A>& to, deque<U, A>& from);

    // accessors
    template <typename Self>
    constexpr auto operator[](this Self&& self, size_type index) -> 
    std::conditional_t<
        std::is_const_v<std::remove_reference_t<Self>>,
        const_reference,
        reference> {
        auto bi = (self.ai_ + index) / BlockSize;
        auto ri = (self.ai_ + index) % BlockSize;
        return std::forward<Self>(self).outer_[bi][ri];
    }

    size_type size() const { return sz_; }

    // TODO:
    size_type capacity() const { return -1; }

    [[nodiscard]] bool empty() { return sz_; }

    // TODO:
    template <typename Self>
    constexpr auto back(this Self&& self) -> 
    std::conditional_t<
        std::is_const_v<std::remove_reference_t<Self>>,
        const_pointer,
        pointer> {
    }

    // TODO:
    template <typename Self>
    constexpr auto front(this Self&& self) -> 
    std::conditional_t<
        std::is_const_v<std::remove_reference_t<Self>>,
        const_pointer,
        pointer> {
    }

    template <typename Self>
    constexpr auto at(this Self&& self, size_type index) -> 
    std::conditional_t<
        std::is_const_v<std::remove_reference_t<Self>>,
        const_reference,
        reference>;

    // iterator

    iterator begin() {
        return {outer_.data(), ai_};
    }

    iterator end() {
        return {outer_.data(), ai_ + sz_};
    }

    
    const_iterator begin() const {
        return {outer_.data(), ai_};
    }

    const_iterator end() const {
        return {outer_.data(), ai_ + sz_};
    }

    struct D {
        D() = delete;
    };
    const_iterator cbegin() const {
        if constexpr (std::is_const_v<std::remove_reference_t<decltype(outer_.data())>>) {
            D d;
        }
        return {outer_.data(), ai_};
    }

    const_iterator cend() const {
        return {outer_.data(), ai_ + sz_};
    }

private:
    template <std::input_iterator InputIt,
             std::forward_iterator NoThrowForwardIt>
    auto uninitialized_move(
            InputIt first,
            InputIt last,
            NoThrowForwardIt d_first) -> NoThrowForwardIt;

    template <std::input_iterator InputIt,
             std::forward_iterator NoThrowForwardIt,
             std::predicate<value_type&&> UnaryPred>
    auto uninitialized_move_if(
            InputIt first,
            InputIt last,
            NoThrowForwardIt d_first,
            UnaryPred P = [](){return false;}) -> NoThrowForwardIt;

    void allocateBlocks(size_t n) {
        outer_.resize(n);
        size_t i = 0;
        try {
            for (; i < n; ++i) {
                auto& p = outer_[i];
                p = alloc_traits::allocate(alloc_, BlockSize);
            }
        } catch (std::bad_alloc&) {
            for (size_t j = 0; j != i; ++j) {
                auto p = outer_[j];
                alloc_traits::deallocate(alloc_, p, BlockSize); 
            }
            throw;
        }
    }

    void deallocateBlocks() {
        for (auto p: outer_) {
            alloc_traits::deallocate(alloc_, p, BlockSize);
        }
    }

    template<bool isConst>
    class base_iterator {
    public:
        friend class deque<T, Allocator>;
        using iterator_category = std::random_access_iterator_tag;
        using value_type = deque<T>::value_type;
        using difference_type = ptrdiff_t;
        using pointer = std::conditional_t<isConst, const T*, T*>;
        using pointer_to_pointer = std::conditional_t<isConst, const T* const*, T**>;
        using reference = std::conditional_t<isConst, const T&, T&>;

        base_iterator(const base_iterator&) = default;
        base_iterator& operator=(const base_iterator&) = default;
        base_iterator(base_iterator&&) = default;
        base_iterator& operator=(base_iterator&&) = default;
        ~base_iterator() = default;

        friend difference_type operator-(base_iterator a,
                base_iterator b) {
            return a.ai_ - b.ai_;
        }

        friend base_iterator operator+(base_iterator it,
                                               difference_type index) {
           it.ai_ += index;
           return {it};
        }

        friend base_iterator operator-(base_iterator it,
                                               difference_type index) {
           it.ai_ -= index;
           return {it};
        }

        friend base_iterator operator+(difference_type index,
                                               base_iterator it) {
           it.ai_ += index;
           return {it};
        }

        friend base_iterator operator-(difference_type index,
                                               base_iterator it) {
           it.ai_ -= index;
           return {it};
        }

        iterator& operator+=(difference_type index) {
            ai_ += index;
        }

        iterator& operator-=(difference_type index) {
            ai_ -= index;
        }

        base_iterator& operator++() {
            ++ai_;
            return *this;
        }

        base_iterator operator++(int) {
            base_iterator it = *this;
            ++ai_;
            return it;
        }

        base_iterator& operator--() {
            --ai_;
            return *this;
        }

        base_iterator operator--(int) {
            base_iterator it = *this;
            --ai_;
            return it;
        }

        reference operator*() const {
            auto bi = ai_ / BlockSize;
            auto ri = ai_ % BlockSize;
            return ptr_[bi][ri];
        }

        pointer operator->() const {
            auto bi = ai_ / BlockSize;
            auto ri = ai_ % BlockSize;
            return ptr_[bi] + ri;
        }

        auto operator<=>(const base_iterator& it) const = default;

        operator base_iterator<true>() const {
            return {ptr_, ai_};
        }

    private:
        pointer_to_pointer ptr_;
        size_t ai_;
        base_iterator(pointer_to_pointer ptr, size_t ai) noexcept
            : ptr_{ptr}, ai_{ai} {}
    };
};

template<typename T, typename Allocator>
std::ostream& operator<<(std::ostream& os, const deque<T, Allocator>& vec);


}

#include "ktxdeque_realization.h" // IWYU pragma: keep
