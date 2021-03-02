#ifndef __TABLE_H__
#define __TABLE_H__

#include <cstddef>
#include <vector>
#include <iterator>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <tuple>
#include <numeric>
#include <tbb/parallel_sort.h>
#include <tbb/task_arena.h>
#include <cassert>

template<typename T>
class Table{
public:
    /**
     * Pointer-like container for one row of a table.
     * Implements access, comparison and output
    */
    class Row{
    public:
        Row(T* data_ptr, std::size_t row_size) : data_ptr_(data_ptr), row_size_(row_size){}
        T* begin() const { return data_ptr_; } 
        T* end() const { return data_ptr_+row_size_; }
        T& operator[](const std::size_t n) { return data_ptr_[n]; }
        std::size_t size() const {return row_size_; };

        friend void swap(Row x, Row y){
            #ifndef NDEBUG
            assert(x.row_size_ == y.row_size_);
            #endif

            if (x.data_ptr_ != y.data_ptr_){
                std::swap_ranges(x.data_ptr_, x.data_ptr_ + x.row_size_, y.data_ptr_);
            }
        }

        template<typename U>
        Row& operator=(const U& other){
            #ifndef NDEBUG
            assert(other.size() == row_size_);
            #endif

            std::copy(other.begin(), other.end(), data_ptr_);
            return *this;
        }

        friend bool operator<(const Row& x, const Row& y){
            return x.compare(y) < 0; 
        }

        friend bool operator<=(const Row& x, const Row& y){
            return x.compare(y) <= 0; 
        }

        friend bool operator>(const Row& x, const Row& y){
            return x.compare(y) > 0; 
        }

        friend bool operator>=(const Row& x, const Row& y){
            return x.compare(y) >= 0; 
        }

        friend bool operator==(const Row& x, const Row& y){
            return x.compare(y) == 0; 
        }

        friend std::ostream& operator<<(std::ostream& os, const Row& row){
            for(const auto& e : row) os << e << " ";
            return os;
        }

        int compare(const Row& other) const{
            #ifndef NDEBUG
            assert(row_size_ == other.row_size_);
            #endif

            int result = 0;
            for (std::size_t i = 0; i < row_size_; ++i){
                if (data_ptr_[i] < other.data_ptr_[i]){
                    result = -1;
                    break;
                }
                if (data_ptr_[i] > other.data_ptr_[i]){
                    result = 1;
                    break;
                }
            }
            return result;
        }

    private:
        T* data_ptr_;
        std::size_t row_size_;
    };
    
    /**
     * Iterator for rows that meets the requirements of ValueSwappable
     * and LegacyRandomAccessIterator. Handles only references to rows,
     * as rows do not support copying.
    */
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
        reference operator*( ) {return Row(m_, row_size_); }

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

    Table() : row_size_(0), n_rows_(0) {};
    Table(std::size_t row_size, std::size_t n_rows=0) 
    : row_size_(row_size), n_rows_(n_rows) {
        data_.resize(n_rows_ * row_size_);
    };
    RowIterator begin() { return RowIterator(&(data_.front()), row_size_); }
    RowIterator end() { return RowIterator(&(data_.back())+1, row_size_); }
    Row at(const std::size_t n) { 
        if (n >= n_rows_){
            throw std::runtime_error(std::string("Wrong row index: ")
                                    + std::to_string(n) + " >= " + std::to_string(n_rows_));
        }
        return Row(&(data_[n * row_size_]), row_size_);
    }
    Row operator[](const std::size_t n) {
        #ifndef NDEBUG
        assert(n < n_rows_);
        #endif
        return Row(&(data_[n * row_size_]), row_size_); 
    }
    std::size_t size() {return n_rows_ * row_size_; };
    std::tuple<std::size_t, std::size_t> shape() {return std::make_tuple(n_rows_, row_size_); }

    template<typename U>
    void push_back(const U& row){
        #ifndef NDEBUG
        assert(row.size() == row_size_);
        #endif

        data_.insert(data_.end(), row.begin(), row.end());
        n_rows_++;
    }

    template<typename U>
    void parallel_permute(U& positions, int n_threads=tbb::task_arena::automatic){
        #ifndef NDEBUG
        assert(positions.size() == n_rows_);
        #endif
        
        // First of all, we find cycles
        std::size_t n = positions.size();
        // The array containing cycles
        std::vector<std::size_t> cycles(n);
        // The positions of cycles starts
        std::vector<std::size_t> offsets = {0};
        // Flags for checked positions
        std::vector<bool> is_checked(n);
        // Offset iterator
        std::size_t it = 0;
        // Position iterator
        std::size_t i = 0;

        while (i < n){
            if (is_checked[i]){
                // If the cycle contains only 1 element, ignore it
                if (it == offsets.back() + 1){
                    it--;
                } else{
                    offsets.emplace_back(it);
                }
                for (; i < n && is_checked[i]; i++);
            } else{
                cycles[it] = i;
                is_checked[i] = true;
                i = positions[i];
                it++;
            }
        }

        // Then we do permutations for different cycles in parallel
        tbb::task_arena arena(n_threads);
        arena.execute([&]{
            tbb::parallel_for(tbb::blocked_range<size_t>(0, offsets.size()-1), 
                [&](const tbb::blocked_range<size_t>& r){
                    for(size_t i=r.begin(); i!=r.end(); ++i){
                        std::size_t from = offsets[i], to = offsets[i+1];
                        for (size_t k=from+1; k < to; ++k){
                            swap((*this)[cycles[from]], (*this)[cycles[k]]);
                        }
                    }
                });
        });
    }

    template<typename U>
    void permute(U& positions){
        #ifndef NDEBUG
        assert(positions.size() == n_rows_);
        #endif
        
        std::size_t pos = 0;
        while (pos < n_rows_){
            std::size_t new_pos = positions[pos];
            if (new_pos != pos){
                swap((*this)[pos], (*this)[new_pos]);
                std::swap(positions[pos], positions[new_pos]);
            } else{
                pos++;
            }
        }
    }

    void sort(){
        std::vector<std::size_t> pos(n_rows_);
        std::iota(pos.begin(), pos.end(), 0);

        std::sort(pos.begin(), pos.end(), [&](std::size_t a, std::size_t b) {
            return (*this)[a] < (*this)[b];
        });
        std::vector<size_t> perm(pos.size());
        for (std::size_t i = 0; i < perm.size(); ++i) perm[pos[i]] = i;

        permute(perm);
    }

    void parallel_sort(int n_threads=tbb::task_arena::automatic){
        std::vector<std::size_t> pos(n_rows_);
        std::iota(pos.begin(), pos.end(), 0);

        tbb::task_arena arena(n_threads);
        arena.execute([&]{
            tbb::parallel_sort(pos.begin(), pos.end(), [&](std::size_t a, std::size_t b) {
                return (*this)[a] < (*this)[b];
            });
        });
        std::vector<size_t> perm(pos.size());
        for (std::size_t i = 0; i < perm.size(); ++i) perm[pos[i]] = i;

        parallel_permute(perm, n_threads);
    }

    void reserve(std::size_t new_cap) {
        data_.reserve(new_cap * row_size_);
    }

    void resize(std::size_t n) {
        data_.resize(n * row_size_);
        n_rows_ = n;
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
    // Number of elements per row
    std::size_t row_size_;
    // Number of rows
    std::size_t n_rows_;
    std::vector<T> data_;
};

#endif // __TABLE_H__