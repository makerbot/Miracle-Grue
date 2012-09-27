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
#include <stdexcept>
#include <iostream>

namespace mgl {


template <int F, typename VT>
basic_scalar<F, VT>::basic_scalar() 
        : value(0) {}
template <int F, typename VT> template <int OF, typename OVT>
basic_scalar<F, VT>::basic_scalar(const basic_scalar<OF, OVT>& other) 
        : value(other.underlyingValue() << 
        (FACTOR - basic_scalar<OF, OVT>::FACTOR)) {}
template <int F, typename VT> template <int OF, typename OVT>
basic_scalar<F, VT>& basic_scalar<F, VT>::operator =(const basic_scalar<OF,OVT>& other) {
    value = other.underlyingValue() << 
            (FACTOR - basic_scalar<OF, OVT>::FACTOR);
    return *this;
}
template <int F, typename VT> template <typename T>
basic_scalar<F, VT>::basic_scalar(const T& other)
        : value(static_cast<value_type>(other * FACTOR_PRODUCT)) {
    std::cout 
            <<   "Input:    " << other 
            << "\nMax:      " << std::numeric_limits<basic_scalar<F, VT> >::max() 
            << "\nMin:      " << std::numeric_limits<basic_scalar<F, VT> >::min() 
            << "\nEpsilon:  " << std::numeric_limits<basic_scalar<F, VT> >::epsilon() 
            //<< "\nInputDbl: " << static_cast<double>(other) 
            << "\nOutput:   " << *this << std::endl;
    math_type error = static_cast<math_type>(underlyingValue()) - 
            other * FACTOR_PRODUCT;
    if(error < -1 || error > 1)
        throw std::logic_error("Fixed Point conversion error");
}
template <int F, typename VT> template <typename T>
basic_scalar<F, VT>& basic_scalar<F, VT>::operator =(const T& other) {
    return *this = basic_scalar<F, VT>(other);
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
template <int F, typename VT>
typename basic_scalar<F, VT>::value_type basic_scalar<F, VT>::convertToValue() const{
    value_type result;
    convertToValue(result);
    return result;
}
template <int F, typename VT>
void basic_scalar<F, VT>::convertToValue(typename basic_scalar<F, VT>::value_type& result) const{
    result = value >> FACTOR;
}
template <int F, typename VT>
basic_scalar<F, VT> basic_scalar<F, VT>::ipart() const {
    value_type retValue =  (value >> FACTOR) << FACTOR;
    basic_scalar ret;
    ret.value = retValue;
    return ret;
}
template <int F, typename VT>
basic_scalar<F, VT> basic_scalar<F, VT>::fpart() const {
    return *this - ipart();
}
template <int F, typename VT>
typename basic_scalar<F, VT>::value_type& basic_scalar<F, VT>::underlyingValue() {
    return value;
}
template <int F, typename VT>
const typename basic_scalar<F, VT>::value_type& basic_scalar<F, VT>::underlyingValue() const {
    return value;
}



//operator overloads
template <int F, typename VT>
basic_scalar<F, VT>& operator-=(basic_scalar<F, VT>& lhs, const basic_scalar<F, VT>& rhs){
    return lhs+=-rhs;
}
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

//boolean operations
template <int F, typename VT>
bool operator<(const basic_scalar<F, VT>& lhs, const basic_scalar<F, VT>& rhs) {
    return lhs.underlyingValue() < rhs.underlyingValue();
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
    return lhs.underlyingValue() == rhs.underlyingValue();
}
template <int F, typename VT>
bool operator!=(const basic_scalar<F, VT>& lhs, const basic_scalar<F, VT>& rhs) {
    return !(lhs == rhs);
}

//operators with other types
template <int F, typename VT, typename T>
basic_scalar<F, VT>& operator+=(basic_scalar<F, VT>& lhs, const T& rhs) {
    return lhs += basic_scalar<F, VT>(rhs);
}
template <int F, typename VT, typename T>
basic_scalar<F, VT>& operator-=(basic_scalar<F, VT>& lhs, const T& rhs) {
    return lhs -= basic_scalar<F, VT>(rhs);
}
template <int F, typename VT, typename T>
basic_scalar<F, VT>& operator*=(basic_scalar<F, VT>& lhs, const T& rhs) {
    return lhs *= basic_scalar<F, VT>(rhs);
}
template <int F, typename VT, typename T>
basic_scalar<F, VT>& operator/=(basic_scalar<F, VT>& lhs, const T& rhs) {
    return lhs /= basic_scalar<F, VT>(rhs);
}
template <int F, typename VT, typename T>
basic_scalar<F, VT> operator+(const basic_scalar<F, VT>& lhs, const T& rhs) {
    return lhs + basic_scalar<F, VT>(rhs);
}
template <int F, typename VT, typename T>
basic_scalar<F, VT> operator-(const basic_scalar<F, VT>& lhs, const T& rhs) {
    return lhs - basic_scalar<F, VT>(rhs);
}
template <int F, typename VT, typename T>
basic_scalar<F, VT> operator*(const basic_scalar<F, VT>& lhs, const T& rhs) {
    return lhs * basic_scalar<F, VT>(rhs);
}
template <int F, typename VT, typename T>
basic_scalar<F, VT> operator/(const basic_scalar<F, VT>& lhs, const T& rhs) {
    return lhs / basic_scalar<F, VT>(rhs);
}
template <int F, typename VT, typename T>
basic_scalar<F, VT> operator+(const T& lhs, const basic_scalar<F, VT>& rhs) {
    return basic_scalar<F, VT>(lhs) + rhs;
}
template <int F, typename VT, typename T>
basic_scalar<F, VT> operator-(const T& lhs, const basic_scalar<F, VT>& rhs) {
    return basic_scalar<F, VT>(lhs) - rhs;
}
template <int F, typename VT, typename T>
basic_scalar<F, VT> operator*(const T& lhs, const basic_scalar<F, VT>& rhs) {
    return basic_scalar<F, VT>(lhs) * rhs;
}
template <int F, typename VT, typename T>
basic_scalar<F, VT> operator/(const T& lhs, const basic_scalar<F, VT>& rhs) {
    return basic_scalar<F, VT>(lhs) / rhs;
}

//boolean operations with other types
template <int F, typename VT, typename T>
bool operator<(const basic_scalar<F, VT>& lhs, const T& rhs) {
    return lhs < basic_scalar<F, VT>(rhs);
}
template <int F, typename VT, typename T>
bool operator>(const basic_scalar<F, VT>& lhs, const T& rhs) {
    return lhs > basic_scalar<F, VT>(rhs);
}
template <int F, typename VT, typename T>
bool operator<=(const basic_scalar<F, VT>& lhs, const T& rhs) {
    return lhs <= basic_scalar<F, VT>(rhs);
}
template <int F, typename VT, typename T>
bool operator>=(const basic_scalar<F, VT>& lhs, const T& rhs) {
    return lhs >= basic_scalar<F, VT>(rhs);
}
template <int F, typename VT, typename T>
bool operator==(const basic_scalar<F, VT>& lhs, const T& rhs) {
    return lhs == basic_scalar<F, VT>(rhs);
}
template <int F, typename VT, typename T>
bool operator!=(const basic_scalar<F, VT>& lhs, const T& rhs) {
    return lhs != basic_scalar<F, VT>(rhs);
}
template <int F, typename VT, typename T>
bool operator<(const T& lhs, const basic_scalar<F, VT>& rhs) {
    return basic_scalar<F, VT>(lhs) < rhs;
}
template <int F, typename VT, typename T>
bool operator>(const T& lhs, const basic_scalar<F, VT>& rhs) {
    return basic_scalar<F, VT>(lhs) > rhs;
}
template <int F, typename VT, typename T>
bool operator<=(const T& lhs, const basic_scalar<F, VT>& rhs) {
    return basic_scalar<F, VT>(lhs) <= rhs;
}
template <int F, typename VT, typename T>
bool operator>=(const T& lhs, const basic_scalar<F, VT>& rhs) {
    return basic_scalar<F, VT>(lhs) >= rhs;
}
template <int F, typename VT, typename T>
bool operator==(const T& lhs, const basic_scalar<F, VT>& rhs) {
    return basic_scalar<F, VT>(lhs) == rhs;
}
template <int F, typename VT, typename T>
bool operator!=(const T& lhs, const basic_scalar<F, VT>& rhs) {
    return basic_scalar<F, VT>(lhs) != rhs;
}

//math function overloads
template <int F, typename VT>
basic_scalar<F, VT> abs(const basic_scalar<F, VT>& arg) {
    return arg < 0 ? -arg : arg;
}
template <int F, typename VT>
basic_scalar<F, VT> floor(const basic_scalar<F, VT>& arg) {
    return basic_scalar<F, VT>(arg.convertToValue());
}
template <int F, typename VT>
basic_scalar<F, VT> ceil(const basic_scalar<F, VT>& arg) {
    return floor(arg + (arg.fpart().underlyingValue() ? 1 : 0));
}
template <int F, typename VT>
basic_scalar<F, VT> round(const basic_scalar<F, VT>& arg) {
    return floor(arg + 0.5);
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

template <int F, typename VT, typename LHT>
LHT& operator<<(LHT& lhs, const basic_scalar<F, VT>& rhs) {
    return lhs << rhs.convertToMath();
}
template <int F, typename VT, typename LHT>
LHT& operator>>(LHT& lhs, basic_scalar<F, VT>& rhs) {
    typedef typename basic_scalar<F, VT>::math_type math_type;
    math_type intermediate;
    lhs >> intermediate;
    rhs = intermediate;
    return lhs;
}

}


#endif	/* SCALAR_IMPL_H */

