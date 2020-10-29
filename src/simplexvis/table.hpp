#ifndef __TABLE_H__
#define __TABLE_H__

#include <cstddef>
#include <vector>
#include <iterator>
#include <stdexcept>
#include <iostream>
#include <tuple>
#include <algorithm>

template<typename T>
class Table{
public:
    class Row{
    public:
        template<typename U>
        Row(const Row& other) : row_size_(other.size()) {
            std::vector<T> data(other.begin(), other.end());
            data_ptr_ = &(data[0]);
            std::cout << "Row(const Row& other) " << other << std::endl;
        }
        Row(T* data_ptr, std::size_t row_size) : data_ptr_(data_ptr), row_size_(row_size){
             std::cout << "Row(T* data_ptr, std::size_t row_size) " << data_ptr << std::endl;
        }
        T* begin() const { return data_ptr_; } 
        T* end() const { return data_ptr_+row_size_; }
        T& operator[](const std::size_t n) { return data_ptr_[n]; }
        std::size_t size() {return row_size_; };

        friend void swap(Row x, Row y){
            std::cout << "swap " << x.data_ptr_ << " <> " << y.data_ptr_ << std::endl;
            if (x.row_size_ != y.row_size_){
                throw std::runtime_error(std::string("Can't swap Rows of different size: ")
                                        + std::to_string(x.row_size_) + " != " + std::to_string(y.row_size_));
            }
            std::swap_ranges(x.data_ptr_, x.data_ptr_ + x.row_size_, y.data_ptr_);
        }

        template<typename U>
        Row& operator=(const U& other){
            std::cout << "Row& operator=(const U& other)" << std::endl;
            
            if (other.size() != row_size_){
                throw std::runtime_error(std::string("Can't assign to iterable of different size: ")
                                        + std::to_string(row_size_) + " != " + std::to_string(other.size()));
            }
            std::copy(other.begin(), other.end(), data_ptr_);
            return *this;
        }

        friend bool operator<(const Row& x, const Row& y) {
            std::cout << "compare " << x << "< " << y << "? ";
            if (x.row_size_ != y.row_size_){
                throw std::runtime_error(std::string("Can't compare Rows of different size: ")
                                        + std::to_string(x.row_size_) + " != " + std::to_string(y.row_size_));
            }
            bool less = true;
            for (std::size_t i = 0; i < x.row_size_; ++i){
                if (x.data_ptr_[i] >= y.data_ptr_[i]){
                    less = false;
                    break;
                }
            }
            std::cout << less << std::endl;
            return less;
        }

        friend std::ostream& operator<<(std::ostream& os, const Row& row){
            for(const auto& e : row) os << e << " ";
            return os;
        }
    private:
        T* data_ptr_;
        std::size_t row_size_;
    };
    
    class RowIterator
    {
    public:
        typedef Row value_type;
        typedef Row reference;
        typedef typename std::iterator_traits<T*>::difference_type difference_type;
        typedef typename std::iterator_traits<T*>::pointer pointer;
        typedef std::random_access_iterator_tag iterator_category;

        // constructors
        RowIterator( ) : m_(NULL), row_size_(0) {};
        RowIterator(const RowIterator& x) : m_(x.m_), row_size_(x.row_size_) {};
        RowIterator(T* x, difference_type n) : m_(x), row_size_(n) {};

        // operators
        RowIterator& operator++( ) { m_ += row_size_; return *this; }
        RowIterator operator++(int) { RowIterator tmp = *this; m_ += row_size_; return tmp; }
        RowIterator& operator+=(const difference_type x) { m_ += (x * row_size_); return *this; }
        RowIterator& operator--( ) { m_ -= row_size_; return *this; }
        RowIterator operator--(int) { RowIterator tmp = *this; m_ -= row_size_; return tmp; }
        RowIterator& operator-=(const difference_type x) { m_ -= x * row_size_; return *this; }
        reference operator[](const difference_type n) { return Row(m_ + n * row_size_, row_size_); }
        reference operator*( ) { std::cout << "operator* " << m_ << std::endl; return Row(m_, row_size_); }

        // friend operators
        friend bool operator==(const RowIterator& x, const RowIterator& y) {
            return x.m_ == y.m_;
        }
        friend bool operator!=(const RowIterator& x, const RowIterator& y) {
            return x.m_ != y.m_;
        }
        friend bool operator<(const RowIterator& x, const RowIterator& y) {
            return x.m_ < y.m_;
        }
        friend difference_type operator-(const RowIterator& x, const RowIterator& y) {
            return (x.m_ - y.m_) / x.row_size_;
        }
        friend RowIterator operator+(const RowIterator& x, difference_type y) {
            return RowIterator(x.m_ + (y * x.row_size_), x.row_size_);
        }
        friend RowIterator operator+(difference_type x, const RowIterator& y) {
            return y+x;
        }
        friend RowIterator operator-(const RowIterator& x, difference_type y) {
            return RowIterator(x.m_ - (y * x.row_size_), x.row_size_);
        }
    private:
        T* m_;
        difference_type row_size_;
    };

    Table() = default;
    Table(std::size_t row_size, std::size_t n_rows=0) : row_size_(row_size), n_rows_(n_rows){
        data_.resize(n_rows_ * row_size_);
    };
    RowIterator begin() { return RowIterator(&(data_.front()), row_size_); }
    RowIterator end() { return RowIterator(&(data_.back())+1, row_size_); }
    Row operator[](const std::size_t n) { return Row(&(data_[n * row_size_]), row_size_); }
    std::size_t size() {return n_rows_ * row_size_; };
    std::tuple<std::size_t, std::size_t> shape() {return std::make_tuple(n_rows_, row_size_); }

    void reserve(std::size_t new_cap) {
        data_.reserve(new_cap * row_size_);
    }

    void resize(std::size_t n) {
        n_rows_ = n;
        data_.resize(n * row_size_);
    }

    friend std::ostream& operator<<(std::ostream& os, const Table& table){
        for(std::size_t i = 0; i < table.n_rows_; ++i){
            os << "[" << i << "] ";
            for (std::size_t j = 0; j < table.row_size_; ++j){
                os << table.data_.at(i*table.row_size_ + j) << " ";
            }
            os << std::endl;
        }
        return os;
    }
private:
    std::size_t row_size_;
    std::size_t n_rows_;
    std::vector<T> data_;
};

#endif // __TABLE_H__