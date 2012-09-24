/* 
 * File:   Scalar.cc
 * Author: Dev
 * 
 * Created on September 21, 2012, 1:41 PM
 */

#include "Scalar.h"

namespace mgl {

Scalar abs(const Scalar& arg) { return arg < 0 ? -arg : arg; }
Scalar sqrt(const Scalar& arg) { return static_cast<Scalar>(sqrt(static_cast<double>(arg))); }
Scalar sin(const Scalar& arg) { return static_cast<Scalar>(sin(static_cast<double>(arg))); }
Scalar cos(const Scalar& arg) { return static_cast<Scalar>(cos(static_cast<double>(arg))); }
Scalar asin(const Scalar& arg) { return static_cast<Scalar>(asin(static_cast<double>(arg))); }
Scalar acos(const Scalar& arg) { return static_cast<Scalar>(acos(static_cast<double>(arg))); }

}

