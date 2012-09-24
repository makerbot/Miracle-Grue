/* 
 * File:   Scalar.h
 * Author: Dev
 *
 * Created on September 21, 2012, 1:41 PM
 */

#ifndef MGL_SCALAR_H
#define	MGL_SCALAR_H

#include "Scalar_decl.h"
#include "Scalar_impl.h"

namespace mgl {


//typedef basic_scalar<10, long int> Scalar;
typedef float Scalar;

template <typename T>
T abs(const T& arg) { return arg < 0 ? -arg : arg; }
template <typename T>
T sqrt(const T& arg) { return static_cast<T>(sqrt(static_cast<double>(arg))); }
template <typename T>
T sin(const T& arg) { return static_cast<T>(sin(static_cast<double>(arg))); }
template <typename T>
T cos(const T& arg) { return static_cast<T>(cos(static_cast<double>(arg))); }
template <typename T>
T asin(const T& arg) { return static_cast<T>(asin(static_cast<double>(arg))); }
template <typename T>
T acos(const T& arg) { return static_cast<T>(acos(static_cast<double>(arg))); }

}

#endif	/* MGL_SCALAR_H */

