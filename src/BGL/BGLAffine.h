//
//  BGLAffine.h
//  Part of the Belfry Geometry Library
//
//  Created by GM on 10/13/10.
//  Copyright 2010 Belfry Software. All rights reserved.
//


#ifndef BGL_MATRIX_H
#define BGL_MATRIX_H

#include <math.h>

namespace BGL {


class Affine {
public:
    float a, b, c, d, tx, ty;

    // Constructors
    Affine() : a(1.0), b(0.0), c(0.0), d(1.0), tx(0.0), ty(0.0)  {}
    Affine(float A, float B, float C, float D, float TX, float TY) : a(A), b(B), c(C), d(D), tx(TX), ty(TY) {}
    Affine(const Affine& x) : a(x.a), b(x.b), c(x.c), d(x.d), tx(x.tx), ty(x.ty) {}

    static Affine translationAffine(float dx, float dy);
    static Affine scalingAffine(float sx, float sy);
    static Affine rotationAffine(float radang);

    Affine& transform(const Affine& aff);
    Affine& translate(float dx, float dy);
    Affine& scale(float sx, float sy);
    Affine& scaleAroundPoint(float sx, float sy, float x, float y);
    Affine& rotate(float radang);
    Affine& rotateAroundPoint(float radang, float x, float y);

    void transformPoint(float& x, float &y) const;
};



}

#endif

