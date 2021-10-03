#ifndef MATRIX_H
#define MATRIX_H

#include <vector>

template <typename T>
using mat = std::vector<std::vector<T>>;

template <typename T>
class matrix :public mat<T>
{
public:
    using size_type = typename mat<T>::size_type;


    matrix() {};
    matrix(size_type row, size_type col, T def = T())
        :mat<T>(row, std::vector<T>(col, def)), row_size{row}, col_size{col} {}

    inline void reset(size_type row, size_type col, T def = T())
    { *this = matrix(row, col, def); }

    inline int row() const {
        return row_size;
    }
    inline int col() const {
        return col_size;
    }

private:
    size_type row_size = 0;
    size_type col_size = 0;
};

#endif // MATRIX_H
