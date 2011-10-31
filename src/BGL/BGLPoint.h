//
//  BGLPoint.h
//  Part of the Belfry Geometry Library
//
//  Created by GM on 10/13/10.
//  Copyright 2010 Belfry Software. All rights reserved.
//

#ifndef BGL_POINT_H
#define BGL_POINT_H

#include <math.h>
#include <iostream>
#include <list>
#include "config.h"
#include "BGLCommon.h"
#include "BGLAffine.h"
#include "BGLPoint3d.h"



namespace BGL {


class Point {
public:
    // Member variables
    Scalar x, y;

    // Constructors
    Point() : x(0.0), y(0.0) {}
    Point(Scalar nux, Scalar nuy) : x(nux), y(nuy) {}
    Point(const Point &pt) : x(pt.x), y(pt.y) {}
    Point(const Point3d &pt) : x(pt.x), y(pt.y) {}

    // Assignment operator
    Point& operator=(const Point &rhs) {
	if (this != &rhs) {
	    this->x = rhs.x;
	    this->y = rhs.y;
	}
	return *this;
    }

    // Compound assignment operators
    Point& operator+=(const Point &rhs) {
        this->x += rhs.x;
        this->y += rhs.y;
	return *this;
    }
    Point& operator-=(const Point &rhs) {
        this->x -= rhs.x;
        this->y -= rhs.y;
	return *this;
    }
    Point& operator*=(Scalar rhs) {
        this->x *= rhs;
        this->y *= rhs;
	return *this;
    }
    Point& operator*=(const Point &rhs) {
        this->x *= rhs.x;
        this->y *= rhs.y;
	return *this;
    }
    Point& operator/=(Scalar rhs) {
        this->x /= rhs;
        this->y /= rhs;
	return *this;
    }
    Point& operator/=(const Point &rhs) {
        this->x /= rhs.x;
        this->y /= rhs.y;
	return *this;
    }

    // Binary arithmetic operators
    const Point operator+(const Point &rhs) const {
	return Point(*this) += rhs;
    }
    const Point operator-(const Point &rhs) const {
	return Point(*this) -= rhs;
    }
    const Point operator*(Scalar rhs) const {
	return Point(*this) *= rhs;
    }
    const Point operator*(const Point &rhs) const {
	return Point(*this) *= rhs;
    }
    const Point operator/(Scalar rhs) const {
	return Point(*this) /= rhs;
    }
    const Point operator/(const Point &rhs) const {
	return Point(*this) /= rhs;
    }

    // Comparison operators
    bool operator==(const Point &rhs) const {
        return (fabsf(x-rhs.x) < CLOSEENOUGH &&  fabsf(y-rhs.y) < CLOSEENOUGH);
    }
    bool operator>=(const Point &rhs) const {
	if (x < rhs.x) {
	    return false;
	}
	if (y < rhs.y) {
	    return false;
	}
	return true;
    }
    bool operator<=(const Point &rhs) const {
	if (x > rhs.x) {
	    return false;
	}
	if (y > rhs.y) {
	    return false;
	}
	return true;
    }
    bool operator>(const Point &rhs) const {
        return !(*this <= rhs);
    }
    bool operator<(const Point &rhs) const {
        return !(*this >= rhs);
    }
    bool operator!=(const Point &rhs) const {
        return !(*this == rhs);
    }

    // Transformations
    Point& scale(Scalar scale) {
	*this *= scale;
	return *this;
    }
    Point& scale(const Point& vect) {
	*this *= vect;
	return *this;
    }
    Point& scaleAroundPoint(const Point& center, Scalar scale) {
	*this -= center;
	*this *= scale;
	*this += center;
	return *this;
    }
    Point& scaleAroundPoint(const Point& center, const Point& vect) {
	*this -= center;
	*this *= vect;
	*this += center;
	return *this;
    }

    // Calculations
    Scalar distanceFrom(const Point& pt) const {
        Point delta = *this - pt;
	return hypotf(delta.y,delta.x);
    }
    Scalar angleToPoint(const Point& pt) const {
        Point delta = *this - pt;
	return atan2f(delta.y,delta.x);
    }

    // Friend functions
    friend std::ostream& operator <<(std::ostream &os,const Point &pt);
};

typedef std::list<Point> Points;



}

#endif

