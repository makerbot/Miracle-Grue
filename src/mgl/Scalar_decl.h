/* 
 * File:   Scalar_decl.h
 * Author: Dev
 *
 * Created on September 21, 2012, 1:50 PM
 */

#ifndef MGL_SCALAR_DECL_H
#define	MGL_SCALAR_DECL_H

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


#endif	/* SCALAR_DECL_H */

