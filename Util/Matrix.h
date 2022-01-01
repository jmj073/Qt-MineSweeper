#pragma once

#include <memory>
#include <functional>

//#include "DeleterForAllocator.h"

template<typename VT, typename _Alloc = std::allocator<VT>>
struct DeleterForAllocator
{
    size_t elem_size;

    DeleterForAllocator(size_t size)
        : elem_size{ size } {}

    void operator()(VT* ptr)
    {
        for (size_t i = elem_size; i-- > 0;)
            ptr[i].~VT();
        _Alloc allocator{};
        allocator.deallocate(ptr, elem_size);
        std::cout << "deleted!\n";
    }
};


template <typename VT, typename _Alloc = std::allocator<VT>>
class Matrix
{
public:
    using value_type = VT;
    using allocator_type = _Alloc;
    using reference = VT&;
    using const_reference = const VT&;
    using size_type = size_t;

public:
    Matrix(size_type row, size_type col, const VT& def = VT());
    Matrix(const Matrix& other);
    Matrix(Matrix&& other) noexcept;

    size_type row() const
    {
        return m_row_size;
    }
    size_type col() const
    {
        return m_col_size;
    }
    size_type size() const
    {
        return row() * col();
    }

    VT* operator[](size_type row_idx)
    {
        return &m_elem[row_idx * m_col_size];
    }
    const VT* operator[](size_type row_idx) const
    {
        return &m_elem[row_idx * m_col_size];
    }
    VT& at(size_type row_idx, size_type col_idx)
    {
        return m_elem[row_idx * m_col_size + col_idx];
    }
    const VT& at(size_type row_idx, size_type col_idx) const
    {
        return m_elem[row_idx * m_col_size + col_idx];
    }

    Matrix& operator=(const Matrix& other);
    Matrix& operator=(Matrix&& other);

    ~Matrix()
    {
        destroy();
        m_elem = nullptr;
    }

private:
    void destroy();

private:
    _Alloc m_allocator;
    VT* m_elem;
    size_type m_row_size;
    size_type m_col_size;
};

template <typename VT, typename _Alloc>
Matrix<VT, _Alloc>::Matrix(size_type row_size, size_type col_size, const VT& def)
    : m_row_size{ row_size }, m_col_size{ col_size }
{
    m_elem = m_allocator.allocate(size());
    std::uninitialized_fill(m_elem, m_elem + size(), def);
}

template <typename VT, typename _Alloc>
Matrix<VT, _Alloc>::Matrix(const Matrix& other)
    : m_row_size{ other.m_row_size }, m_col_size{ other.m_col_size }
{
    m_elem = m_allocator.allocate(other.size());
    std::uninitialized_copy(other.m_elem, other.m_elem + other.size(), m_elem);
}

template <typename VT, typename _Alloc>
Matrix<VT, _Alloc>::Matrix(Matrix&& other) noexcept
    : m_row_size{ other.m_row_size }, m_col_size{ other.m_col_size }, m_elem{ other.m_elem }
{
    other.m_row_size = 0;
    other.m_col_size = 0;
    other.m_elem = nullptr;
}

template <typename VT, typename _Alloc>
Matrix<VT, _Alloc>& Matrix<VT, _Alloc>::operator=(const Matrix& other)
{
    DeleterForAllocator<VT, _Alloc> deleter{ other.size() };
    std::unique_ptr<VT, DeleterForAllocator<VT, _Alloc>> new_elem{ m_allocator.allocate(other.size()), deleter };
    
    std::uninitialized_copy(other.m_elem, other.m_elem + other.size(), new_elem.get());

    destroy();

    m_row_size = other.m_row_size;
    m_col_size = other.m_col_size;
    m_elem = new_elem.release();

    return *this;
}

template <typename VT, typename _Alloc>
Matrix<VT, _Alloc>& Matrix<VT, _Alloc>::operator=(Matrix&& other)
{
    VT* temp = other.m_elem;
    other.m_elem = nullptr;

    destroy();

    m_row_size = other.m_row_size;
    m_col_size = other.m_col_size;
    other.m_row_size = 0;
    other.m_col_size = 0;
    m_elem = temp;

    return *this;
}

template <typename VT, typename _Alloc>
void Matrix<VT, _Alloc>::destroy() // DeleterForAllocator?
{
    for (size_type i = m_row_size * m_col_size; i-- > 0;) 
        m_elem[i].~VT();

    m_allocator.deallocate(m_elem, m_row_size * m_col_size);
}