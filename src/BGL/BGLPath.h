//
//  BGLPath.h
//  Part of the Belfry Geometry Library
//
//  Created by GM on 10/13/10.
//  Copyright 2010 Belfry Software. All rights reserved.
//

#ifndef BGL_PATH_H
#define BGL_PATH_H

#include <list>
#include "config.h"
#include "BGLCommon.h"
#include "BGLAffine.h"
#include "BGLBounds.h"
#include "BGLIntersection.h"
#include "BGLPoint.h"
#include "BGLLine.h"



namespace BGL {

class Path;
typedef std::list<Path> Paths;

class Path {
public:
    int flags;
    Lines segments;

    // Constructors
    Path() : flags(0), segments() {}
    Path(const Lines& x) : flags(0), segments(x) {}
    Path(const Path& x) : flags(x.flags), segments(x.segments) {}

    // Assignment operator
    Path& operator=(const Path &rhs) {
	if (this != &rhs) {
	    flags = rhs.flags;
	    segments = rhs.segments;
	}
	return *this;
    }

    // Comparison operators
    bool operator==(const Path &rhs) const;
    bool operator!=(const Path &rhs) const {
        return !(*this == rhs);
    }

    const Point startPoint() const {
        return segments.front().startPt;
    }
    const Point endPoint() const {
        return segments.back().endPt;
    }
    bool isClosed() const {
	if (size() == 0) {
	    return false;
	}
        return (startPoint() == endPoint());
    }
    int size() const {
        return segments.size();
    }
    bool hasEndPoint(const Point& pt) const {
        return (pt == startPoint() || pt == endPoint());
    }
    Scalar length() const;
    Scalar area() const;
    Scalar windingArea() const;
    bool isClockwise() const;
    Bounds bounds() const;

    bool couldAttach(const Line& ln) const;
    bool couldAttach(const Path& path) const;
    bool attach(const Line& ln);
    bool attach(const Path& path);

    std::string svgPathWithOffset(Scalar dx, Scalar dy) const;

    bool intersects(const Line &ln) const;
    bool intersects(const Path &path) const;
    Intersections &intersectionsWith(const Line &ln, Intersections &outISects) const;

    bool hasEdgeWithPoint(const Point &pt) const;
    bool contains(const Point &pt) const;

    Lines::const_iterator begin() const { return segments.begin(); }
    Lines::const_reverse_iterator rbegin() const { return segments.rbegin(); }
    Lines::const_iterator end() const { return segments.begin(); }
    Lines::const_reverse_iterator rend() const { return segments.rend(); }

    Lines::iterator begin() { return segments.begin(); }
    Lines::reverse_iterator rbegin() { return segments.rbegin(); }
    Lines::iterator end() { return segments.begin(); }
    Lines::reverse_iterator rend() { return segments.rend(); }

    // Strips out segments that are shorter than the given length.
    void stripSegmentsShorterThan(Scalar minlen);
    void splitSegmentsAtIntersectionsWithPath(const Path &path);

    void untag();
    void tagSegmentsRelativeToClosedPath(const Path &path);

    static Paths &assemblePathsFromSegments(const Lines &segs, Paths &outPaths);
    static Paths &repairUnclosedPaths(const Paths &paths, Paths &outPaths);
    static Paths &assembleTaggedPaths(Path &path1, uint32_t flags1, Path &path2, uint32_t flags2, Paths &outPaths);

    static Paths &differenceOf  (Path &path1, Path &path2, Paths &outPaths);
    static Paths &unionOf       (Path &path1, Path &path2, Paths &outPaths);
    static Paths &intersectionOf(Path &path1, Path &path2, Paths &outPaths);

    static Paths &unionOf       (Paths &paths, Paths &outPaths);
    static Paths &differenceOf  (Paths &paths1, Paths &paths2, Paths &outPaths);

    Lines &containedSegments(const Line &line, Lines &outSegs) const;
    Paths &containedSubpathsOfPath(Path &path, Paths outPaths) const;

    // Friend functions
    friend std::ostream& operator <<(std::ostream &os,const Path &pt);

};



}

#endif

