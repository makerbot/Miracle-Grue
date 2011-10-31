//
//  BGLPoint3d.h
//  Part of the Belfry Geometry Library
//
//  Created by GM on 10/13/10.
//  Copyright 2010 Belfry Software. All rights reserved.
//

#ifndef BGL_POINT3D_H
#define BGL_POINT3D_H

#include <math.h>
#include <iostream>
#include "config.h"
#include "BGLCommon.h"



namespace BGL {

class Point3d {
public:
    // Member variables
    Scalar x, y, z;

    // Constructors
    Point3d() : x(0.0), y(0.0), z(0.0) {}
    Point3d(Scalar nux, Scalar nuy, Scalar nuz) : x(nux), y(nuy), z(nuz) {}
    Point3d(const Point3d &pt) : x(pt.x), y(pt.y), z(pt.z) {}

    // Assignment operator
    Point3d& operator=(const Point3d &rhs) {
	if (this != &rhs) {
	    this->x = rhs.x;
	    this->y = rhs.y;
	    this->z = rhs.z;
	}
	return *this;
    }

    // Compound assignment operators
    Point3d& operator+=(const Point3d &rhs) {
        this->x += rhs.x;
        this->y += rhs.y;
        this->z += rhs.z;
	return *this;
    }
    Point3d& operator-=(const Point3d &rhs) {
        this->x -= rhs.x;
        this->y -= rhs.y;
        this->z -= rhs.z;
	return *this;
    }
    Point3d& operator*=(Scalar rhs) {
        this->x *= rhs;
        this->y *= rhs;
        this->z *= rhs;
	return *this;
    }
    Point3d& operator*=(const Point3d &rhs) {
        this->x *= rhs.x;
        this->y *= rhs.y;
        this->z *= rhs.z;
	return *this;
    }
    Point3d& operator/=(Scalar rhs) {
        this->x /= rhs;
        this->y /= rhs;
        this->z /= rhs;
	return *this;
    }
    Point3d& operator/=(const Point3d &rhs) {
        this->x /= rhs.x;
        this->y /= rhs.y;
        this->z /= rhs.z;
	return *this;
    }

    // Binary arithmetic operators
    const Point3d operator+(const Point3d &rhs) const {
	return Point3d(*this) += rhs;
    }
    const Point3d operator-(const Point3d &rhs) const {
	return Point3d(*this) -= rhs;
    }
    const Point3d operator*(Scalar rhs) const {
	return Point3d(*this) *= rhs;
    }
    const Point3d operator*(const Point3d &rhs) const {
	return Point3d(*this) *= rhs;
    }
    const Point3d operator/(Scalar rhs) const {
	return Point3d(*this) /= rhs;
    }
    const Point3d operator/(const Point3d &rhs) const {
	return Point3d(*this) /= rhs;
    }

    // Comparison operators
    bool operator==(const Point3d &rhs) const {
        return (fabsf(this->x-rhs.x) + fabsf(this->y+rhs.y) + fabsf(this->z+rhs.z) < CLOSEENOUGH);
    }
    bool operator!=(const Point3d &rhs) const {
        return !(*this == rhs);
    }

    // Comparators for z height Scalar.
    bool operator== (Scalar zcoord) const {
        return (fabsf(z-zcoord) < CLOSEENOUGH);
    }
    bool operator< (Scalar zcoord) const {
        return (z < zcoord);
    }
    bool operator> (Scalar zcoord) const {
        return (z > zcoord);
    }
    bool operator<= (Scalar zcoord) const {
        return !(*this > zcoord);
    }
    bool operator>= (Scalar zcoord) const {
        return !(*this < zcoord);
    }


    // Transformations
    Point3d& scale(Scalar scale) {
	*this *= scale;
	return *this;
    }
    Point3d& scale(const Point3d& vect) {
	*this *= vect;
	return *this;
    }
    Point3d& scaleAroundPoint3d(const Point3d& center, Scalar scale) {
	*this -= center;
	*this *= scale;
	*this += center;
	return *this;
    }
    Point3d& scaleAroundPoint3d(const Point3d& center, const Point3d& vect) {
	*this -= center;
	*this *= vect;
	*this += center;
	return *this;
    }

    // Calculations
    Scalar distanceFrom(const Point3d& pt) const {
        Point3d vect(*this);
	vect -= pt;
	vect *= vect;
	return sqrtf(vect.x+vect.y+vect.z);
    }

    // Friend functions
    friend std::ostream& operator <<(std::ostream &os,const Point3d &pt);
};


}

#endif

