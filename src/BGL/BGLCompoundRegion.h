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


class CompoundRegion {
public:
    SimpleRegions subregions;
    Scalar zLevel;

    CompoundRegion() : subregions(), zLevel(0.0f) {}
    CompoundRegion(const SimpleRegions &x) : subregions(x), zLevel(0.0f) {}
    CompoundRegion(const CompoundRegion &x) : subregions(x.subregions), zLevel(x.zLevel) {}


    int32_t size() const;
    bool contains(const Point &pt) const;
    std::string svgPathWithOffset(Scalar dx, Scalar dy);

    CompoundRegion &unionWith(SimpleRegion &reg);
    CompoundRegion &differenceWith(SimpleRegion &reg);
    CompoundRegion &intersectionWith(SimpleRegion &reg);

    CompoundRegion &unionWith(CompoundRegion &reg);
    CompoundRegion &differenceWith(CompoundRegion &reg);
    CompoundRegion &intersectionWith(CompoundRegion &reg);

    static CompoundRegion &differenceOf  (CompoundRegion &r1, CompoundRegion &r2, CompoundRegion &outReg);
    static CompoundRegion &unionOf       (CompoundRegion &r1, CompoundRegion &r2, CompoundRegion &outReg);
    static CompoundRegion &intersectionOf(CompoundRegion &r1, CompoundRegion &r2, CompoundRegion &outReg);

    static CompoundRegion &assembleCompoundRegionFrom(Paths &paths, CompoundRegion &outReg);

    Lines &containedSegmentsOfLine(Line &line, Lines &lnsref);
    Paths &containedSubpathsOfPath(Path &path, Paths &pathsref);

    Paths &infillPathsForRegionWithDensity(Scalar density, Scalar extrusionWidth, Paths &outPaths);

};
typedef std::list<CompoundRegion> CompoundRegions;


}

#endif

