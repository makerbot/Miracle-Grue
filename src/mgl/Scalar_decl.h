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
class Scalar;

template <int F, typename VT>
bool operator<(const Scalar<F, VT>& lhs, const Scalar<F, VT>& rhs);
template <int F, typename VT>
bool operator==(const Scalar<F, VT>& lhs, const Scalar<F, VT>& rhs);

template <int F, typename VT = long int>
class Scalar{
public:
    
    template <int OF, typename OVT>
    friend class Scalar;
    
    friend bool operator < <>(const Scalar& lhs, const Scalar& rhs);
    friend bool operator == <>(const Scalar& lhs, const Scalar& rhs);
    
    typedef VT value_type;
    static const int FACTOR = F;
    static const value_type FACTOR_PRODUCT = 2 << FACTOR;
    static const value_type FACTOR2_PRODUCT = FACTOR_PRODUCT << 1;
    
    template <typename T>
    Scalar(const T& other);
    template <typename T>
    Scalar& operator=(const T& other);
    template <int OF, typename OVT>
    Scalar(const Scalar<OF, OVT>& other);
    template <int OF, typename OVT>
    Scalar& operator=(const Scalar<OF, OVT>& other);
    Scalar& operator+=(const Scalar& other);
    Scalar& operator*=(const Scalar& other);
    Scalar& operator/=(const Scalar& other);
    Scalar operator-() const;
private:
    value_type value;
};

template <int F, typename VT>
Scalar<F, VT> operator+(const Scalar<F, VT>& lhs, const Scalar<F, VT>& rhs);
template <int F, typename VT>
Scalar<F, VT> operator-(const Scalar<F, VT>& lhs, const Scalar<F, VT>& rhs);
template <int F, typename VT>
Scalar<F, VT> operator*(const Scalar<F, VT>& lhs, const Scalar<F, VT>& rhs);
template <int F, typename VT>
Scalar<F, VT> operator/(const Scalar<F, VT>& lhs, const Scalar<F, VT>& rhs);

template <int F, typename VT>
bool operator<(const Scalar<F, VT>& lhs, const Scalar<F, VT>& rhs);
template <int F, typename VT>
bool operator>(const Scalar<F, VT>& lhs, const Scalar<F, VT>& rhs);
template <int F, typename VT>
bool operator<=(const Scalar<F, VT>& lhs, const Scalar<F, VT>& rhs);
template <int F, typename VT>
bool operator>=(const Scalar<F, VT>& lhs, const Scalar<F, VT>& rhs);
template <int F, typename VT>
bool operator==(const Scalar<F, VT>& lhs, const Scalar<F, VT>& rhs);
template <int F, typename VT>
bool operator!=(const Scalar<F, VT>& lhs, const Scalar<F, VT>& rhs);

}


#endif	/* SCALAR_DECL_H */

