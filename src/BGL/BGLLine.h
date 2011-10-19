//
//  BGLLine.h
//  Part of the Belfry Geometry Library
//
//  Created by GM on 10/13/10.
//  Copyright 2010 Belfry Software. All rights reserved.
//


#ifndef BGL_LINE_H
#define BGL_LINE_H

#include <math.h>
#include <iostream>
#include <list>
#include "config.h"
#include "BGLCommon.h"
#include "BGLAffine.h"
#include "BGLIntersection.h"
#include "BGLPoint.h"

using namespace std;

namespace BGL {


class Line {
public:
    // Member variables
    Point startPt;
    Point endPt;
    int16_t flags;

    // Constructors
    Line() : startPt(), endPt() {}
    Line(const Point& p1, const Point& p2) : startPt(p1), endPt(p2) {}
    Line(const Line& ln) : startPt(ln.startPt), endPt(ln.endPt) {}

    // Assignment operator
    Line& operator=(const Line &rhs) {
	if (this != &rhs) {
	    startPt = rhs.startPt;
	    endPt = rhs.endPt;
	}
	return *this;
    }

    // Compound assignment operators
    Line& operator+=(const Point &rhs) {
        this->startPt += rhs;
        this->endPt += rhs;
	return *this;
    }
    Line& operator-=(const Point &rhs) {
        this->startPt -= rhs;
        this->endPt -= rhs;
	return *this;
    }
    Line& operator*=(Scalar rhs) {
        this->startPt *= rhs;
        this->endPt *= rhs;
	return *this;
    }
    Line& operator*=(const Point &rhs) {
        this->startPt *= rhs;
        this->endPt *= rhs;
	return *this;
    }
    Line& operator/=(Scalar rhs) {
        this->startPt /= rhs;
        this->endPt /= rhs;
	return *this;
    }
    Line& operator/=(const Point &rhs) {
        this->startPt /= rhs;
        this->endPt /= rhs;
	return *this;
    }

    // Binary arithmetic operators
    const Line operator+(const Point &rhs) const {
	return Line(*this) += rhs;
    }
    const Line operator-(const Point &rhs) const {
	return Line(*this) -= rhs;
    }
    const Line operator*(Scalar rhs) const {
	return Line(*this) *= rhs;
    }
    const Line operator*(const Point &rhs) const {
	return Line(*this) *= rhs;
    }
    const Line operator/(Scalar rhs) const {
	return Line(*this) /= rhs;
    }
    const Line operator/(const Point &rhs) const {
	return Line(*this) /= rhs;
    }

    // Comparison operators
    bool operator==(const Line &rhs) const {
        return ((startPt == rhs.startPt && endPt == rhs.endPt) ||
                (startPt == rhs.endPt && endPt == rhs.startPt));
    }
    bool operator!=(const Line &rhs) const {
        return !(*this == rhs);
    }

    bool hasEndPoint(const Point& pt) const {
        return (pt == startPt || pt == endPt);
    }

    // Transformations
    Line& scale(Scalar scale) {
	*this *= scale;
	return *this;
    }
    Line& scale(const Point& vect) {
	*this *= vect;
	return *this;
    }
    Line& scaleAroundPoint(const Point& center, Scalar scale) {
	*this -= center;
	*this *= scale;
	*this += center;
	return *this;
    }
    Line& scaleAroundPoint(const Point& center, const Point& vect) {
	*this -= center;
	*this *= vect;
	*this += center;
	return *this;
    }

    // Calculations
    Scalar length() const {
        return startPt.distanceFrom(endPt);
    }
    Scalar angle() const {
        return startPt.angleToPoint(endPt);
    }
    Scalar angleDelta(const Line& ln) const {
        Scalar delta = angle() - ln.angle();
	if (delta < -M_PI) {
	    delta += M_PI * 2.0f;
	} else if (delta > M_PI) {
	    delta -= M_PI * 2.0f;
	}
	return delta;
    }

    // Misc
    Line& reverse() {
        Point tmpPt = startPt;
	startPt = endPt;
	endPt = tmpPt;
	return *this;
    }
    bool isLinearWith(const Point& pt) const;
    bool hasInBounds(const Point &pt) const;
    bool contains(const Point &pt) const;
    Point closestSegmentPointTo(const Point &pt) const;
    Point closestExtendedLinePointTo(const Point &pt) const;
    Scalar minimumSegmentDistanceFromPoint(const Point &pt) const;
    Scalar minimumExtendedLineDistanceFromPoint(const Point &pt) const;
    Intersection intersectionWithSegment(const Line &ln) const;
    Intersection intersectionWithExtendedLine(const Line &ln) const;

    // Friend functions
    friend ostream& operator <<(ostream &os,const Line &pt);
};

typedef list<Line> Lines;



}


#endif

