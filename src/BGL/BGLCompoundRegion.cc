//
//  BGLCompoundRegion.m
//  Part of the Belfry Geometry Library
//
//  Created by GM on 10/14/10.
//  Copyright 2010 Belfry Software. All rights reserved.
//

#include <sstream>
#include <fstream>
#include "BGLCommon.h"
#include "BGLPoint.h"
#include "BGLCompoundRegion.h"

using namespace std;


namespace BGL {


// Compound assignment operators
CompoundRegion& CompoundRegion::operator+=(const Point &rhs) {
    SimpleRegions::iterator it;
    for (it = subregions.begin(); it != subregions.end(); it++) {
	*it += rhs;
    }
    return *this;
}



CompoundRegion& CompoundRegion::operator-=(const Point &rhs) {
    SimpleRegions::iterator it;
    for (it = subregions.begin(); it != subregions.end(); it++) {
	*it -= rhs;
    }
    return *this;
}



CompoundRegion& CompoundRegion::operator*=(double rhs) {
    SimpleRegions::iterator it;
    for (it = subregions.begin(); it != subregions.end(); it++) {
	*it *= rhs;
    }
    return *this;
}



CompoundRegion& CompoundRegion::operator*=(const Point &rhs) {
    SimpleRegions::iterator it;
    for (it = subregions.begin(); it != subregions.end(); it++) {
	*it *= rhs;
    }
    return *this;
}



CompoundRegion& CompoundRegion::operator/=(double rhs) {
    SimpleRegions::iterator it;
    for (it = subregions.begin(); it != subregions.end(); it++) {
	*it /= rhs;
    }
    return *this;
}



CompoundRegion& CompoundRegion::operator/=(const Point &rhs) {
    SimpleRegions::iterator it;
    for (it = subregions.begin(); it != subregions.end(); it++) {
	*it /= rhs;
    }
    return *this;
}





int CompoundRegion::size() const
{
    return subregions.size();
}



bool CompoundRegion::contains(const Point &pt) const
{
    SimpleRegions::const_iterator it;
    for (it = subregions.begin(); it != subregions.end(); it++) {
        if (it->contains(pt)) {
	    return true;
	}
    }
    return false;
}



void CompoundRegion::simplify(double minErr)
{
    SimpleRegions::iterator it;
    for (it = subregions.begin(); it != subregions.end(); it++) {
	it->simplify(minErr);
    }
}



CompoundRegion &CompoundRegion::assembleCompoundRegionFrom(Paths &paths, CompoundRegion &outReg)
{
    SimpleRegion::assembleSimpleRegionsFrom(paths, outReg.subregions);
    return outReg;
}



string CompoundRegion::svgPathWithOffset(double dx, double dy)
{
    string out;
    SimpleRegions::iterator rit;
    for (rit = subregions.begin(); rit != subregions.end(); rit++) {
        out.append(rit->svgPathWithOffset(dx, dy));
    }
    return out;
}



ostream &CompoundRegion::svgPathDataWithOffset(ostream& os, double dx, double dy) const
{
    SimpleRegions::const_iterator rit;
    for (rit = subregions.begin(); rit != subregions.end(); rit++) {
        rit->svgPathDataWithOffset(os, dx, dy);
    }
    return os;
}



ostream &CompoundRegion::svgPathWithOffset(ostream& os, double dx, double dy) const
{
    os << "<path fill=\"none\" d=\"";
    svgPathDataWithOffset(os, dx, dy);
    os << "\" />" << endl;
    return os;
}



CompoundRegion &CompoundRegion::unionWith(SimpleRegion &reg)
{
    SimpleRegion currReg(reg);
    SimpleRegions::iterator rit;
    for (rit = subregions.begin(); rit != subregions.end(); ) {
        if (currReg.intersects(*rit)) {
	    SimpleRegions tempRegs;
	    SimpleRegion::unionOf(currReg, *rit, tempRegs);
	    currReg = tempRegs.front();
	    rit = subregions.erase(rit);
	} else {
	    rit++;
	}
    }
    subregions.push_back(currReg);
    return *this;
}



CompoundRegion &CompoundRegion::differenceWith(SimpleRegion &reg)
{
    SimpleRegions::iterator rit;
    SimpleRegions::iterator rit2;
    for (rit = subregions.begin(); rit != subregions.end(); ) {
        if (reg.intersects(*rit)) {
	    SimpleRegions tempRegs;
	    SimpleRegion::differenceOf(*rit, reg, tempRegs);
	    rit = subregions.erase(rit);
	    for (rit2 = tempRegs.begin(); rit2 != tempRegs.end(); rit2++) {
	        subregions.push_front(*rit2);
	    }
	} else {
	    rit++;
	}
    }
    return *this;
}



CompoundRegion &CompoundRegion::intersectionWith(SimpleRegion &reg)
{
    // TODO: implement.
    return *this;
}



CompoundRegion &CompoundRegion::unionWith(CompoundRegion &reg)
{
    SimpleRegions::iterator rit;
    for (rit = reg.subregions.begin(); rit != reg.subregions.end(); rit++) {
        unionWith(*rit);
    }
    return *this;
}



CompoundRegion &CompoundRegion::differenceWith(CompoundRegion &reg)
{
    SimpleRegions::iterator rit;
    for (rit = reg.subregions.begin(); rit != reg.subregions.end(); rit++) {
        differenceWith(*rit);
    }
    return *this;
}



CompoundRegion &CompoundRegion::intersectionWith(CompoundRegion &reg)
{
    // TODO: implement.
    return *this;
}



CompoundRegion &CompoundRegion::unionOf(CompoundRegion &r1, CompoundRegion &r2, CompoundRegion &outReg)
{
    outReg = r1;
    outReg.unionWith(r2);
    return outReg;
}



CompoundRegion &CompoundRegion::differenceOf(CompoundRegion &r1, CompoundRegion &r2, CompoundRegion &outReg)
{
    outReg = r1;
    outReg.differenceWith(r2);
    return outReg;
}



CompoundRegion &CompoundRegion::intersectionOf(CompoundRegion &r1, CompoundRegion &r2, CompoundRegion &outReg)
{
    outReg = r1;
    outReg.intersectionWith(r2);
    return outReg;
}


CompoundRegion &CompoundRegion::insetBy(double insetBy, CompoundRegion &outReg)
{
    SimpleRegions::iterator it1;
    for (it1 = subregions.begin(); it1 != subregions.end(); it1++) {
    	CompoundRegion insetReg;
    	it1->inset(insetBy, insetReg.subregions);
    	outReg.unionWith(insetReg);
    }
    return outReg;
}


CompoundRegion &CompoundRegion::inset(double insetBy, CompoundRegion &outReg)
{
    SimpleRegions::iterator it1;
    for (it1 = subregions.begin(); it1 != subregions.end(); it1++) {
    	CompoundRegion insetReg;
    	it1->inset(insetBy, insetReg.subregions);
    	outReg.unionWith(insetReg);
    }
    return outReg;
}



Lines &CompoundRegion::containedSegmentsOfLine(Line &line, Lines &outSegs)
{
    SimpleRegions::iterator rit;
    for (rit = subregions.begin(); rit != subregions.end(); rit++) {
        rit->containedSegmentsOfLine(line, outSegs);
    }
    return outSegs;
}



Paths &CompoundRegion::containedSubpathsOfPath(Path &path, Paths &outPaths)
{
    SimpleRegions::iterator rit;
    for (rit = subregions.begin(); rit != subregions.end(); rit++) {
        rit->containedSubpathsOfPath(path, outPaths);
    }
    return outPaths;
}



Paths &CompoundRegion::infillPathsForRegionWithDensity(double density, double extrusionWidth, Paths &outPaths)
{
    SimpleRegions::iterator rit;
    for (rit = subregions.begin(); rit != subregions.end(); rit++) {
        rit->infillPathsForRegionWithDensity(density, extrusionWidth, outPaths);
    }
    return outPaths;
}



}

