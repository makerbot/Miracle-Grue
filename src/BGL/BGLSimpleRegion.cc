//
//  BGLRegion.m
//  Part of the Belfry Geometry Library
//
//  Created by GM on 10/14/10.
//  Copyright 2010 Belfry Software. All rights reserved.
//

#include "BGLCommon.h"
#include "BGLBounds.h"
#include "BGLPoint.h"
#include "BGLLine.h"
#include "BGLPath.h"
#include "BGLSimpleRegion.h"

using namespace std;

namespace BGL {


// Compound assignment operators
SimpleRegion& SimpleRegion::operator+=(const Point &rhs) {
    Paths::iterator it;
    for (it = subpaths.begin(); it != subpaths.end(); it++) {
	*it += rhs;
    }
    outerPath += rhs;
    return *this;
}



SimpleRegion& SimpleRegion::operator-=(const Point &rhs) {
    Paths::iterator it;
    for (it = subpaths.begin(); it != subpaths.end(); it++) {
	*it -= rhs;
    }
    outerPath -= rhs;
    return *this;
}



SimpleRegion& SimpleRegion::operator*=(double rhs) {
    Paths::iterator it;
    for (it = subpaths.begin(); it != subpaths.end(); it++) {
	*it *= rhs;
    }
    outerPath *= rhs;
    return *this;
}



SimpleRegion& SimpleRegion::operator*=(const Point &rhs) {
    Paths::iterator it;
    for (it = subpaths.begin(); it != subpaths.end(); it++) {
	*it *= rhs;
    }
    outerPath *= rhs;
    return *this;
}



SimpleRegion& SimpleRegion::operator/=(double rhs) {
    Paths::iterator it;
    for (it = subpaths.begin(); it != subpaths.end(); it++) {
	*it /= rhs;
    }
    outerPath /= rhs;
    return *this;
}



SimpleRegion& SimpleRegion::operator/=(const Point &rhs) {
    Paths::iterator it;
    for (it = subpaths.begin(); it != subpaths.end(); it++) {
	*it /= rhs;
    }
    outerPath /= rhs;
    return *this;
}




int SimpleRegion::size()
{
    return subpaths.size();
}



bool SimpleRegion::contains(const Point &pt) const
{
    int count = 0;
    if (outerPath.contains(pt)) {
        count++;
    }
    Paths::const_iterator it1;
    for (it1 = subpaths.begin(); it1 != subpaths.end(); it1++) {
	if (it1->contains(pt)) {
	    count++;
	}
    }
    return ((count & 0x1) == 0x1);
}



bool SimpleRegion::intersects(const Path &path) const
{
    if (path.contains(outerPath.startPoint())) {
	return true;
    }
    if (contains(path.startPoint())) {
	return true;
    }
    Paths::const_iterator it1;
    for (it1 = subpaths.begin(); it1 != subpaths.end(); it1++) {
	if (it1->intersects(path)) {
	    return true;
	}
    }
    return false;
}



bool SimpleRegion::intersects(const SimpleRegion &reg) const
{
    Paths::const_iterator pit1;
    Paths::const_iterator pit2;
    if (contains(reg.outerPath.startPoint())) {
	return true;
    }
    if (reg.contains(outerPath.startPoint())) {
	return true;
    }
    if (outerPath.intersects(reg.outerPath)) {
	return true;
    }
    for (pit1 = subpaths.begin(); pit1 != subpaths.end(); pit1++) {
	if (pit1->intersects(reg.outerPath)) {
	    return true;
	}
    }
    for (pit2 = reg.subpaths.begin(); pit2 != reg.subpaths.end(); pit2++) {
	if (pit2->intersects(outerPath)) {
	    return true;
	}
    }
    for (pit1 = subpaths.begin(); pit1 != subpaths.end(); pit1++) {
	for (pit2 = reg.subpaths.begin(); pit2 != reg.subpaths.end(); pit2++) {
	    if (pit1->intersects(*pit2)) {
		return true;
	    }
	}
    }
    return false;
}



string SimpleRegion::svgPathWithOffset(double dx, double dy)
{
    string out;
    out.append(outerPath.svgPathWithOffset(dx, dy));
    Paths::iterator pit = subpaths.begin();
    for ( ; pit != subpaths.end(); pit++) {
	out.append(" ");
        out.append(pit->svgPathWithOffset(dx, dy));
    }
    return out;
}



ostream &SimpleRegion::svgPathDataWithOffset(ostream& os, double dx, double dy) const
{
    outerPath.svgPathDataWithOffset(os, dx, dy);
    Paths::const_iterator pit;
    for (pit = subpaths.begin(); pit != subpaths.end(); pit++) {
	os << " ";
	pit->svgPathDataWithOffset(os, dx, dy);
    }
    return os;
}


ostream &SimpleRegion::svgPathWithOffset(ostream& os, double dx, double dy) const
{
    os << "<path fill=\"none\" d=\"";
    svgPathDataWithOffset(os, dx, dy);
    os << "\" />" << endl;
    return os;
}





void SimpleRegion::simplify(double minErr)
{
    Paths::iterator it;
    for (it = subpaths.begin(); it != subpaths.end(); it++) {
	it->simplify(minErr);
    }
    outerPath.simplify(minErr);
}



SimpleRegions &SimpleRegion::assembleSimpleRegionsFrom(Paths &paths, SimpleRegions &outRegs)
{
    Paths::iterator it1;
    Paths::iterator it2;

    for (it1 = paths.begin(); it1 != paths.end(); it1++) {
	it1->flags = 0;
	it1->quantize(CLOSEENOUGH/2.0);
	it1->simplify(0.05);
    }
    int count1, count2;
    for (count1 = 0, it1 = paths.begin(); it1 != paths.end(); count1++, it1++) {
	for (count2 = 0, it2 = paths.begin(); it2 != paths.end(); count2++, it2++) {
	    if (it1 != it2) {
	        if (it2->contains(it1->startPoint())) {
		    it1->flags++;
		}
	    }
	}
    }
    for (it1 = paths.begin(); it1 != paths.end(); it1++) {
        if ((it1->flags & 0x1) == 0) {
	    // Even contained count means outerpath.
	    outRegs.push_back(SimpleRegion(*it1));
	}
    }
    SimpleRegions::iterator rit;
    for (it1 = paths.begin(); it1 != paths.end(); it1++) {
	if ((it1->flags & 0x1) == 1) {
	    // Odd contained count means innerpath.
	    for (rit = outRegs.begin(); rit != outRegs.end(); rit++) {
		if ((rit->outerPath.flags | 0x1) == (it1->flags | 0x1)) {
		    // Same level of containedness.
		    if (rit->outerPath.contains(it1->startPoint())) {
		        rit->subpaths.push_back(*it1);
		    }
		}
	    }
	}
    }
    return outRegs;
}



SimpleRegions &SimpleRegion::assembleSimpleRegionsFrom(const Paths &outerPaths, const Paths &innerPaths, SimpleRegions &outRegs)
{
    Paths tempPaths(outerPaths);

    Paths::const_iterator it1;
    for (it1 = innerPaths.begin(); it1 != innerPaths.end(); it1++) {
	tempPaths.push_back(*it1);
    }
    return assembleSimpleRegionsFrom(tempPaths, outRegs);
}




SimpleRegions &SimpleRegion::unionOf(SimpleRegion &r1, SimpleRegion &r2, SimpleRegions &outRegs)
{
    Paths outerPaths;
    Path::unionOf(r1.outerPath, r2.outerPath, outerPaths);

    Paths::iterator it1;
    Paths::iterator it2;

    Paths newInnerPaths;
    for (it1 = r1.subpaths.begin(); it1 != r1.subpaths.end(); it1++) {
	for (it2 = r2.subpaths.begin(); it2 != r2.subpaths.end(); it2++) {
	    Path::intersectionOf(*it1, *it2, newInnerPaths);
	}
    }
    for (it1 = r1.subpaths.begin(); it1 != r1.subpaths.end(); it1++) {
	Path::differenceOf(*it1, r2.outerPath, newInnerPaths);
    }
    for (it2 = r2.subpaths.begin(); it2 != r2.subpaths.end(); it2++) {
	Path::differenceOf(*it2, r1.outerPath, newInnerPaths);
    }

    Paths innerPaths;
    Path::unionOf(newInnerPaths, innerPaths);
    assembleSimpleRegionsFrom(outerPaths, innerPaths, outRegs);

    return outRegs;
}



SimpleRegions &SimpleRegion::differenceOf(SimpleRegion &r1, SimpleRegion &r2, SimpleRegions &outRegs)
{
    Paths outerPaths;
    Path::differenceOf(r1.outerPath, r2.outerPath, outerPaths);

    Paths::iterator it1;
    Paths::iterator it2;

    Paths newInnerPaths;
    for (it2 = r2.subpaths.begin(); it2 != r2.subpaths.end(); it2++) {
	Path::intersectionOf(*it2, r1.outerPath, newInnerPaths);
    }
    for (it2 = r1.subpaths.begin(); it2 != r1.subpaths.end(); it2++) {
        Paths tempPaths;
	for (it1 = newInnerPaths.begin(); it1 != newInnerPaths.end(); it1++) {
	    Path::differenceOf(*it1, *it2, tempPaths);
	}
	newInnerPaths = tempPaths;
    }
    for (it2 = r1.subpaths.begin(); it2 != r1.subpaths.end(); it2++) {
        Paths tempPaths;
	for (it1 = outerPaths.begin(); it1 != outerPaths.end(); it1++) {
	    Path::differenceOf(*it1, *it2, tempPaths);
	}
	outerPaths = tempPaths;
    }

    Paths innerPaths;
    Path::unionOf(newInnerPaths, innerPaths);
    assembleSimpleRegionsFrom(outerPaths, innerPaths, outRegs);

    return outRegs;
}



/*
 * Intersection:
 *   new.outerPaths = r1.outerPath.intersect(r2.outerPath)
 *   diff r1.outerPath from each r2.innerpath
 *   diff r2.outerPath from each r1.innerpath
 *   unionize all innerPaths
 *   foreach p1 innerPaths {
 *     if (p1 intersects a path in new.outerPaths) {
 *       diff p1 from new.outerPaths
 *     } else {
 *       add p1 to new.innerPaths
 *     }
 *   }
 *   sort new innerpaths into SimpleRegions with new outerPaths
 */
SimpleRegions &SimpleRegion::intersectionOf(SimpleRegion &r1, SimpleRegion &r2, SimpleRegions &outRegs)
{
    Paths outerPaths;
    Path::intersectionOf(r1.outerPath, r2.outerPath, outerPaths);

    Paths::iterator it1;
    Paths::iterator it2;

    for (it1 = r1.subpaths.begin(); it1 != r1.subpaths.end(); it1++) {
	Paths tempPaths;
	for (it2 = outerPaths.begin(); it2 != outerPaths.end(); it2++) {
	    Path::differenceOf(*it2, *it1, tempPaths);
	}
	outerPaths = tempPaths;
    }

    for (it1 = r2.subpaths.begin(); it1 != r2.subpaths.end(); it1++) {
	Paths tempPaths;
	for (it2 = outerPaths.begin(); it2 != outerPaths.end(); it2++) {
	    Path::differenceOf(*it2, *it1, tempPaths);
	}
	outerPaths = tempPaths;
    }

    bool found;
    do {
	found = false;
	for (it1 = outerPaths.begin(); !found && it1 != outerPaths.end(); it1++) {
	    for (it2 = it1; !found && it2 != outerPaths.end(); it2++) {
		if (it1 != it2) {
		    if (it1->intersects(*it2)) {
			Paths tempPaths;
			Path::unionOf(*it1, *it2, tempPaths);
			if (tempPaths.size() < 2) {
			    Paths::iterator it3;
			    for (it3 = tempPaths.begin(); it3 != tempPaths.end(); it3++) {
				outerPaths.push_back(*it3);
			    }
			    it2 = outerPaths.erase(it2);
			    it1 = outerPaths.erase(it1);
			    found = true;
			}
		    }
		}
	    }
	}
    } while (found);

    assembleSimpleRegionsFrom(outerPaths, outRegs);
    return outRegs;
}



SimpleRegions &SimpleRegion::inset(double offsetby, SimpleRegions& outRegs)
{
    Paths outerPaths;
    outerPath.inset(offsetby, outerPaths);

    Paths innerPaths;
    Paths::iterator it1;
    for (it1 = subpaths.begin(); it1 != subpaths.end(); it1++) {
	it1->inset(-offsetby, innerPaths);
    }

    Paths innerPaths2;
    Path::unionOf(innerPaths, innerPaths2);

    Paths newPaths;
    Path::differenceOf(outerPaths, innerPaths2, newPaths);

    assembleSimpleRegionsFrom(newPaths, outRegs);
    return outRegs;
}



Lines &SimpleRegion::containedSegmentsOfLine(Line &line, Lines &outSegs)
{
    Path newpath;
    newpath.segments.push_back(line);
    newpath.splitSegmentsAtIntersectionsWithPath(outerPath);

    Paths::iterator it;
    for (it = subpaths.begin(); it != subpaths.end(); it++) {
	newpath.splitSegmentsAtIntersectionsWithPath(*it);
    }

    Lines::iterator lit;
    for (lit = newpath.segments.begin(); lit != newpath.segments.end(); lit++) {
        if (contains((lit->startPt + lit->endPt)/2.0)) {
	    // Now inside
	    outSegs.push_back(*lit);
	}
    }
    return outSegs;
}



Paths &SimpleRegion::containedSubpathsOfPath(const Path &path, Paths &outPaths)
{
    Path newpath(path);
    newpath.splitSegmentsAtIntersectionsWithPath(outerPath);

    Paths::iterator it;
    for (it = subpaths.begin(); it != subpaths.end(); it++) {
	newpath.splitSegmentsAtIntersectionsWithPath(*it);
    }

    Lines::iterator lit;
    bool wasOut = true;
    Path tempPath;
    for (lit = newpath.segments.begin(); lit != newpath.segments.end(); lit++) {
        if (contains((lit->startPt + lit->endPt)/2.0)) {
	    // Now inside
	    tempPath.segments.push_back(*lit);
	    wasOut = false;
	} else {
	    // Now outside
	    if (!wasOut) {
	        outPaths.push_back(tempPath);
		tempPath.segments.clear();
		tempPath.flags = INSIDE;
	    }
	    wasOut = true;
	}
    }
    if (tempPath.size() > 1) {
	outPaths.push_back(tempPath);
    }
    return outPaths;
}



Paths &SimpleRegion::infillPathsForRegionWithDensity(double density, double extrusionWidth, Paths &outPaths)
{
    Bounds bounds = outerPath.bounds();
    if (bounds.minX == Bounds::NONE) {
        return outPaths;
    }
    if (density <= 0.001f) {
        return outPaths;
    }
    
    // D = WSsqrt2/SS
    // D = Wsqrt2/S
    // DS = Wsqrt2
    // S = Wsqrt2/D
    double spacing = extrusionWidth*sqrt(2.0f)/density;
    if (density >= 0.99f) {
        spacing = extrusionWidth;
    }
    double zag = spacing;
    
    bool alternate = (((int)floor(bounds.minX/spacing-1)) & 0x1) == 0;
    for (double fillx = floor(bounds.minX/spacing-1)*spacing; fillx < bounds.maxX+spacing; fillx += spacing) {
        alternate = !alternate;
	Path path;
        double zig = 0.0f;
        if (density < 0.99f) {
            zig = 0.5f*zag;
            if (alternate) {
                zig = -zig;
            }
        }
        for (double filly = floor(0.5*bounds.minY/zag-1)*2.0f*zag; filly < bounds.maxY+zag; filly += zag) {
            path.segments.push_back(Line(Point(fillx+zig,filly),Point(fillx-zig,filly+zag)));
            zig = -zig;
        }
        Paths infillPaths;
	containedSubpathsOfPath(path, outPaths);
    }
    return outPaths;
}


}

