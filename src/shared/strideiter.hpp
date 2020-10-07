#ifndef STRIDEITER_HPP
#define STRIDEITER_HPP

#include <iterator>

template<typename T>
class StrideIter
{
public:
    // public typedefs
    typedef typename std::iterator_traits<T>::value_type value_type;
    typedef typename std::iterator_traits<T>::reference reference;
    typedef typename std::iterator_traits<T>::difference_type difference_type;
    typedef typename std::iterator_traits<T>::pointer pointer;
    typedef std::random_access_iterator_tag iterator_category;
    typedef StrideIter self;

    // constructors
    StrideIter() : m_(NULL), step_(0) {};
    StrideIter(const self& x) : m_(x.m_), step_(x.step_) {};
    StrideIter(T x, difference_type n) : m_(x), step_(n) {};

    // operators
    self& operator++( ) { m_ += step_; return *this; }
    self operator++(int) { self tmp = *this; m_ += step_; return tmp; }
    self& operator+=(const difference_type x) { m_ += (x * step_); return *this; }
    self& operator--( ) { m_ -= step_; return *this; }
    self operator--(int) { self tmp = *this; m_ -= step_; return tmp; }
    self& operator-=(const difference_type x) { m_ -= x * step_; return *this; }
    reference operator[](const difference_type n) { return m_[n * step_]; }
    reference operator*( ) { return *m_; }

    // friend operators
    friend bool operator==(const self& x, const self& y) {
        return x.m_ == y.m_;
    }
    friend bool operator!=(const self& x, const self& y) {
        return x.m_ != y.m_;
    }
    friend bool operator<(const self& x, const self& y) {
        return x.m_ < y.m_;
    }
    friend difference_type operator-(const self& x, const self& y) {
        return (x.m_ - y.m_) / x.step_;
    }
    friend self operator+(const self& x, difference_type y) {
        return self(x.m_ + (y * x.step_), x.step_);
    }
    friend self operator+(difference_type x, const self& y) {
        return y+x;
    }
    friend self operator-(const self& x, difference_type y) {
        return self(x.m_ - (y * x.step_), x.step_);
    }
private:
  T m_;
  difference_type step_;
};

#endif // __STRIDEITER_H__