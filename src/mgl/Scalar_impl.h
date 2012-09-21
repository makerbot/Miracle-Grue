/* 
 * File:   Scalar_impl.h
 * Author: Dev
 *
 * Created on September 21, 2012, 1:50 PM
 */

#ifndef MGL_SCALAR_IMPL_H
#define	MGL_SCALAR_IMPL_H

#include "Scalar_decl.h"

namespace mgl {

template <int F, typename VT> template <int OF, typename OVT>
Scalar<F, VT>::Scalar(const Scalar<OF, OVT>& other) 
        : value(other.Scalar<OF, OVT>::value << 
        (FACTOR - Scalar<OF, OVT>::FACTOR)) {}
template <int F, typename VT> template <int OF, typename OVT>
Scalar<F, VT>& Scalar<F, VT>::operator =(const Scalar<OF,OVT>& other) {
    value = other.Scalar<OF, OVT>::value << 
            (FACTOR - Scalar<OF, OVT>::FACTOR);
    return *this;
}
template <int F, typename VT> template <typename T>
Scalar<F, VT>::Scalar(const T& other)
        : value(static_cast<value_type>(other * FACTOR_PRODUCT)) {}
template <int F, typename VT> template <typename T>
Scalar<F, VT>& Scalar<F, VT>::operator =(const T& other) {
    value = static_cast<value_type>(other * FACTOR_PRODUCT);
    return *this;
}
template <int F, typename VT>
Scalar<F, VT>& Scalar<F, VT>::operator +=(const Scalar& other) {
    value += other.value;
    return *this;
}
template <int F, typename VT>
Scalar<F, VT>& Scalar<F, VT>::operator *=(const Scalar& other) {
    (value *= other.value) >>= FACTOR;
    return *this;
}
template <int F, typename VT>
Scalar<F, VT>& Scalar<F, VT>::operator /=(const Scalar& other) {
    (value <<= FACTOR) /= other.value;
    return *this;
}
template <int F, typename VT>
Scalar<F, VT> Scalar<F, VT>::operator -() const {
    Scalar ret;
    ret.value = -value;
    return ret;
}

//operator overloads
template <int F, typename VT>
Scalar<F, VT> operator+(const Scalar<F, VT>& lhs, const Scalar<F, VT>& rhs){
    Scalar<F, VT> ret = lhs;
    return ret += rhs;
}
template <int F, typename VT>
Scalar<F, VT> operator-(const Scalar<F, VT>& lhs, const Scalar<F, VT>& rhs){
    Scalar<F, VT> ret = lhs;
    return ret -= rhs;
}
template <int F, typename VT>
Scalar<F, VT> operator*(const Scalar<F, VT>& lhs, const Scalar<F, VT>& rhs){
    Scalar<F, VT> ret = lhs;
    return ret *= rhs;
}
template <int F, typename VT>
Scalar<F, VT> operator/(const Scalar<F, VT>& lhs, const Scalar<F, VT>& rhs){
    Scalar<F, VT> ret = lhs;
    return ret /= rhs;
}
template <int F, typename VT>
bool operator<(const Scalar<F, VT>& lhs, const Scalar<F, VT>& rhs) {
    return lhs.value_type < rhs.value_type;
}
template <int F, typename VT>
bool operator>(const Scalar<F, VT>& lhs, const Scalar<F, VT>& rhs){
    return rhs < lhs;
}
template <int F, typename VT>
bool operator<=(const Scalar<F, VT>& lhs, const Scalar<F, VT>& rhs){
    return !(lhs>rhs);
}
template <int F, typename VT>
bool operator>=(const Scalar<F, VT>& lhs, const Scalar<F, VT>& rhs){
    return !(rhs<lhs);
}
template <int F, typename VT>
bool operator==(const Scalar<F, VT>& lhs, const Scalar<F, VT>& rhs) {
    return lhs.value_type == rhs.value_type;
}
template <int F, typename VT>
bool operator!=(const Scalar<F, VT>& lhs, const Scalar<F, VT>& rhs) {
    return !(lhs == rhs);
}

}


#endif	/* SCALAR_IMPL_H */

