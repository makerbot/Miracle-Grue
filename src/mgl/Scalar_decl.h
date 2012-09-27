/* 
 * File:   Scalar_decl.h
 * Author: Dev
 *
 * Created on September 21, 2012, 1:50 PM
 */

#ifndef MGL_SCALAR_DECL_H
#define	MGL_SCALAR_DECL_H

#include <limits>

namespace mgl {

typedef double basic_scalar_internal_type;

template <int F, typename VT>
class basic_scalar;

template <int F, typename VT>
bool operator<(const basic_scalar<F, VT>& lhs, const basic_scalar<F, VT>& rhs);
template <int F, typename VT>
bool operator==(const basic_scalar<F, VT>& lhs, const basic_scalar<F, VT>& rhs);

template <int F, typename VT = long int>
class basic_scalar{
public:
    
    template <int OF, typename OVT>
    friend class basic_scalar;
    
    friend bool operator < <>(const basic_scalar& lhs, const basic_scalar& rhs);
    friend bool operator == <>(const basic_scalar& lhs, const basic_scalar& rhs);
    
    typedef VT value_type;
    static const int FACTOR = F;
    static const int NFACTOR = sizeof(value_type) - FACTOR;
    static const value_type FACTOR_PRODUCT = 1 << FACTOR;
    
    typedef double math_type;   //what we use for doing cmath functions
    
    basic_scalar();
    template <typename T>
    basic_scalar(const T& other);
    template <typename T>
    basic_scalar& operator=(const T& other);
    template <int OF, typename OVT>
    basic_scalar(const basic_scalar<OF, OVT>& other);
    template <int OF, typename OVT>
    basic_scalar& operator=(const basic_scalar<OF, OVT>& other);
    basic_scalar& operator+=(const basic_scalar& other);
    basic_scalar& operator*=(const basic_scalar& other);
    basic_scalar& operator/=(const basic_scalar& other);
    basic_scalar operator-() const;
    
    math_type convertToMath() const;
    void convertToMath(math_type& result) const;
    value_type convertToValue() const;
    void convertToValue(value_type& result) const;
    
    basic_scalar ipart() const;
    basic_scalar fpart() const;
    
    value_type& underlyingValue();
    const value_type& underlyingValue() const;
    
private:
    value_type value;
};

template <int F, typename VT>
basic_scalar<F, VT>& operator-=(basic_scalar<F, VT>& lhs, const basic_scalar<F, VT>& rhs);
template <int F, typename VT>
basic_scalar<F, VT> operator+(const basic_scalar<F, VT>& lhs, const basic_scalar<F, VT>& rhs);
template <int F, typename VT>
basic_scalar<F, VT> operator-(const basic_scalar<F, VT>& lhs, const basic_scalar<F, VT>& rhs);
template <int F, typename VT>
basic_scalar<F, VT> operator*(const basic_scalar<F, VT>& lhs, const basic_scalar<F, VT>& rhs);
template <int F, typename VT>
basic_scalar<F, VT> operator/(const basic_scalar<F, VT>& lhs, const basic_scalar<F, VT>& rhs);

template <int F, typename VT, typename T>
basic_scalar<F, VT>& operator+=(basic_scalar<F, VT>& lhs, const T& rhs);
template <int F, typename VT, typename T>
basic_scalar<F, VT>& operator-=(basic_scalar<F, VT>& lhs, const T& rhs);
template <int F, typename VT, typename T>
basic_scalar<F, VT>& operator*=(basic_scalar<F, VT>& lhs, const T& rhs);
template <int F, typename VT, typename T>
basic_scalar<F, VT>& operator/=(basic_scalar<F, VT>& lhs, const T& rhs);
template <int F, typename VT, typename T>
basic_scalar<F, VT> operator+(const basic_scalar<F, VT>& lhs, const T& rhs);
template <int F, typename VT, typename T>
basic_scalar<F, VT> operator-(const basic_scalar<F, VT>& lhs, const T& rhs);
template <int F, typename VT, typename T>
basic_scalar<F, VT> operator*(const basic_scalar<F, VT>& lhs, const T& rhs);
template <int F, typename VT, typename T>
basic_scalar<F, VT> operator/(const basic_scalar<F, VT>& lhs, const T& rhs);
template <int F, typename VT, typename T>
basic_scalar<F, VT> operator+(const T& lhs, const basic_scalar<F, VT>& rhs);
template <int F, typename VT, typename T>
basic_scalar<F, VT> operator-(const T& lhs, const basic_scalar<F, VT>& rhs);
template <int F, typename VT, typename T>
basic_scalar<F, VT> operator*(const T& lhs, const basic_scalar<F, VT>& rhs);
template <int F, typename VT, typename T>
basic_scalar<F, VT> operator/(const T& lhs, const basic_scalar<F, VT>& rhs);

template <int F, typename VT>
bool operator<(const basic_scalar<F, VT>& lhs, const basic_scalar<F, VT>& rhs);
template <int F, typename VT>
bool operator>(const basic_scalar<F, VT>& lhs, const basic_scalar<F, VT>& rhs);
template <int F, typename VT>
bool operator<=(const basic_scalar<F, VT>& lhs, const basic_scalar<F, VT>& rhs);
template <int F, typename VT>
bool operator>=(const basic_scalar<F, VT>& lhs, const basic_scalar<F, VT>& rhs);
template <int F, typename VT>
bool operator==(const basic_scalar<F, VT>& lhs, const basic_scalar<F, VT>& rhs);
template <int F, typename VT>
bool operator!=(const basic_scalar<F, VT>& lhs, const basic_scalar<F, VT>& rhs);

template <int F, typename VT, typename T>
bool operator<(const basic_scalar<F, VT>& lhs, const T& rhs);
template <int F, typename VT, typename T>
bool operator>(const basic_scalar<F, VT>& lhs, const T& rhs);
template <int F, typename VT, typename T>
bool operator<=(const basic_scalar<F, VT>& lhs, const T& rhs);
template <int F, typename VT, typename T>
bool operator>=(const basic_scalar<F, VT>& lhs, const T& rhs);
template <int F, typename VT, typename T>
bool operator==(const basic_scalar<F, VT>& lhs, const T& rhs);
template <int F, typename VT, typename T>
bool operator!=(const basic_scalar<F, VT>& lhs, const T& rhs);
template <int F, typename VT, typename T>
bool operator<(const T& lhs, const basic_scalar<F, VT>& rhs);
template <int F, typename VT, typename T>
bool operator>(const T& lhs, const basic_scalar<F, VT>& rhs);
template <int F, typename VT, typename T>
bool operator<=(const T& lhs, const basic_scalar<F, VT>& rhs);
template <int F, typename VT, typename T>
bool operator>=(const T& lhs, const basic_scalar<F, VT>& rhs);
template <int F, typename VT, typename T>
bool operator==(const T& lhs, const basic_scalar<F, VT>& rhs);
template <int F, typename VT, typename T>
bool operator!=(const T& lhs, const basic_scalar<F, VT>& rhs);

template <int F, typename VT>
basic_scalar<F, VT> abs(const basic_scalar<F, VT>& arg);
template <int F, typename VT>
basic_scalar<F, VT> floor(const basic_scalar<F, VT>& arg);
template <int F, typename VT>
basic_scalar<F, VT> ceil(const basic_scalar<F, VT>& arg);
template <int F, typename VT>
basic_scalar<F, VT> round(const basic_scalar<F, VT>& arg);
template <int F, typename VT>
basic_scalar<F, VT> sqrt(const basic_scalar<F, VT>& arg);
template <int F, typename VT>
basic_scalar<F, VT> sin(const basic_scalar<F, VT>& arg);
template <int F, typename VT>
basic_scalar<F, VT> cos(const basic_scalar<F, VT>& arg);
template <int F, typename VT>
basic_scalar<F, VT> asin(const basic_scalar<F, VT>& arg);
template <int F, typename VT>
basic_scalar<F, VT> acos(const basic_scalar<F, VT>& arg);


template <int F, typename VT, typename LHT>
LHT& operator<<(LHT& lhs, const basic_scalar<F, VT>& rhs);
template <int F, typename VT, typename LHT>
LHT& operator>>(LHT& lhs, basic_scalar<F, VT>& rhs);

}

//limits
namespace std {

template <int F, typename VT>
struct numeric_limits<mgl::basic_scalar<F, VT> > {
    
    typedef mgl::basic_scalar<F, VT> numeric_type;
    typedef typename numeric_type::value_type value_type;
    
    static const bool is_specialized = true;

    static numeric_type min() throw () {
        numeric_type val;
        val.underlyingValue() = numeric_limits<value_type>::min();
        return val;
    }

    static numeric_type max() throw () {
        numeric_type val;
        val.underlyingValue() = numeric_limits<value_type>::max();
        return val;
    }

    static const int digits = numeric_limits<value_type>::digits;
    static const int digits10 = numeric_limits<value_type>::digits10;
    static const bool is_signed = numeric_limits<value_type>::is_signed;
    static const bool is_integer = numeric_limits<value_type>::is_integer;
    static const bool is_exact = numeric_limits<value_type>::is_exact;
    static const int radix = numeric_limits<value_type>::radix;

    static numeric_type epsilon() throw () {
        numeric_type val;
        val.underlyingValue() = 1;
        return val;
    }

    static numeric_type round_error() throw () {
        numeric_type val;
        val.underlyingValue() = numeric_limits<value_type>::round_error();
        return val;
    }

    static const int min_exponent = numeric_limits<value_type>::min_exponent;
    static const int min_exponent10 = numeric_limits<value_type>::min_exponent10;
    static const int max_exponent = numeric_limits<value_type>::max_exponent;
    static const int max_exponent10 = numeric_limits<value_type>::max_exponent10;

    static const bool has_infinity = numeric_limits<value_type>::has_infinity;
    static const bool has_quiet_NaN = numeric_limits<value_type>::has_quiet_NaN;
    static const bool has_signaling_NaN = numeric_limits<value_type>::has_signaling_NaN;
    static const float_denorm_style has_denorm = numeric_limits<value_type>::has_denorm;
    static const bool has_denorm_loss = numeric_limits<value_type>::has_denorm_loss;

    static numeric_type infinity() throw () {
        numeric_type val;
        val.underlyingValue() = numeric_limits<value_type>::infinity();
        return val;
    }

    static numeric_type quiet_NaN() throw () {
        numeric_type val;
        val.underlyingValue() = numeric_limits<value_type>::quiet_NaN();
        return val;
    }

    static numeric_type signaling_NaN() throw () {
        numeric_type val;
        val.underlyingValue() = numeric_limits<value_type>::signaling_NaN();
        return val;
    }

    static numeric_type denorm_min() throw () {
        numeric_type val;
        val.underlyingValue() = numeric_limits<value_type>::denorm_min();
        return val;
    }

    static const bool is_iec559 = numeric_limits<value_type>::is_iec559;
    static const bool is_bounded = numeric_limits<value_type>::is_bounded;
    static const bool is_modulo = numeric_limits<value_type>::is_modulo;

    static const bool traps = numeric_limits<value_type>::traps;
    static const bool tinyness_before = numeric_limits<value_type>::tinyness_before;
    static const float_round_style round_style = numeric_limits<value_type>::round_style;
};

}


#endif	/* SCALAR_DECL_H */

