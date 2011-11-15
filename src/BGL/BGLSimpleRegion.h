//
//  BGLRegion.h
//  Part of the Belfry Geometry Library
//
//  Created by GM on 10/14/10.
//  Copyright 2010 Belfry Software. All rights reserved.
//


#ifndef BGL_CONTIGUOUSREGION_H
#define BGL_CONTIGUOUSREGION_H

#include "config.h"
#include "BGLPath.h"
#include "BGLLine.h"

namespace BGL {

class SimpleRegion;
typedef std::list<SimpleRegion> SimpleRegions;

class SimpleRegion {
public:
    Path outerPath;
    Paths subpaths;
    double zLevel;

    SimpleRegion() : outerPath(), subpaths(), zLevel(0.0f) {}
    SimpleRegion(const Path &x) : outerPath(x), subpaths(), zLevel(0.0f) {}
    SimpleRegion(const SimpleRegion &x) : outerPath(x.outerPath), subpaths(x.subpaths), zLevel(x.zLevel) {}

    // Compound assignment operators
    SimpleRegion& operator+=(const Point &rhs);
    SimpleRegion& operator-=(const Point &rhs);
    SimpleRegion& operator*=(double rhs);
    SimpleRegion& operator*=(const Point &rhs);
    SimpleRegion& operator/=(double rhs);
    SimpleRegion& operator/=(const Point &rhs);

    // Binary arithmetic operators
    const SimpleRegion operator+(const Point &rhs) const {
	return SimpleRegion(*this) += rhs;
    }
    const SimpleRegion operator-(const Point &rhs) const {
	return SimpleRegion(*this) -= rhs;
    }
    const SimpleRegion operator*(double rhs) const {
	return SimpleRegion(*this) *= rhs;
    }
    const SimpleRegion operator*(const Point &rhs) const {
	return SimpleRegion(*this) *= rhs;
    }
    const SimpleRegion operator/(double rhs) const {
	return SimpleRegion(*this) /= rhs;
    }
    const SimpleRegion operator/(const Point &rhs) const {
	return SimpleRegion(*this) /= rhs;
    }

    int size();
    bool contains(const Point &pt) const;

    bool intersects(const Path& path) const;
    bool intersects(const SimpleRegion& path) const;

    std::string svgPathWithOffset(double dx, double dy);
    std::ostream &svgPathDataWithOffset(std::ostream& os, double dx, double dy) const;
    std::ostream &svgPathWithOffset(std::ostream& os, double dx, double dy) const;

    void simplify(double minErr);

    static SimpleRegions &assembleSimpleRegionsFrom(Paths &paths, SimpleRegions &outRegs);
    static SimpleRegions &assembleSimpleRegionsFrom(const Paths &outerPaths, const Paths &innerPaths, SimpleRegions &outRegs);

    static SimpleRegions& unionOf       (SimpleRegion &r1, SimpleRegion &r2, SimpleRegions &outReg);
    static SimpleRegions& differenceOf  (SimpleRegion &r1, SimpleRegion &r2, SimpleRegions &outReg);
    static SimpleRegions& intersectionOf(SimpleRegion &r1, SimpleRegion &r2, SimpleRegions &outReg);

    SimpleRegions &inset(double offsetby, SimpleRegions& outRegs);

    Lines &containedSegmentsOfLine(Line &line, Lines &lnsref);
    Paths &containedSubpathsOfPath(const Path &path, Paths &pathsref);

    Paths &infillPathsForRegionWithDensity(double density, double extrusionWidth, Paths &outPaths);
};


}

#endif

