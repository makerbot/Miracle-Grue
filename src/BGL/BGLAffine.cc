//
//  BGLAffine.cc
//  Part of the Belfry Geometry Library
//
//  Created by GM on 10/13/10.
//  Copyright 2010 Belfry Software. All rights reserved.
//


#include "BGLAffine.h"

namespace BGL {

    Affine Affine::translationAffine(float dx, float dy)
    {
        return Affine(1.0, 0.0, 0.0, 1.0, dx, dy);
    }



    Affine Affine::scalingAffine(float sx, float sy)
    {
        return Affine(sx, 0.0, 0.0, sy, 0.0, 0.0);
    }



    Affine Affine::rotationAffine(float radang)
    {
	float cosv = cosf(radang);
	float sinv = sinf(radang);
	return Affine(cosv, -sinv, sinv, cosv, 0.0, 0.0);
    }



    Affine& Affine::transform(const Affine& aff)
    {
	float olda = a;
	float oldb = b;
	float oldc = c;
	float oldd = d;
	a  = aff.a  * olda  +  aff.b  * oldc;
	b  = aff.a  * oldb  +  aff.b  * oldd;
	c  = aff.c  * olda  +  aff.d  * oldc;
	d  = aff.c  * oldb  +  aff.d  * oldd;
	tx += aff.tx * olda  +  aff.ty * oldc;
	ty += aff.tx * oldb  +  aff.ty * oldd;
	return *this;
    }



    Affine& Affine::translate(float dx, float dy)
    {
        return transform(translationAffine(dx,dy));
    }



    Affine& Affine::scale(float sx, float sy)
    {
        return transform(scalingAffine(sx,sy));
    }



    Affine& Affine::scaleAroundPoint(float sx, float sy, float x, float y)
    {
        translate(-x,-y);
        transform(scalingAffine(sx,sy));
        translate(x,y);
	return *this;
    }



    Affine& Affine::rotate(float radang)
    {
        return transform(rotationAffine(radang));
    }



    Affine& Affine::rotateAroundPoint(float radang, float x, float y)
    {
        translate(-x,-y);
        transform(rotationAffine(radang));
        translate(x,y);
	return *this;
    }



    void Affine::transformPoint(float& x, float &y) const
    {
	float nx = a * x + b * y + tx;
	float ny = c * x + d * y + ty;
	x = nx;
	y = ny;
    }

}


