//
//  BGLAffine.cc
//  Part of the Belfry Geometry Library
//
//  Created by GM on 10/13/10.
//  Copyright 2010 Belfry Software. All rights reserved.
//


#include "BGLAffine.h"

namespace BGL {

    Affine Affine::translationAffine(Scalar dx, Scalar dy)
    {
        return Affine(1.0, 0.0, 0.0, 1.0, dx, dy);
    }



    Affine Affine::scalingAffine(Scalar sx, Scalar sy)
    {
        return Affine(sx, 0.0, 0.0, sy, 0.0, 0.0);
    }



    Affine Affine::rotationAffine(Scalar radang)
    {
	Scalar cosv = cosf(radang);
	Scalar sinv = sinf(radang);
	return Affine(cosv, -sinv, sinv, cosv, 0.0, 0.0);
    }



    Affine& Affine::transform(const Affine& aff)
    {
	Scalar olda = a;
	Scalar oldb = b;
	Scalar oldc = c;
	Scalar oldd = d;
	a  = aff.a  * olda  +  aff.b  * oldc;
	b  = aff.a  * oldb  +  aff.b  * oldd;
	c  = aff.c  * olda  +  aff.d  * oldc;
	d  = aff.c  * oldb  +  aff.d  * oldd;
	tx += aff.tx * olda  +  aff.ty * oldc;
	ty += aff.tx * oldb  +  aff.ty * oldd;
	return *this;
    }



    Affine& Affine::translate(Scalar dx, Scalar dy)
    {
        return transform(translationAffine(dx,dy));
    }



    Affine& Affine::scale(Scalar sx, Scalar sy)
    {
        return transform(scalingAffine(sx,sy));
    }



    Affine& Affine::scaleAroundPoint(Scalar sx, Scalar sy, Scalar x, Scalar y)
    {
        translate(-x,-y);
        transform(scalingAffine(sx,sy));
        translate(x,y);
	return *this;
    }



    Affine& Affine::rotate(Scalar radang)
    {
        return transform(rotationAffine(radang));
    }



    Affine& Affine::rotateAroundPoint(Scalar radang, Scalar x, Scalar y)
    {
        translate(-x,-y);
        transform(rotationAffine(radang));
        translate(x,y);
	return *this;
    }



    void Affine::transformPoint(Scalar& x, Scalar &y) const
    {
	Scalar nx = a * x + b * y + tx;
	Scalar ny = c * x + d * y + ty;
	x = nx;
	y = ny;
    }

}


