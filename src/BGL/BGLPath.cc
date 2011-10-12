//
//  BGLPath.cc
//  Part of the Belfry Geometry Library
//
//  Created by GM on 10/13/10.
//  Copyright 2010 Belfry Software. All rights reserved.
//

#include "BGLPath.h"
#include <stdio.h>

using namespace std;
using namespace BGL;

namespace BGL {


// Comparison operators
bool Path::operator==(const Path &rhs) const
{
    if (size() != rhs.size()) {
	return false;
    }
    Lines::const_iterator itera = segments.begin();
    Lines::const_iterator iterb = rhs.segments.begin();
    while(itera != segments.end() && iterb != rhs.segments.end()) {
	if (*itera != *iterb) {
	    return false;
	}
	itera++, iterb++;
    }
    return true;
}



float Path::length() const
{
    float totlen = 0.0f;
    Lines::const_iterator itera = segments.begin();
    for(; itera != segments.end(); itera++) {
	totlen += itera->length();
    }
    return totlen;
}



float Path::windingArea() const
{
    float totarea = 0.0f;
    Lines::const_iterator itera = segments.begin();
    for(; itera != segments.end(); itera++) {
	totarea += itera->startPt.x * itera->endPt.y;
	totarea -= itera->endPt.x * itera->startPt.y;
    }
    return (totarea/2.0f);
}



bool Path::isClockwise() const
{
    return (windingArea() > 0);
}



float Path::area() const
{
    return fabs(windingArea());
}



Bounds Path::bounds() const
{
    Bounds bnds;
    Lines::const_iterator itera = segments.begin();
    for (; itera != segments.end(); itera++) {
        bnds.expand(itera->startPt);
        bnds.expand(itera->endPt);
    }
    return bnds;
}



bool Path::couldAttach(const Line& ln) const
{
    if (size() > 0) return true;
    if (hasEndPoint(ln.startPt)) return true;
    if (hasEndPoint(ln.endPt)) return true;
    return false;
}



bool Path::couldAttach(const Path& path) const
{
    if (size() <= 0) return true;
    if (hasEndPoint(path.startPoint())) return true;
    if (hasEndPoint(path.endPoint())) return true;
    return false;
}



bool Path::attach(const Line& ln)
{
    if (size() <= 0) {
	segments.push_back(ln);
	return true;
    }
    if (endPoint() == ln.startPt) {
	segments.push_back(ln);
	return true;
    }
    if (startPoint() == ln.endPt) {
	segments.push_front(ln);
	return true;
    }
    if (endPoint() == ln.endPt) {
	Line lnrev(ln);
	lnrev.reverse();
	segments.push_back(lnrev);
	return true;
    }
    if (startPoint() == ln.startPt) {
	Line lnrev(ln);
	lnrev.reverse();
	segments.push_front(lnrev);
	return true;
    }
    return false;
}



bool Path::attach(const Path& path)
{
    if (couldAttach(path)) {
	Lines::const_iterator itera = path.segments.begin();
	for(; itera != path.segments.end(); itera++) {
	    attach(*itera);
	}
	return true;
    }
    return false;
}



string Path::svgPathWithOffset(float dx, float dy) const
{
    char buf[80];
    string out;
    if (size() == 0) {
	return out;
    }
    Line prev;
    float mult = 90.0f / 25.4f;
    Lines::const_iterator itera = segments.begin();
    bool isfirst = true;
    for (; itera != segments.end(); itera++) {
	if (!isfirst) {
	    out.append(" ");
	}
	if (isfirst || prev.endPt != itera->startPt) {
	    snprintf(buf,sizeof(buf),"M%.3f,%.3f ",
		     (itera->startPt.x+dx)*mult,
		     (itera->startPt.y+dy)*mult);
	    out.append(buf);
	    isfirst = false;
	}
	snprintf(buf,sizeof(buf),"L%.3f,%.3f",
		 (itera->endPt.x+dx)*mult,
		 (itera->endPt.y+dy)*mult);
	out.append(buf);
	prev = *itera;
    }
    return out;
}




bool Path::intersects(const Line &ln) const
{
    Lines::const_iterator itera = segments.begin();
    for (; itera != segments.end(); itera++) {
	Intersection isect = itera->intersectionWithSegment(ln);
	if (isect.type != NONE) {
	    return true;
	}
    }
    return false;
}



bool Path::intersects(const Path &path) const
{
    Lines::const_iterator itera = segments.begin();
    for (; itera != segments.end(); itera++) {
	Lines::const_iterator iterb = path.segments.begin();
	for (; iterb != path.segments.end(); iterb++) {
	    Intersection isect = itera->intersectionWithSegment(*iterb);
	    if (isect.type != NONE) {
		return true;
	    }
	}
    }
    return false;
}



Intersections &Path::intersectionsWith(const Line &ln, Intersections &outISects) const
{
    int segnum = 0;
    bool isclosed = isClosed();
    Lines::const_iterator itera = segments.begin();
    for (; itera != segments.end(); itera++) {
	Intersection isect = itera->intersectionWithSegment(ln);
	// Ignore point intersections with the startpoint of a segment.
	// It should have already been caught as the endpoint of the
	//  previous segment.
	if (isect.type != NONE) {
	    if (isect.type != POINT ||
		isect.p1 != itera->startPt ||
		(segnum == 0 && !isclosed)
	    ) {
		isect.segment = segnum;
		outISects.push_back(isect);
	    }
	}
	segnum++;
    }
    return outISects;
}



bool Path::hasEdgeWithPoint(const Point &pt) const
{
    Lines::const_iterator itera = segments.begin();
    for (; itera != segments.end(); itera++) {
	if (itera->isLinearWith(pt)) {
	    return true;
	}
    }
    return false;
}



bool Path::contains(const Point &pt) const
{
    if (!isClosed()) {
	return false;
    }
    Line longLine(pt,Point(1.0e9,pt.y));
    Line testLine;
    Point& sp = testLine.startPt;
    Point& ep = testLine.endPt;
    int icount = 0;
    Lines::const_iterator itera = segments.begin();
    for (; itera != segments.end(); itera++) {
	sp = itera->startPt;
	if (sp.y == pt.y) {
	    sp.y += 10*EPSILON;
	}
	ep = itera->endPt;
	if (ep.y == pt.y) {
	    ep.y += 10*EPSILON;
	}
	Intersection isect = testLine.intersectionWithSegment(longLine);
	if (isect.type != NONE) {
	    icount++;
	}
    }
    return ((icount & 0x1) != 0);
}


// Strips out segments that are shorter than the given length.
void Path::stripSegmentsShorterThan(float minlen)
{
    Lines::iterator itera = segments.begin();
    while (itera != segments.end()) {
	if (itera->length() < minlen) {
	    Line ln = *itera;
	    itera = segments.erase(itera);
	    if (itera == segments.end()) {
		if (segments.size() > 0) {
		    segments.back().endPt = ln.endPt;
		}
	    } else {
		itera->startPt = ln.startPt;
	    }
	} else {
	    itera++;
	}
    }
}



Paths &Path::assemblePathsFromSegments(const Lines &segs, Paths &outPaths)
{
    Lines unhandled(segs);
    Path currPath;
    bool foundLink = false;
    while (unhandled.size() > 0) {
	if (currPath.size() == 0) {
	    Line ln = unhandled.front();
	    currPath.attach(ln);
	    unhandled.pop_front();
	}
	foundLink = false;
	Lines::iterator itera = unhandled.begin();
	while (itera != unhandled.end()) {
	    if (currPath.attach(*itera)) {
		itera = unhandled.erase(itera);
		foundLink = true;
	    } else {
		itera++;
	    }
	}
	if (!foundLink || unhandled.size() == 0) {
	    outPaths.push_back(currPath);
	    currPath = Path();
	}
    }
    return outPaths;
}



Paths &Path::repairUnclosedPaths(const Paths &paths, Paths &outPaths)
{
    Paths unhandled(paths);
    
    // filter out all completed paths.
    Paths::iterator itera = unhandled.begin();
    while (itera != unhandled.end()) {
	if (itera->isClosed()) {
	    outPaths.push_back(*itera);
	    itera = unhandled.erase(itera);
	} else {
	    itera++;
	}
    }
    
    // Now we just have incomplete paths left.
    while (unhandled.size() > 0) {
	Path path = unhandled.front();
	unhandled.pop_front();
	for (;;) {
	    // Find closest remaining incomplete path
	    Paths::iterator closestIter;
	    float closestDist = 9.0e9;
	    float closingDist = path.startPoint().distanceFrom(path.endPoint());
	    for (itera = unhandled.begin(); itera != unhandled.end(); itera++) {
		Path &path2 = *itera;
		float dist1 = path.endPoint().distanceFrom(path2.startPoint());
		float dist2 = path.endPoint().distanceFrom(path2.endPoint());
		if (dist1 < closestDist) {
		    closestDist = dist1;
		    closestIter = itera;
		}
		if (dist2 < closestDist) {
		    closestDist = dist2;
		    closestIter = itera;
		}
	    }
	    // If closest found incomplete path is closer than just closing the path, then attach it.
	    if (closestDist < closingDist) {
		Path &path2 = *closestIter;
		path.attach(Line(path.endPoint(),path2.startPoint()));
		path.attach(path2);
		closestIter = unhandled.erase(closestIter);
	    } else {
		// Closest found match is if we just close the path.
		if (path.size() < 2) {
		    break;
		}
		Line ln(path.endPoint(), path.startPoint());
		path.attach(ln);
	    }
	    if (path.isClosed()) {
		// Path has been closed.  On to the next path!
		outPaths.push_back(path);
		break;
	    }
	}
    }
    return outPaths;
}



void Path::splitSegmentsAtIntersectionsWithPath(const Path &path)
{
    Lines::iterator itera;
    for (itera = segments.begin(); itera != segments.end(); itera++) {
	Lines::const_iterator iterb;
	for (iterb = path.segments.begin(); iterb != path.segments.end(); iterb++) {
	    Intersection isect = itera->intersectionWithSegment(*iterb);
	    if (isect.type != NONE) {
		if (!itera->hasEndPoint(isect.p1)) {
		    Point tempPt = itera->startPt;
		    itera->startPt = isect.p1;
		    itera = segments.insert(itera, Line(tempPt, isect.p1));
		}
		if (isect.type == SEGMENT) {
		    if (!itera->hasEndPoint(isect.p2) && itera->contains(isect.p2)) {
			Point tempPt = itera->startPt;
			itera->startPt = isect.p2;
			itera = segments.insert(itera, Line(tempPt, isect.p2));
		    }
		}
	    }
	}
    }
    stripSegmentsShorterThan(CLOSEENOUGH);
}



void Path::untag()
{
    flags = OUTSIDE;
    Lines::iterator itera = segments.begin();
    for (; itera != segments.end(); itera++) {
	itera->flags = USED;
    }
}



void Path::tagSegmentsRelativeToClosedPath(const Path &path)
{
    bool invert = (flags == INSIDE);
    splitSegmentsAtIntersectionsWithPath(path);
    Point midpt;
    Lines::iterator itera = segments.begin();
    for (; itera != segments.end(); itera++) {
	Line &seg = *itera;
	midpt.x = (seg.startPt.x + seg.endPt.x) / 2.0f;
	midpt.y = (seg.startPt.y + seg.endPt.y) / 2.0f;
	if (path.hasEdgeWithPoint(midpt)) {
	    // Either shared or unshared segment.
	    // Test a point very slightly to the side, to see
	    //  if the inside of both paths is on the same side.
	    if (fabsf(seg.endPt.x-seg.startPt.x) > fabsf(seg.endPt.y-seg.startPt.y)) {
		midpt.x += 2.0f*CLOSEENOUGH;
	    } else {
		midpt.y += 2.0f*CLOSEENOUGH;
	    }
	    bool isInPath = path.contains(midpt) && !invert;
	    bool isInSelf = contains(midpt);
	    if (isInPath == isInSelf) {
		// tweak sharedness, for use in checking against multiple paths.
		switch (seg.flags) {
		    case USED:
		    case OUTSIDE:
		    case SHARED:
		    case UNSHARED:
			seg.flags = SHARED;
			break;
		    case INSIDE:
			seg.flags = UNSHARED;
			break;
		}
	    } else {
		// tweak unsharedness, for use in checking against multiple paths.
		switch (seg.flags) {
		    case USED:
		    case OUTSIDE:
		    case UNSHARED:
			seg.flags = UNSHARED;
			break;
		    case SHARED:
		    case INSIDE:
			seg.flags = SHARED;
			break;
		}
	    }
	} else if (path.contains(midpt) == !invert) {
	    // toggle insideness, for use in checking against multiple paths.
	    switch (seg.flags) {
		case USED:
		    seg.flags = INSIDE;
		    break;
		case INSIDE:
		    seg.flags = OUTSIDE;
		    break;
		case OUTSIDE:
		    seg.flags = INSIDE;
		    break;
		case SHARED:
		    seg.flags = UNSHARED;
		    break;
		case UNSHARED:
		    seg.flags = SHARED;
		    break;
	    }
	} else {
	    if (seg.flags == USED) {
		seg.flags = OUTSIDE;
	    }
	}
    }
}



Paths& Path::assembleTaggedPaths(Path &path1, uint32_t flags1, Path &path2, uint32_t flags2, Paths &outPaths)
{
    uint32_t remaining = path1.size() + path2.size();
    
    // Tag segments for insideness, outsideness, or sharedness.
    path1.untag();
    path1.tagSegmentsRelativeToClosedPath(path2);
    path2.untag();
    path2.tagSegmentsRelativeToClosedPath(path1);
    
    // Mark all unwanted segments in path1 as used.
    Lines::iterator itera = path1.segments.begin();
    for (; itera != path1.segments.end(); itera++) {
	if ((itera->flags & flags1) == 0) {
	    itera->flags = USED;
	    remaining--;
	}
    }
    
    // Mark all unwanted segments in path2 as used.
    Lines::iterator iterb = path2.segments.begin();
    for (; iterb != path2.segments.end(); iterb++) {
	if ((iterb->flags & flags1) == 0) {
	    iterb->flags = USED;
	    remaining--;
	}
    }
    
    // Try assembling path from unused segments.
    Lines::iterator currseg;
    Lines::iterator otherseg;
    currseg = path1.segments.begin();
    otherseg = path2.segments.begin();
    int32_t pathLimit = 0;
    Path* patha = &path1;
    Path* pathb = &path2;

    outPaths.push_back(Path());
    Path* outPath = &outPaths.back();
    while (remaining > 0) {
	Line &seg = *currseg;
	if (seg.flags != USED && outPath->couldAttach(seg)) {
	    // Found a connected unused segment.
	    // Attach it to the current path.
	    seg.flags = USED;
	    outPath->attach(seg);
	    remaining--;
	    pathLimit = 0;
	    currseg++;
	    if (currseg == patha->segments.end()) {
	        currseg = patha->segments.begin();
	    }
	    
	    // If path was closed by this segment, remember it and start a new path.
	    if (outPath->isClosed()) {
		outPaths.push_back(Path());
		outPath = &outPaths.back();
		pathLimit = 0;
	    }
	} else {
	    pathLimit++;
	    
	    // Swap over to the other path.
	    Path* tmppath = patha;
	    patha = pathb;
	    pathb = tmppath;
	    
	    Lines::iterator tmpseg = currseg;
	    currseg = otherseg;
	    otherseg = tmpseg;
	    
	    // Find a connected unused segment in the new path.
	    // Stop looking if we completely circumnavigate the path.
	    int32_t limit;
	    for (limit = patha->size(); limit >= 0; limit--) {
		currseg++;
		if (currseg == patha->segments.end()) {
		    currseg = patha->segments.begin();
		}
		if (currseg->flags != USED && outPath->couldAttach(*currseg)) {
		    break;
		}
	    }
	    
	    if (limit == 0 && remaining > 0 && pathLimit == 2) {
		// Failed to find another connected segment in either path.
		// Remember this path, and start a new one.
		outPaths.push_back(Path());
		outPath = &outPaths.back();
		pathLimit = 0;
	    }
	}
    }
    if (outPath->size() == 0) {
	// Remember the final path.
	outPaths.pop_back();
    }
    
    return outPaths;
}



Paths &Path::differenceOf(Path &path1, Path &path2, Paths &outPaths)
{
    return assembleTaggedPaths(path1, INSIDE, path2, (OUTSIDE|UNSHARED), outPaths);
}



Paths &Path::unionOf(Path &path1, Path &path2, Paths &outPaths)
{
    return assembleTaggedPaths(path1, (OUTSIDE|SHARED), path2, OUTSIDE, outPaths);
}



Paths &Path::intersectionOf(Path &path1, Path &path2, Paths &outPaths)
{
    return assembleTaggedPaths(path1, (INSIDE|SHARED), path2, INSIDE, outPaths);
}



Paths &Path::unionOf(Paths &paths, Paths &outPaths)
{
    Paths::iterator it1;
    Paths::iterator it2;
    Paths::iterator it3;

    for (it1 = paths.begin(); it1 != paths.end(); it1++) {
	outPaths.push_back(*it1);
    }
    
    for (it1 = outPaths.begin(); it1 != outPaths.end(); ) {
	bool found = false;
	for (it2 = it1; it2 != outPaths.end(); it2++) {
	    if (it1 != it2) {
		Paths tempPaths;
	        Path::unionOf(*it1, *it2, tempPaths);
		if (tempPaths.size() < 2) {
		    for (it3 = tempPaths.begin(); it3 != tempPaths.end(); it3++) {
		        outPaths.push_back(*it3);
		    }
		    it2 = outPaths.erase(it2);
		    it1 = outPaths.erase(it1);
		    found = true;
		    break;
		}
	    }
	}
	if (!found) {
	    it1++;
	}
    }
    return outPaths;
}



Paths &Path::differenceOf  (Paths &paths1, Paths &paths2, Paths &outPaths)
{
    Paths::iterator it1;
    Paths::iterator it2;

    outPaths = paths1;
    for (it2 = paths2.begin(); it2 != paths2.end(); it2++) {
	Paths tempPaths;
	for (it1 = outPaths.begin(); it1 != outPaths.end(); it1++) {
	    Path::differenceOf(*it1, *it2, tempPaths);
	}
	outPaths = tempPaths;
    }
    return outPaths;
}



Lines &Path::containedSegments(const Line &line, Lines &outSegs) const
{
    Path linePath;
    linePath.attach(line);
    linePath.untag();
    linePath.tagSegmentsRelativeToClosedPath(*this);
    
    Lines::iterator itera = linePath.segments.begin();
    for (; itera != linePath.segments.end(); itera++) {
	Line &seg = *itera;
	if (seg.flags == INSIDE || seg.flags == SHARED || seg.flags == UNSHARED) {
	    outSegs.push_back(seg);
	}
    }
    return outSegs;
}



Paths &Path::containedSubpathsOfPath(Path &path, Paths outPaths) const
{
    path.untag();
    path.tagSegmentsRelativeToClosedPath(*this);
    
    Lines outSegs;
    Lines::iterator itera = path.segments.begin();
    for (; itera != path.segments.end(); itera++) {
	Line &seg = *itera;
	if (seg.flags == INSIDE || seg.flags == SHARED || seg.flags == UNSHARED) {
	    outSegs.push_back(seg);
	}
    }
    
    return assemblePathsFromSegments(outSegs, outPaths);
}



ostream& operator <<(ostream &os, const Path &path)
{
    os << "{";
    Lines::const_iterator it = path.segments.begin();
    for ( ; it != path.segments.end(); it++) {
	os << *it;
    }
    os << "}";
    return os;
}


}

