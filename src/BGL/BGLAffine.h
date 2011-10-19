//
//  BGLAffine.h
//  Part of the Belfry Geometry Library
//
//  Created by GM on 10/13/10.
//  Copyright 2010 Belfry Software. All rights reserved.
//


#ifndef BGL_MATRIX_H
#define BGL_MATRIX_H

#include "config.h"
#include <math.h>

namespace BGL {


class Affine {
public:
    Scalar a, b, c, d, tx, ty;

    // Constructors
    Affine() : a(1.0), b(0.0), c(0.0), d(1.0), tx(0.0), ty(0.0)  {}
    Affine(Scalar A, Scalar B, Scalar C, Scalar D, Scalar TX, Scalar TY) : a(A), b(B), c(C), d(D), tx(TX), ty(TY) {}
    Affine(const Affine& x) : a(x.a), b(x.b), c(x.c), d(x.d), tx(x.tx), ty(x.ty) {}

    static Affine translationAffine(Scalar dx, Scalar dy);
    static Affine scalingAffine(Scalar sx, Scalar sy);
    static Affine rotationAffine(Scalar radang);

    Affine& transform(const Affine& aff);
    Affine& translate(Scalar dx, Scalar dy);
    Affine& scale(Scalar sx, Scalar sy);
    Affine& scaleAroundPoint(Scalar sx, Scalar sy, Scalar x, Scalar y);
    Affine& rotate(Scalar radang);
    Affine& rotateAroundPoint(Scalar radang, Scalar x, Scalar y);

    void transformPoint(Scalar& x, Scalar &y) const;
};



}

#endif

