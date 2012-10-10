/*

   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

 */


#include <map>
#include <set>


#include "shrinky.h"
#include "log.h"

namespace mgl {

using namespace std;


void lengthCheck(const std::vector<Segment2Type> &segments, const char *msg) {
    for (unsigned int i = 0; i < segments.size(); i++) {
        const Segment2Type &seg = segments[i];
        Scalar l = seg.length();
        // Log::often() << msg << " seg[" << i << "] = " << seg << " l=" << l << endl;
        if (!(l > 0)) {
            Log::info() << "Z";
            stringstream ss;
            ss << msg << " Zero length: segment[" << i << "] = " << seg << endl;
            ScadDebugFile::segment3(ss, "", "segments", segments, 0, 0.1);
            ShrinkyException mixup(ss.str().c_str());
            throw mixup;
        }
    }
}

void connectivityCheck(const std::vector<Segment2Type> &segments,
        Scalar tol) {

    for (unsigned int i = 0; i < segments.size(); i++) {
        unsigned int prevId = i == 0 ? segments.size() - 1 : i - 1;
        const Segment2Type &prevSeg = segments[prevId];
        Segment2Type seg = segments[i];

        if (!prevSeg.b.tequals(seg.a, tol)) {
            Point2Type dist = prevSeg.b - seg.a;
            stringstream ss;
            ss << "Connectivity error: segment[" << prevId << "] = " << prevSeg << endl;
            ss << " and segment[" << i << "] = " << seg << endl;
            ss << "	segment[" << prevId << "] length = " << prevSeg.length() << endl;
            ss << " segment[" << i << "] length = " << seg.length() << endl;
            ss << " Distance between segments " << dist.magnitude();

            ss << endl;
            Log::info() << "C";
            // Log::often() << "|" << dist.magnitude() << "|" << prevSeg.length() << "|" << seg.length() << "|";
            ScadDebugFile::segment3(ss, "", "segments", segments, 0, 0.1);
            ShrinkyException mixup(ss.str().c_str());
            throw mixup;

        }
    }
}

void createConvexList(const std::vector<Segment2Type> &segments, std::vector<bool> &convex) {
    Scalar tol = 0.3;

    for (size_t id = 0; id < segments.size(); id++) {
        size_t prevId = id == 0 ? segments.size() - 1 : id - 1;

        const Segment2Type &seg = segments[id];
        const Segment2Type &prevSeg = segments[prevId];

        const Point2Type & i = prevSeg.a;
        const Point2Type & j = seg.a;
        const Point2Type & j2 = prevSeg.b;
        const Point2Type & k = seg.b;
        bool isSameSame = j.tequals(j2, tol);

        if (!isSameSame) {
            stringstream ss;
            ss << endl << "CONNECTIVITY ERROR" << endl;
            ss << "Segment id: " << id << ", prevId: " << prevId << endl;
            ss << "i: " << i << endl;
            ss << "j: " << j << endl;
            ss << "j2: " << j2 << endl;
            ss << "k: " << k << endl;
            Point2Type d = j2 - j;
            Scalar distance = d.magnitude();
            ss << "distance " << distance << endl;
            ss << "SameSame " << isSameSame << endl;
            Log::info() << "_C_";
            ShrinkyException mixup(ss.str().c_str());
            throw mixup;

        }
        bool vertex = convexVertex(i, j, k);
        convex.push_back(vertex);
    }
}

void segmentsDiagnostic(const char* title, const std::vector<Segment2Type> &segments) {

    Log::info() << endl << title << endl;
    Log::info() << "id\tconvex\tlength\tdistance\tangle\ta, b" << endl;

    for (size_t id = 0; id < segments.size(); id++) {

        const Segment2Type &seg = segments[id];
        size_t prevId = id == 0 ? segments.size() - 1 : id - 1;

        const Segment2Type &prevSeg = segments[prevId];

        const Point2Type & i = prevSeg.a;
        const Point2Type & j = seg.a;
        const Point2Type & j2 = prevSeg.b;
        const Point2Type & k = seg.b;

        Point2Type d = j2 - j;
        Scalar distance = d.magnitude();
        Scalar length = seg.squaredLength();
        Scalar angle = d.angleFromPoint2s(i, j, k);
        bool vertex = convexVertex(i, j, k);

        Log::info() << id << "\t" << vertex << "\t" << length << ",\t" << distance << ",\t" << angle << "\t" << seg.a << ", " << seg.b << "\t" << endl;
    }
}

Point2Type getInsetDirection(const Segment2Type &seg) {
    Point3Type v(seg.b.x - seg.a.x, seg.b.y - seg.a.y, 0);
    Point3Type up(0, 0, 1);
    Point3Type inset = v.crossProduct(up);
    inset.normalise();
    Point2Type inset2(inset.x, inset.y);
    return inset2;
}

Segment2Type elongateAndPrelongate(const Segment2Type &s, Scalar dist) {
    Segment2Type segment(s);
    Point2Type l = segment.b - segment.a;
    l.normalise();
    l *= dist;
    segment.b += l;
    segment.a -= l;
    return segment;

}

void insetSegments(const std::vector<Segment2Type> &segments, Scalar d,
        std::vector<Segment2Type> &insets) {
    assert(insets.size() == 0);
    for (size_t i = 0; i < segments.size(); i++) {
        Segment2Type seg = segments[i];
        Point2Type inset = getInsetDirection(seg);
        inset *= d;
        seg.a += inset;
        seg.b += inset;
        insets.push_back(seg);
    }
    assert(insets.size() == segments.size());
}

string segment3(const Segment2Type &s, Scalar z) {
    stringstream ss;
    ss << "[[" << s.a[0] << ", " << s.a[1] << ", " << z << "], [" << s.b[0] << ", " << s.b[1] << ", " << z << "]]";
    return ss.str();
}

void trimConvexSegments(const std::vector<Segment2Type> & rawInsets,
        const std::vector<bool> &convex,
        std::vector<Segment2Type> & segments) {
    assert(segments.size() == 0);
    segments = rawInsets;

    for (unsigned int i = 0; i < segments.size(); i++) {
        unsigned int prevId = i == 0 ? segments.size() - 1 : i - 1;

        Segment2Type &currentSegment = segments[i];
        Segment2Type &previousSegment = segments[prevId];

        if (convex[i]) {

            Point2Type intersection;
            bool trimmed = segmentSegmentIntersection(previousSegment, currentSegment, intersection);
            if (trimmed) {
                previousSegment.b = intersection;
                currentSegment.a = intersection;
            } else {
                stringstream ss;
                ss << "Trim ERROR: segment " << i << endl;
                ss << "segments = [  " << segment3(previousSegment, 0) << " , " << segment3(currentSegment, 0) << " ]; " << endl;
                ss << "color([0,0.5,0,1])loop_segments3(segments, false);" << endl;
                ShrinkyException mixup(ss.str().c_str());
                // throw mixup;
            }
        }
    }
}

void AddReflexSegments(const std::vector<Segment2Type> &segments,
        const std::vector<Segment2Type> &trimmedInsets,
        const std::vector<bool> &convexVertices,
        std::vector<Segment2Type> &newSegments) {
    assert(newSegments.size() == 0);
    newSegments.reserve(segments.size() * 2);

    for (unsigned int i = 0; i < segments.size(); i++) {
        unsigned int prevId = i == 0 ? segments.size() - 1 : i - 1;

        if (!convexVertices[i]) {
            // Point2Type center = segments[i].a;
            Point2Type start = trimmedInsets[prevId].b;
            Point2Type end = trimmedInsets[i].a;
            Segment2Type straight(start, end);
            newSegments.push_back(straight);
        }
        newSegments.push_back(trimmedInsets[i]);
    }

}

void removeShortSegments(const std::vector<Segment2Type> &segments,
        Scalar cutoffLength,
        std::vector<Segment2Type> &shorts) {

    shorts.reserve(segments.size()); // worst case
    assert(cutoffLength > 0);
    Scalar cutoffLength2 = cutoffLength * cutoffLength;

    for (unsigned int i = 0; i < segments.size(); i++) {
        const Segment2Type &seg = segments[i];
        Scalar length2 = seg.squaredLength();
        if (length2 > cutoffLength2) {
            shorts.push_back(seg);
        }
    }
}

void Shrinky::openScadFile(const char *scadFileName) {
    if (scadFileName) {
        fscad.open(scadFileName);
        std::ostream & out = fscad.getOut();
        out << "module loop_segments3(segments, ball=true)" << endl;
        out << "{" << endl;
        out << "	if(ball) corner (x=segments[0][0][0],  y=segments[0][0][1], z=segments[0][0][2], diameter=0.25, faces=12, thickness_over_width=1);" << endl;
        out << "    for(seg = segments)" << endl;
        out << "    {" << endl;
        out << "        tube(x1=seg[0][0], y1=seg[0][1], z1=seg[0][2], x2=seg[1][0], y2=seg[1][1], z2=seg[1][2] , diameter1=0.1, diameter2=0.05, faces=4, thickness_over_width=1);" << endl;
        out << "    }" << endl;
        out << "}" << endl;
        fscad.writeHeader();
    }
}

Shrinky::Shrinky(const char *scadFileName)
: scadFileName(scadFileName), scadZ(0), color(1), counter(0), dz(0) {
    openScadFile(scadFileName);
}

bool attachSegments(Segment2Type &first, Segment2Type &second, Scalar elongation) {
    //	LineSegment2 s0 = elongateAndPrelongate(first, elongation); // elongate(first, elongation);
    //	LineSegment2 s1 = elongateAndPrelongate(second, elongation); //prelongate(second, elongation);

    Segment2Type s0 = first.elongate(elongation);
    Segment2Type s1 = second.prelongate(elongation);

    Point2Type intersection;
    bool trimmed = segmentSegmentIntersection(s0, s1, intersection);
    if (trimmed) {
        first.b = intersection;
        second.a = intersection;
        return true;
    }
    return false;
}

//
// Calculates altitude in terms of the sides
// http://en.wikipedia.org/wiki/Altitude_(triangle)
//
//  @inputs: the length of 3 triangle sides
//  @returns the altitude  from side a

Scalar triangleAltitude(Scalar a, Scalar b, Scalar c) {
    Scalar s = 0.5 * (a + b + c);
    Scalar h = 2 * SCALAR_SQRT(s * (s - a)*(s - b)*(s - c)) / a;
    return h;
}

// an edged has collapsed when its 2 bisectors intersect
// at an altitude that is lower than the inset distance

bool edgeCollapse(const Segment2Type& segment,
        const Point2Type& bisector0,
        const Point2Type& bisector1,
        Scalar elongation,
        Scalar &collapseDistance) {
    // segment is the base of the triangle
    // from which we want the altitude

    Segment2Type bisectorSegment0;
    bisectorSegment0.a = segment.a;
    bisectorSegment0.b = segment.a + bisector0;

    Segment2Type bisectorSegment1;
    bisectorSegment1.a = segment.b + bisector1;
    bisectorSegment1.b = segment.b;

    Segment2Type s0 = bisectorSegment0.elongate(elongation);
    Segment2Type s1 = bisectorSegment1.prelongate(elongation);
    Point2Type intersection;
    bool attached = segmentSegmentIntersection(s0, s1, intersection);
    if (attached) {
        // the triangle is made from
        Point2Type edge0 = segment.a - intersection;
        Point2Type edge1 = segment.b - intersection;
        Scalar a, b, c;
        a = segment.length();
        b = edge0.magnitude();
        c = edge1.magnitude();

        collapseDistance = triangleAltitude(a, b, c);
        if (collapseDistance < 0) {
            assert(0);
        }
        return true;
    }
    return false;
}

void outMap(const std::multimap<Scalar, unsigned int> &collapsingSegments) {
    Log::info() << "collapse distance\tsegment id" << endl;
    Log::info() << "--------------------------------" << endl;
    for (std::multimap<Scalar, unsigned int>::const_iterator it = collapsingSegments.begin();
            it != collapsingSegments.end(); it++) {
        const std::pair<Scalar, unsigned int>& seg = *it;
        Log::info() << "\t" << seg.first << ",\t" << seg.second << endl;
    }
}

Scalar removeFirstCollapsedSegments(const std::vector<Segment2Type> &originalSegments,
        const std::vector<Point2Type> &bisectors,
        Scalar insetDist,
        std::vector<Segment2Type> &relevantSegments) {
    Scalar elongation = 100;
    assert(relevantSegments.size() == 0);

    relevantSegments.reserve(originalSegments.size());
    //Log::often() << "NB of segments:" << originalSegments.size() << endl;

    multimap<Scalar, unsigned int> collapsingSegments;

    // Log::often() << endl << "removeFirstCollapsedSegments:: looking for collapses" << endl;
    std::vector<Segment2Type> segments = originalSegments;
    for (unsigned int i = 0; i < segments.size(); i++) {
        unsigned int nextId = i == segments.size() - 1 ? 0 : i + 1;

        //const LineSegment2 &nextSeg = segments[nextId];
        const Point2Type &nextBisector = bisectors[nextId];

        const Segment2Type &currentSegment = segments[i];
        const Point2Type &currentBisector = bisectors[i];

        Scalar collapseDistance;
        // check
        //Log::often() << "segment[" << i << "] = " << currentSegment << endl;
        bool collapsed = edgeCollapse(currentSegment,
                currentBisector,
                nextBisector,
                elongation,
                collapseDistance);
        if (collapsed) {
            //Log::often() << " **  segment " << i << " ,collapse distance " <<  collapseDistance << endl;
            if (collapseDistance < insetDist) {
                // shortestCollapseDistance = collapseDistance;
                collapsingSegments.insert(std::pair<Scalar, unsigned int>(collapseDistance, i));
            }
        }
    }

    if (collapsingSegments.empty()) {
        // no problem... inset all the way!
        for (unsigned int i = 0; i < segments.size(); i++) {
            relevantSegments.push_back(segments[i]);
        }
        return insetDist;
    }

    // otherwise...

    std::multimap<Scalar, unsigned int>::iterator collapserator = collapsingSegments.begin();
    Scalar collapseDistance = (*collapserator).first;
    //Log::often() << "COLLAPSED ID " << firstCollapse << endl;
    std::set<unsigned int> toRemove;

    // Log::often() << "removeFirstCollapsedSegments:: who to remove" << endl;
    bool done = false;
    do {
        Scalar d = (*collapserator).first;
        unsigned int segmentId = (*collapserator).second;
        //Log::often() << "  " << d << ": Removing collapsed segment[" << segmentId <<"]=" << segments[segmentId] << endl;
        toRemove.insert(segmentId);
        collapserator++;
        if (collapserator == collapsingSegments.end()) {
            done = true;
        }
        if (d > collapseDistance) {
            //	Log::often() << "d(" << d << ") > collapseDistance (" << collapseDistance << endl;
            done = true;
        }
    } while (!done);

    //Log::often() << "removeFirstCollapsedSegments:: making new list" << endl;
    for (unsigned int i = 0; i < segments.size(); i++) {
        if (toRemove.find(i) == toRemove.end()) {
            relevantSegments.push_back(segments[i]);
        }
    }
    return collapseDistance;
}

// True if the 3 points are collinear

bool collinear(const Point2Type &a, const Point2Type &b, const Point2Type &c, Scalar tol) {
    Scalar dot = ((b[0] - a[0]) * (c[1] - a[1]) - (c[0] - c[0]) * (b[1] - a[1]));
    bool r = libthing::tequals(dot, 0, tol);
    return r;
}

void elongateAndTrimSegments(const std::vector<Segment2Type> & longSegments,
        Scalar elongation,
        std::vector<Segment2Type> &segments) {
    Scalar tol = 1e-6;


    segments = longSegments;
    for (unsigned int i = 0; i < segments.size(); i++) {
        unsigned int prevId = i == 0 ? segments.size() - 1 : i - 1;

        Segment2Type &previousSegment = segments[prevId];
        Segment2Type &currentSegment = segments[i];

        //Log::often() << "prev: seg[" << prevId << "] = " << previousSegment << endl;
        //Log::often() << "cur:  seg[" << i << "] = " << currentSegment << endl;

        if (previousSegment.b.tequals(currentSegment.a, tol)) {
            // the job is already done.. segments are attached,
            // nothing to see
            // Log::often() << "already attached" << endl;
            continue;
        }

        if (previousSegment.length() == 0) {
            Log::info() << "X";
            continue;
        }

        if (currentSegment.length() == 0) {
            Log::info() << "Y";
            continue;
        }

        bool attached = attachSegments(previousSegment, currentSegment, elongation);
        if (!attached) {
            Log::info() << "!";
            Point2Type m = (previousSegment.a + currentSegment.b) * 0.5;
            previousSegment.b = m;
            currentSegment.a = m;

        }
        // Log::often() << "attach point " << currentSegment.a << endl;
        //Log::often() << endl;
    }
}

void createBisectors(const std::vector<Segment2Type>& segments,
        Scalar tol,
        std::vector<Point2Type> &motorCycles) {
    for (unsigned int i = 0; i < segments.size(); i++) {
        unsigned int prevId = i == 0 ? segments.size() - 1 : i - 1;

        const Segment2Type &prevSeg = segments[prevId];
        const Segment2Type &seg = segments[i];

        Point2Type prevInset = getInsetDirection(prevSeg);
        Point2Type inset = getInsetDirection(seg);

        Point2Type bisector = inset;

        // if points are disjoint, do not combine both insets
        if (prevSeg.b.tequals(seg.a, tol)) {
            bisector += prevInset;
        } else {
            //
            // ok... maybe this is a bit drastic and we could combine the biesctors
            // this author needs to make up his mind about non closed polygon support
            //
            Point2Type dist = prevSeg.b - seg.a;
            stringstream ss;
            ss << "This is not a closed polygon. segment[" << prevId << "].b = " << prevSeg.b;
            ss << " and segment[" << i << "].a = " << seg.a << " are distant by " << dist.magnitude();
            ss << endl;
            ScadDebugFile::segment3(ss, "", "segments", segments, 0, 0.1);
            Log::info() << "O";
            ShrinkyException mixup(ss.str().c_str());
            throw mixup;
            // assert(0);
        }
        if (bisector.squaredMagnitude() == 0) {
            stringstream ss;
            ss << "Null bisector at segment [" << i << "] position=" << seg.a << endl;
            ss << " previous_inset=" << prevInset << " inset=" << inset;
            Log::info() << "N";
            ShrinkyException mixup(ss.str().c_str());
            throw mixup;
        }
        bisector.normalise();

        motorCycles.push_back(bisector);
    }
}

void Shrinky::writeScadBisectors(const std::vector<Point2Type> & bisectors, const std::vector<Segment2Type> & originalSegments) {
    if (scadFileName) {
        std::vector<Segment2Type> motorCycleTraces;
        for (size_t i = 0; i < bisectors.size(); i++) {
            Point2Type a = originalSegments[i].a;
            Point2Type dir = bisectors[i];
            dir *= 2;
            Point2Type b = a + dir;
            Segment2Type s(a, b);
            motorCycleTraces.push_back(s);
        }
        scadZ = fscad.writeSegments3("bisectors_", "color([0.75,0.5,0.2,1])loop_segments3", motorCycleTraces, scadZ, dz, this->counter);
    }

}

void Shrinky::writeScadSegments(const char* segNames,
        const char* prefix,
        const std::vector<Segment2Type> & segments) {
    if (scadFileName) {
        string funcName = prefix;
        funcName += "loop_segments3";
        scadZ = fscad.writeSegments3(segNames, funcName.c_str(), segments, scadZ, dz, this->counter);
    }
}

void Shrinky::inset(const std::vector<Segment2Type>& originalSegments,
        Scalar insetDist,
        std::vector<Segment2Type> &finalInsets) {
    bool writePartialSteps = true;

    int count = originalSegments.size();

    if (count < 2) {
        assert(0);
    }
    assert(finalInsets.size() == 0);

    finalInsets.reserve(originalSegments.size());

    Scalar tol = 1e-6; // for continuity testing and distance to go
    Scalar distanceToGo = insetDist;

    std::vector<Segment2Type> initialSegs = originalSegments;

    bool done = false;
    while (!done) {
        connectivityCheck(initialSegs, tol);

        //Log::often() << " ** distance to go: " <<  distanceToGo << endl;
        finalInsets.clear();

        Scalar distanceGone = insetStep(initialSegs, distanceToGo, tol, writePartialSteps, finalInsets);

        distanceToGo -= distanceGone;
        if (libthing::tequals(distanceToGo, 0, tol)) {
            done = true;
            return;
        }
        if (finalInsets.size() > 2) {
            initialSegs = finalInsets;
        } else {
            return;
        }
    }

}

void removeZeroLengthSegments(const std::vector<Segment2Type> &inputSegments, std::vector<Segment2Type> &segments, Scalar tol) {

    assert(inputSegments.size() > 0);
    segments.reserve(inputSegments.size());
    // deep copy
    for (unsigned int i = 0; i < inputSegments.size(); i++) {
        const Segment2Type &seg = inputSegments[i];
        if (libthing::tequals(seg.squaredLength(), 0, tol)) {
            continue;
        }
        segments.push_back(seg);
    }
}

Scalar Shrinky::insetStep(const std::vector<Segment2Type>& originalSegments,
        Scalar insetDist,
        Scalar continuityTolerance,
        bool, //  writePartialStep,
        std::vector<Segment2Type> &finalInsets) {
    Scalar tol = 1e-6;
    // magic numbers
    Scalar elongation = insetDist * 100; // continuityTolerance * 5;

    unsigned int segmentCount = originalSegments.size();

    assert(segmentCount > 0);
    assert(finalInsets.size() == 0);
    assert(&originalSegments != &finalInsets);

    if (segmentCount < 2) {
        stringstream ss;
        ss << "1 line segment is not enough to create a closed polygon";
        assert(0);
        ShrinkyException mixup(ss.str().c_str());
        throw mixup;
    }

    bool dumpSteps = false;

    if (dumpSteps)segmentsDiagnostic("originalSegments", originalSegments);

    Scalar insetStepDistance = insetDist;
    try {
        if (scadFileName) {
            // OpenScad
            //	        Scalar dz = 0.1;
            stringstream coloredOutline;
            // Scalar color = (1.0 * i)/(shells-1);
            int color = 0;
            coloredOutline << "color([" << color << "," << color << "," << 1 - color << " ,1])";
            Scalar dzBefore = scadZ;
            writeScadSegments("outlines_", coloredOutline.str().c_str(), originalSegments);
            // trick to get the bisector in place
            scadZ = dzBefore;
        }

        std::vector<Segment2Type> relevantSegments;
        if (originalSegments.size() > 2) {
            //Log::often() << "...BISECTING..." << endl;
            std::vector<Point2Type> bisectors;
            createBisectors(originalSegments, continuityTolerance, bisectors);
            writeScadBisectors(bisectors, originalSegments);

            //Log::often() << "...COLLAPSING..." << endl;
            insetStepDistance = removeFirstCollapsedSegments(originalSegments, bisectors, insetDist, relevantSegments);
            if (dumpSteps) segmentsDiagnostic("relevantSegments", relevantSegments);
            writeScadSegments("relevants_", "color([0.5,0.5,0,1])", relevantSegments);
        }

        std::vector<Segment2Type> insets;
        unsigned int relevantCount = relevantSegments.size();
        if (relevantCount > 2) {
            //Log::often() << "...INSETTING..." << endl;
            insetSegments(relevantSegments, insetStepDistance, insets);
            if (dumpSteps) segmentsDiagnostic("Insets", insets);
            writeScadSegments("raw_insets_", "color([1,0,0.4,1])", insets);
            lengthCheck(insets, "insets");
        }


        std::vector<Segment2Type> connected;
        if (insets.size() > 2) {
            //Log::often() << "...ATTACHING..." << endl;
            elongateAndTrimSegments(insets, elongation, connected);
            writeScadSegments("connected_", "color([0.25,0.25,0.25,1])", connected);
            // lengthCheck(finalInsets, "finalInsets");
        }

        if (connected.size() > 2) {
            removeZeroLengthSegments(connected, finalInsets, tol);
            writeScadSegments("final_insets_", "color([0.5,0.5,0.5,1])", connected);
        }

    } catch (ShrinkyException &mixup) {

        Log::info() << mixup.error << endl;

        // Log::often() << "ABORT MISSION!!! " << insetStepDistance << ": " << mixup.error << endl;
        // this is a lie...  but we want to break the loop
        insetStepDistance = insetDist;
        throw;
    }
    this->counter++;
    return insetStepDistance;
}

void Shrinky::closeScadFile() {
    if (scadFileName) {
        std::ostream & out = fscad.getOut();
        int shells = counter;
        fscad.writeMinMax("draw_outlines", "outlines_", shells);
        fscad.writeMinMax("draw_bisectors", "bisectors_", shells);
        fscad.writeMinMax("draw_raw_insets", "raw_insets_", shells);
        //fscad.writeMinMax("draw_trimmed_insets",  "trimmed_insets_", shells);
        fscad.writeMinMax("draw_relevants", "relevants_", shells);
        fscad.writeMinMax("draw_final_insets", "final_insets_", shells);
        out << "min=0;" << endl;
        out << "max=" << shells - 1 << ";" << std::endl;
        out << std::endl;
        out << "draw_outlines(min, max);" << std::endl;
        out << "draw_bisectors(min, max);" << std::endl;
        out << "draw_relevants(min, max);" << std::endl;
        out << "draw_raw_insets(min, max);" << std::endl;
        out << "draw_final_insets(min, max);" << std::endl;
        out << endl;
        out << "// s = [\"segs.push_back(TriangleSegment2(Point2Type(%s+x, %s+y), Point2Type(%s+x, %s+y)));\" %(x[0][0], x[0][1], x[1][0], x[1][1]) for x in segments]" << std::endl;
        out << "// print '\\n'.join(s) " << endl;
        fscad.close();
    }
}

Shrinky::~Shrinky() {
    closeScadFile();
}

void createShells(const SegmentVector & outlinesSegments,
        const std::vector<Scalar> &insetDistances,
        unsigned int sliceId,
        const char *scadFile,
        bool writeDebugScadFiles,
        std::vector<SegmentVector> & insetsForLoops)
 {
    assert(insetsForLoops.size() == 0);
    unsigned int nbOfShells = insetDistances.size();



    // dbgs__( "outlineSegmentCount " << outlineSegmentCount)
    for (unsigned int outlineId = 0; outlineId < outlinesSegments.size(); outlineId++) {
        const std::vector<Segment2Type> &outlineLoop = outlinesSegments[outlineId];
        assert(outlineLoop.size() > 0);

        insetsForLoops.push_back(SegmentVector());
        assert(insetsForLoops.size() == outlineId + 1);

        SegmentVector &insetTable = *insetsForLoops.rbegin(); // inset curves for a single loop
        insetTable.reserve(nbOfShells);
        for (unsigned int shellId = 0; shellId < nbOfShells; shellId++) {
            insetTable.push_back(std::vector<Segment2Type > ());
        }

        //unsigned int segmentCountBefore =0;
        //unsigned int segmentCountAfter =0;

        unsigned int currentShellIdForErrorReporting = 0;
        try {

            Shrinky shrinky;
            const vector<Segment2Type> *previousInsets = &outlineLoop;
            for (unsigned int shellId = 0; shellId < nbOfShells; shellId++) {
                currentShellIdForErrorReporting = shellId;
                Scalar insetDistance = insetDistances[shellId];
                std::vector<Segment2Type> &insets = insetTable[shellId];
                if ((*previousInsets).size() > 2) {
                    shrinky.inset(*previousInsets, insetDistance, insets);
                    previousInsets = &insets;
                }
            }
        } catch (ShrinkyException &messup) {
            if (writeDebugScadFiles) {
                static int counter = 0;
                Log::info() << endl;
                Log::info() << "----- ------ ERROR " << counter << " ------ ------" << endl;
                Log::info() << "sliceId: " << sliceId << endl;
                Log::info() << "loopId : " << outlineId << endl;
                Log::info() << "shellId: " << currentShellIdForErrorReporting << endl;

                stringstream ss;
                ss << "_slice_" << sliceId << "_loop_" << outlineId << ".scad";

                MyComputer myComputer;
                string loopScadFile = myComputer.fileSystem.ChangeExtension(scadFile, ss.str().c_str());
                Shrinky shriker(loopScadFile.c_str());
                shriker.dz = 0.1;
                try {
                    std::ostream &scad = shriker.fscad.getOut();
                    scad << "/*" << endl;
                    scad << messup.error;
                    scad << endl << "*/" << endl;


                    vector<Segment2Type> previousInsets = outlineLoop;
                    Log::info() << "Creating file: " << loopScadFile << endl;
                    Log::info() << "	Number of points " << (int) previousInsets.size() << endl;
                    ScadDebugFile::segment3(cout, "", "segments", previousInsets, 0, 0.1);
                    std::vector<Segment2Type> insets;
                    for (unsigned int shellId = 0; shellId < nbOfShells; shellId++) {
                        Scalar insetDistance = insetDistances[shellId];
                        shriker.inset(previousInsets, insetDistance, insets);
                        previousInsets = insets;
                        insets.clear(); // discard...
                    }
                } catch (ShrinkyException &) // the same excpetion is thrown again
                {

                    Log::info() << "saving " << endl;
                }
                Log::info() << "--- --- ERROR " << counter << " END --- ----" << endl;
                counter++;
            }
        }
    }
}

//// creates shells from the segments table
////

void createShellsForSliceUsingShrinky(const SegmentVector & outlinesSegments,
        const std::vector<Scalar> &insetDistances,
        unsigned int sliceId,
        const char *scadFile,
        std::vector<SegmentVector> & insetsForLoops)
 {
    assert(insetsForLoops.size() == 0);
    unsigned int nbOfShells = insetDistances.size();

    for (unsigned int shellId = 0; shellId < nbOfShells; shellId++) {
        insetsForLoops.push_back(SegmentVector());
        SegmentVector &currentShellTable = *insetsForLoops.rbegin();
        for (unsigned int outlineId = 0; outlineId < outlinesSegments.size(); outlineId++) {
            try {
                Shrinky shrinky;
                currentShellTable.push_back(std::vector<Segment2Type > ());
                std::vector<Segment2Type> &outlineShell = *currentShellTable.rbegin();
                Scalar dist = insetDistances[shellId];
                const SegmentVector *pInputs = NULL;
                if (shellId == 0) {
                    pInputs = &outlinesSegments;
                } else {
                    pInputs = &insetsForLoops[shellId - 1];
                }
                const SegmentVector &inputTable = *pInputs;
                const std::vector<Segment2Type> & inputSegments = inputTable[outlineId];

                if (inputSegments.size() > 2) {
                    shrinky.inset(inputSegments, dist, outlineShell);
                }

            } catch (ShrinkyException &messup) {
                if (scadFile != 0x00) {
                    static int counter = 0;
                    Log::info() << endl;
                    Log::info() << "----- ------ ERROR " << counter << " ------ ------" << endl;
                    Log::info() << "sliceId: " << sliceId << endl;
                    Log::info() << "loopId : " << outlineId << endl;
                    Log::info() << "shellId: " << shellId << endl;

                    stringstream ss;
                    ss << "_slice_" << sliceId << "_loop_" << outlineId << ".scad";

                    MyComputer myComputer;
                    string loopScadFile = myComputer.fileSystem.ChangeExtension(scadFile, ss.str().c_str());
                    Shrinky shriker(loopScadFile.c_str());
                    shriker.dz = 0.1;
                    try {
                        Shrinky shrinky;
                        currentShellTable.push_back(std::vector<Segment2Type > ());
                        std::vector<Segment2Type> &outlineShell = *currentShellTable.rbegin();
                        Scalar dist = insetDistances[shellId];
                        if (shellId == 0) {
                            const SegmentVector &inputTable = outlinesSegments;
                            const std::vector<Segment2Type> & inputSegments = inputTable[outlineId];
                            shrinky.inset(inputSegments, dist, outlineShell);
                        } else {
                            const SegmentVector &inputTable = insetsForLoops[shellId - 1];
                            const std::vector<Segment2Type> & inputSegments = inputTable[outlineId];
                            shrinky.inset(inputSegments, dist, outlineShell);
                        }
                    } catch (ShrinkyException &) // the same excpetion is thrown again
                    {

                        Log::info() << "saving " << endl;
                    }
                    Log::info() << "--- --- ERROR " << counter << " END --- ----" << endl;
                    counter++;
                }
            }
        }
    }
}

}

