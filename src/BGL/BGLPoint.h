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

using namespace std;

namespace BGL {


class Point {
public:
    // Member variables
    double x, y;

    // Constructors
    Point() : x(0.0), y(0.0) {}
    Point(double nux, double nuy) : x(nux), y(nuy) {}
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
    Point& operator*=(double rhs) {
        this->x *= rhs;
        this->y *= rhs;
	return *this;
    }
    Point& operator*=(const Point &rhs) {
        this->x *= rhs.x;
        this->y *= rhs.y;
	return *this;
    }
    Point& operator/=(double rhs) {
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
    const Point operator*(double rhs) const {
	return Point(*this) *= rhs;
    }
    const Point operator*(const Point &rhs) const {
	return Point(*this) *= rhs;
    }
    const Point operator/(double rhs) const {
	return Point(*this) /= rhs;
    }
    const Point operator/(const Point &rhs) const {
	return Point(*this) /= rhs;
    }

    // Comparison operators
    bool operator==(const Point &rhs) const {
        return (fabs(x-rhs.x) < CLOSEENOUGH &&  fabs(y-rhs.y) < CLOSEENOUGH);
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
    Point& scale(double scale) {
	*this *= scale;
	return *this;
    }
    Point& scale(const Point& vect) {
	*this *= vect;
	return *this;
    }
    Point& scaleAroundPoint(const Point& center, double scale) {
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

    void quantize(float quanta) {
        x = floor(x/quanta) * quanta;
        y = floor(y/quanta) * quanta;
    }

    void quantize() {
        quantize(CLOSEENOUGH/2.0);
    }

    // Calculations
    double distanceFrom(const Point& pt) const {
        Point delta = *this - pt;
	return hypot(delta.y,delta.x);
    }
    double angleToPoint(const Point& pt) const {
        Point delta = pt - *this;
	return atan2(delta.y,delta.x);
    }

    Point &polarOffset(double ang, double rad) {
        x += rad*cos(ang);
	y += rad*sin(ang);
	return *this;
    }

    // Friend functions
    friend ostream& operator <<(ostream &os,const Point &pt);
};

typedef list<Point> Points;



}

#endif

