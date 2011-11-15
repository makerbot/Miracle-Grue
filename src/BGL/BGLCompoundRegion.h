//
//  BGLCompoundRegion.h
//  Part of the Belfry Geometry Library
//
//  Created by GM on 10/14/10.
//  Copyright 2010 Belfry Software. All rights reserved.
//


#ifndef BGL_REGION_H
#define BGL_REGION_H

#include "config.h"
#include "BGLSimpleRegion.h"
#include "BGLPath.h"
#include "BGLLine.h"

namespace BGL {
/*Texture*/
enum Texture {
	INFILL = 0,
	PERIMETER,
};

class CompoundRegion {
public:
    SimpleRegions subregions;
    Scalar zLevel;
    Texture texture;

    CompoundRegion() : subregions(), zLevel(0.0f),texture(/*Texture*/PERIMETER) {};
    CompoundRegion(const SimpleRegions &x) : subregions(x), zLevel(0.0f),texture(/*Texture*/PERIMETER) {}
    CompoundRegion(const CompoundRegion &x) : subregions(x.subregions), zLevel(x.zLevel),texture(/*Texture*/PERIMETER) {}
    CompoundRegion(const Paths &x) : subregions(), zLevel(0.0f),texture(/*Texture*/PERIMETER) {
        Paths::const_iterator it;
	for (it = x.begin(); it != x.end(); it++) {
	    subregions.push_back(SimpleRegion(*it));
	}
    }

    // Compound assignment operators
    CompoundRegion& operator+=(const Point &rhs);
    CompoundRegion& operator-=(const Point &rhs);
    CompoundRegion& operator*=(double rhs);
    CompoundRegion& operator*=(const Point &rhs);
    CompoundRegion& operator/=(double rhs);
    CompoundRegion& operator/=(const Point &rhs);

    // Binary arithmetic operators
    const CompoundRegion operator+(const Point &rhs) const {
	return CompoundRegion(*this) += rhs;
    }
    const CompoundRegion operator-(const Point &rhs) const {
	return CompoundRegion(*this) -= rhs;
    }
    const CompoundRegion operator*(double rhs) const {
	return CompoundRegion(*this) *= rhs;
    }
    const CompoundRegion operator*(const Point &rhs) const {
	return CompoundRegion(*this) *= rhs;
    }
    const CompoundRegion operator/(double rhs) const {
	return CompoundRegion(*this) /= rhs;
    }
    const CompoundRegion operator/(const Point &rhs) const {
	return CompoundRegion(*this) /= rhs;
    }

    int size() const;
    bool contains(const Point &pt) const;

    std::string svgPathWithOffset(double dx, double dy);
    std::ostream &svgPathDataWithOffset(std::ostream& os, double dx, double dy) const;
    std::ostream &svgPathWithOffset(std::ostream& os, double dx, double dy) const;

    void simplify(double minErr);

    CompoundRegion &unionWith(SimpleRegion &reg);
    CompoundRegion &differenceWith(SimpleRegion &reg);
    CompoundRegion &intersectionWith(SimpleRegion &reg);

    CompoundRegion &unionWith(CompoundRegion &reg);
    CompoundRegion &differenceWith(CompoundRegion &reg);
    CompoundRegion &intersectionWith(CompoundRegion &reg);


    CompoundRegion &insetBy(double insetBy, CompoundRegion &outReg);
    CompoundRegion &inset(double insetBy, CompoundRegion &outReg);

    static CompoundRegion &differenceOf  (CompoundRegion &r1, CompoundRegion &r2, CompoundRegion &outReg);
    static CompoundRegion &unionOf       (CompoundRegion &r1, CompoundRegion &r2, CompoundRegion &outReg);
    static CompoundRegion &intersectionOf(CompoundRegion &r1, CompoundRegion &r2, CompoundRegion &outReg);

    static CompoundRegion &assembleCompoundRegionFrom(Paths &paths, CompoundRegion &outReg);

    Lines &containedSegmentsOfLine(Line &line, Lines &lnsref);
    Paths &containedSubpathsOfPath(Path &path, Paths &pathsref);

    Paths &infillPathsForRegionWithDensity(double density, double extrusionWidth, Paths &outPaths);

};

typedef std::list<CompoundRegion> CompoundRegions;


}

#endif /*BGL_REGION_H*/
