#include "table.hpp"

template<typename T>
Table<T>::Table(std::size_t n_rows, std::size_t n_cols):
n_rows_{n_rows},
n_cols_{n_cols}
{
    data_.reserve(n_rows_ * n_cols_);
}
