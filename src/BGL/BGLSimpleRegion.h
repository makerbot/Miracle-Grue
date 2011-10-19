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
typedef list<SimpleRegion> SimpleRegions;

class SimpleRegion {
public:
    Path outerPath;
    Paths subpaths;
    Scalar zLevel;

    SimpleRegion() : outerPath(), subpaths(), zLevel(0.0f) {}
    SimpleRegion(const Path &x) : outerPath(x), subpaths(), zLevel(0.0f) {}
    SimpleRegion(const SimpleRegion &x) : outerPath(x.outerPath), subpaths(x.subpaths), zLevel(x.zLevel) {}

    int32_t size();
    bool contains(const Point &pt) const;

    bool intersects(const Path& path) const;
    bool intersects(const SimpleRegion& path) const;

    string svgPathWithOffset(Scalar dx, Scalar dy);

    static SimpleRegions &assembleSimpleRegionsFrom(Paths &paths, SimpleRegions &outRegs);
    static SimpleRegions &assembleSimpleRegionsFrom(const Paths &outerPaths, const Paths &innerPaths, SimpleRegions &outRegs);

    static SimpleRegions& unionOf       (SimpleRegion &r1, SimpleRegion &r2, SimpleRegions &outReg);
    static SimpleRegions& differenceOf  (SimpleRegion &r1, SimpleRegion &r2, SimpleRegions &outReg);
    static SimpleRegions& intersectionOf(SimpleRegion &r1, SimpleRegion &r2, SimpleRegions &outReg);

    Lines &containedSegmentsOfLine(Line &line, Lines &lnsref);
    Paths &containedSubpathsOfPath(const Path &path, Paths &pathsref);

    Paths &infillPathsForRegionWithDensity(Scalar density, Scalar extrusionWidth, Paths &outPaths);

};


}

#endif

