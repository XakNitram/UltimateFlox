module;
#include "pch.hpp"
export module RawArray;


export template<typename T>
class RawArray {
    struct iterator {
        using iterator_category = std::contiguous_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        explicit iterator(pointer _ptr) : ptr(_ptr) {}

        iterator &operator=(iterator const&) noexcept = default;
        [[nodiscard]] reference operator*() const noexcept { return *ptr; }
        [[nodiscard]] pointer operator->() const noexcept { return ptr; }
        iterator &operator++() noexcept { ++ptr; return *this; }
        iterator operator++(int) noexcept { iterator retval = *this; ++(*this); return retval; } // Gets a warning, but this is exactly the msvc code for vector::operator++(int)
        iterator &operator--() noexcept { --ptr; return *this; }
        iterator operator--(int) noexcept { iterator retval = *this; --(*this); return retval; }  // Gets a warning, but this is exactly the msvc code for vector::operator--(int)
        iterator &operator+=(difference_type _off) noexcept { ptr += _off; return *this; }
        [[nodiscard]] iterator operator+(difference_type _off) const noexcept { iterator retval = *this; retval += _off; return retval; }
        iterator &operator-=(difference_type _off) noexcept { ptr += -_off; return *this; }
        [[nodiscard]] iterator operator-(difference_type _off) const noexcept { iterator retval = *this; retval -= _off; return retval; }
        [[nodiscard]] difference_type operator-(iterator const& _rhs) const noexcept { return ptr - _rhs.ptr; }
        [[nodiscard]] reference operator[](const difference_type _off) const noexcept { return *(*this + _off); }
        [[nodiscard]] bool operator==(iterator const& _rhs) const noexcept { return ptr == _rhs.ptr; }
        [[nodiscard]] std::strong_ordering operator<=>(iterator const& _rhs) { return std::addressof(*ptr) <=> std::addressof(*_rhs.ptr); }

    private:
        pointer ptr;
    };

    T *m_data;
    const std::size_t m_size;
public:
    RawArray(T *data, std::size_t size) : m_data(data), m_size(size) {}

    iterator begin() {
        return iterator(m_data);
    }

    iterator end() {
        return iterator(m_data + m_size);
    }
};
