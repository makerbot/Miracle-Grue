/* 
 * File:   basic_scalar_impl.h
 * Author: Dev
 *
 * Created on September 21, 2012, 1:50 PM
 */

#ifndef MGL_SCALAR_IMPL_H
#define	MGL_SCALAR_IMPL_H

#include "Scalar_decl.h"

namespace mgl {

template <int F, typename VT> template <int OF, typename OVT>
basic_scalar<F, VT>::basic_scalar(const basic_scalar<OF, OVT>& other) 
        : value(other.basic_scalar<OF, OVT>::value << 
        (FACTOR - basic_scalar<OF, OVT>::FACTOR)) {}
template <int F, typename VT> template <int OF, typename OVT>
basic_scalar<F, VT>& basic_scalar<F, VT>::operator =(const basic_scalar<OF,OVT>& other) {
    value = other.basic_scalar<OF, OVT>::value << 
            (FACTOR - basic_scalar<OF, OVT>::FACTOR);
    return *this;
}
template <int F, typename VT> template <typename T>
basic_scalar<F, VT>::basic_scalar(const T& other)
        : value(static_cast<value_type>(other * FACTOR_PRODUCT)) {}
template <int F, typename VT> template <typename T>
basic_scalar<F, VT>& basic_scalar<F, VT>::operator =(const T& other) {
    value = static_cast<value_type>(other * FACTOR_PRODUCT);
    return *this;
}
template <int F, typename VT>
basic_scalar<F, VT>& basic_scalar<F, VT>::operator +=(const basic_scalar& other) {
    value += other.value;
    return *this;
}
template <int F, typename VT>
basic_scalar<F, VT>& basic_scalar<F, VT>::operator *=(const basic_scalar& other) {
    (value *= other.value) >>= FACTOR;
    return *this;
}
template <int F, typename VT>
basic_scalar<F, VT>& basic_scalar<F, VT>::operator /=(const basic_scalar& other) {
    (value <<= FACTOR) /= other.value;
    return *this;
}
template <int F, typename VT>
basic_scalar<F, VT> basic_scalar<F, VT>::operator -() const {
    basic_scalar ret;
    ret.value = -value;
    return ret;
}
template <int F, typename VT>
basic_scalar<F, VT>::operator float() const{
    return float(value)*FACTOR_PRODUCT;
}
template <int F, typename VT>
basic_scalar<F, VT>::operator double() const{
    return double(value)*FACTOR_PRODUCT;
}



//operator overloads
template <int F, typename VT>
basic_scalar<F, VT> operator+(const basic_scalar<F, VT>& lhs, const basic_scalar<F, VT>& rhs){
    basic_scalar<F, VT> ret = lhs;
    return ret += rhs;
}
template <int F, typename VT>
basic_scalar<F, VT> operator-(const basic_scalar<F, VT>& lhs, const basic_scalar<F, VT>& rhs){
    basic_scalar<F, VT> ret = lhs;
    return ret -= rhs;
}
template <int F, typename VT>
basic_scalar<F, VT> operator*(const basic_scalar<F, VT>& lhs, const basic_scalar<F, VT>& rhs){
    basic_scalar<F, VT> ret = lhs;
    return ret *= rhs;
}
template <int F, typename VT>
basic_scalar<F, VT> operator/(const basic_scalar<F, VT>& lhs, const basic_scalar<F, VT>& rhs){
    basic_scalar<F, VT> ret = lhs;
    return ret /= rhs;
}
template <int F, typename VT>
bool operator<(const basic_scalar<F, VT>& lhs, const basic_scalar<F, VT>& rhs) {
    return lhs.value_type < rhs.value_type;
}
template <int F, typename VT>
bool operator>(const basic_scalar<F, VT>& lhs, const basic_scalar<F, VT>& rhs){
    return rhs < lhs;
}
template <int F, typename VT>
bool operator<=(const basic_scalar<F, VT>& lhs, const basic_scalar<F, VT>& rhs){
    return !(lhs>rhs);
}
template <int F, typename VT>
bool operator>=(const basic_scalar<F, VT>& lhs, const basic_scalar<F, VT>& rhs){
    return !(rhs<lhs);
}
template <int F, typename VT>
bool operator==(const basic_scalar<F, VT>& lhs, const basic_scalar<F, VT>& rhs) {
    return lhs.value_type == rhs.value_type;
}
template <int F, typename VT>
bool operator!=(const basic_scalar<F, VT>& lhs, const basic_scalar<F, VT>& rhs) {
    return !(lhs == rhs);
}

}


#endif	/* SCALAR_IMPL_H */

