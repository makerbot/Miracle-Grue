/* 
 * File:   Scalar.h
 * Author: Dev
 *
 * Created on September 21, 2012, 1:41 PM
 */

#ifndef MGL_SCALAR_H
#define	MGL_SCALAR_H

//#include "Scalar_decl.h"
//#include "Scalar_impl.h"

namespace mgl {
//typedef basic_scalar<10, long int> Scalar;

typedef double Scalar;

Scalar abs(const Scalar& arg);
Scalar sqrt(const Scalar& arg);
Scalar sin(const Scalar& arg);
Scalar cos(const Scalar& arg);
Scalar asin(const Scalar& arg);
Scalar acos(const Scalar& arg);

}

#endif	/* MGL_SCALAR_H */

