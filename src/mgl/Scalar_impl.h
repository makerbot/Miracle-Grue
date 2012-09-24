/* 
 * File:   basic_scalar_impl.h
 * Author: Dev
 *
 * Created on September 21, 2012, 1:50 PM
 */

#ifndef MGL_SCALAR_IMPL_H
#define	MGL_SCALAR_IMPL_H

#include "Scalar_decl.h"
#include <cmath>

namespace mgl {


template <int F, typename VT>
basic_scalar<F, VT>::basic_scalar() 
        : value(0) {}
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
        : value(static_cast<value_type>(other) * FACTOR_PRODUCT) {}
template <int F, typename VT> template <typename T>
basic_scalar<F, VT>& basic_scalar<F, VT>::operator =(const T& other) {
    value = static_cast<value_type>(other) * FACTOR_PRODUCT;
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
typename basic_scalar<F, VT>::math_type basic_scalar<F, VT>::convertToMath() const{
    math_type result;
    convertToMath(result);
    return result;
}
template <int F, typename VT>
void basic_scalar<F, VT>::convertToMath(typename basic_scalar<F, VT>::math_type& result) const{
    result = static_cast<math_type>(value) / FACTOR_PRODUCT;
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

template <int F, typename VT>
basic_scalar<F, VT> abs(const basic_scalar<F, VT>& arg) {
    return arg < 0 ? -arg : arg;
}
//no fancy int tricks. We go to double and back
template <int F, typename VT>
basic_scalar<F, VT> sqrt(const basic_scalar<F, VT>& arg) {
    return basic_scalar<F, VT>(::sqrt(arg.convertToMath()));
}
template <int F, typename VT>
basic_scalar<F, VT> sin(const basic_scalar<F, VT>& arg){
    return basic_scalar<F, VT>(::sin(arg.convertToMath()));
}
template <int F, typename VT>
basic_scalar<F, VT> cos(const basic_scalar<F, VT>& arg){
    return basic_scalar<F, VT>(::cos(arg.convertToMath()));
}
template <int F, typename VT>
basic_scalar<F, VT> asin(const basic_scalar<F, VT>& arg){
    return basic_scalar<F, VT>(::asin(arg.convertToMath()));
}
template <int F, typename VT>
basic_scalar<F, VT> acos(const basic_scalar<F, VT>& arg){
    return basic_scalar<F, VT>(::acos(arg.convertToMath()));
}

}


#endif	/* SCALAR_IMPL_H */

