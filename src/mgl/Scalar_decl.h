/* 
 * File:   Scalar_decl.h
 * Author: Dev
 *
 * Created on September 21, 2012, 1:50 PM
 */

#ifndef MGL_SCALAR_DECL_H
#define	MGL_SCALAR_DECL_H

namespace mgl {

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
    static const value_type FACTOR2_PRODUCT = FACTOR_PRODUCT << 1;
    
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
    operator float() const;
    operator double() const;
private:
    value_type value;
};

template <int F, typename VT>
basic_scalar<F, VT> operator+(const basic_scalar<F, VT>& lhs, const basic_scalar<F, VT>& rhs);
template <int F, typename VT>
basic_scalar<F, VT> operator-(const basic_scalar<F, VT>& lhs, const basic_scalar<F, VT>& rhs);
template <int F, typename VT>
basic_scalar<F, VT> operator*(const basic_scalar<F, VT>& lhs, const basic_scalar<F, VT>& rhs);
template <int F, typename VT>
basic_scalar<F, VT> operator/(const basic_scalar<F, VT>& lhs, const basic_scalar<F, VT>& rhs);

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

}


#endif	/* SCALAR_DECL_H */

