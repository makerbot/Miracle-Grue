#include <vector>
#include <algorithm>

#include "loop_utils.h"
#include "clipper.h"

namespace mgl {

static const Scalar DBLTOINT = 20000;

void Point2TypeToIntPoint(const Point2Type pt, ClipperLib::IntPoint &ip) {
	ip.X = pt.x * DBLTOINT;
	ip.Y = pt.y * DBLTOINT;
}

void IntPointToPoint2Type(const ClipperLib::IntPoint ip, Point2Type &pt) {
	pt.x = ip.X / DBLTOINT;
	pt.y = ip.Y / DBLTOINT;
}


void loopToClPolygon(const Loop &loop,
					 ClipperLib::Polygon &clpoly) {
	clpoly.clear();

	for (Loop::const_finite_ccw_iterator pn = loop.counterClockwiseFinite();
		 pn != loop.counterClockwiseEnd(); ++pn) {
		clpoly.push_back(ClipperLib::IntPoint());
		ClipperLib::IntPoint &ip = clpoly.back();

		Point2TypeToIntPoint(pn->getPoint(), ip);
	}
}	

void loopToClPolygon(const LoopList &loops,
					   ClipperLib::Polygons &clpolys) {
	clpolys.clear();

	for (LoopList::const_iterator loop = loops.begin();
		 loop != loops.end(); ++loop) {
		clpolys.push_back(ClipperLib::Polygon());
		ClipperLib::Polygon &clpoly = clpolys.back();

		loopToClPolygon(*loop, clpoly);
	}
}


void ClPolygonToLoop(const ClipperLib::Polygon &clpoly,
					 Loop &loop) {
	for (ClipperLib::Polygon::const_reverse_iterator ip = clpoly.rbegin();
		 ip != clpoly.rend(); ++ip) {
		Point2Type pt;
		IntPointToPoint2Type(*ip, pt);

		loop.insertPointBefore(pt, loop.clockwiseEnd());
	}
}

void ClPolygonToLoop(const ClipperLib::Polygons &clpolys,
					   LoopList &loops) {
	loops.clear();

	for (ClipperLib::Polygons::const_iterator clpoly = clpolys.begin();
		 clpoly != clpolys.end(); ++clpoly) {
		loops.push_back(Loop());
		Loop &loop = loops.back();

		ClPolygonToLoop(*clpoly, loop);
	}
}


void runClipper(LoopList &dest, const LoopList &subject, const LoopList &apply,
				const ClipperLib::ClipType type) {
	ClipperLib::Clipper clip;

	ClipperLib::Polygons clsubject;
	loopToClPolygon(subject, clsubject);
	clip.AddPolygons(clsubject, ClipperLib::ptSubject);
	
	ClipperLib::Polygons clapply;
	loopToClPolygon(apply, clapply);
	clip.AddPolygons(clapply, ClipperLib::ptClip);

	ClipperLib::Polygons cldest;
	clip.Execute(type, cldest);

	ClPolygonToLoop(cldest, dest);
}	


void loopsUnion(LoopList &dest,
				const LoopList &subject, const LoopList &apply) {
	runClipper(dest, subject, apply, ClipperLib::ctUnion);
}

void loopsUnion(LoopList &subject, const LoopList &apply) {
	loopsUnion(subject, subject, apply);
}


void loopsDifference(LoopList &dest,
				const LoopList &subject, const LoopList &apply) {
	runClipper(dest, subject, apply, ClipperLib::ctDifference);
}

void loopsDifference(LoopList &subject, const LoopList &apply) {
	loopsDifference(subject, subject, apply);
}


void loopsIntersection(LoopList &dest,
				const LoopList &subject, const LoopList &apply) {
	runClipper(dest, subject, apply, ClipperLib::ctIntersection);
}

void loopsIntersection(LoopList &subject, const LoopList &apply) {
	loopsIntersection(subject, subject, apply);
}


void loopsXOR(LoopList &dest,
			  const LoopList &subject, const LoopList &apply) {
	runClipper(dest, subject, apply, ClipperLib::ctXor);
}

void loopsXOR(LoopList &subject, const LoopList &apply) {
	loopsXOR(subject, subject, apply);
}

void loopsOffset(LoopList& dest, const LoopList& subject, Scalar distance) {
	ClipperLib::Polygons subjectPolys, destPolys;
	loopToClPolygon(subject, subjectPolys);
	ClipperLib::OffsetPolygons(subjectPolys, destPolys, distance * DBLTOINT, 
			ClipperLib::jtSquare, 2.0);
	ClPolygonToLoop(destPolys, dest);
}

void loopSnip(OpenPath& dest, const LoopPath& subject, Scalar distance) {
    dest.clear();
    Point2Type startingPoint = *subject.fromEnd();
    //back up at least distance
    LoopPath::iterator currentPointIter = subject.fromStart();
    Scalar currentDistance = 0;
    std::cout << "Start" << std::endl;
    do {
        ++currentPointIter;
        std::cout << "Iteration A" << std::endl;
    } while (currentPointIter != subject.end() && (currentDistance = (startingPoint - 
            *currentPointIter).magnitude()) < distance);
    if(currentPointIter == subject.end()) {
        //subject too short
        dest.appendPoints(subject.fromStart(), subject.end());
        std::cout << "END" << std::endl;
        return;
    }
    //we know that currentPointIter is too far, but the one before is too close
    //so go back by the difference
    Scalar adjustment = currentDistance - distance;
    LoopPath::iterator previousPointIter = currentPointIter;
    --previousPointIter;
    Point2Type adjustedPoint = *currentPointIter + 
            Point2Type((*previousPointIter) - 
            (*currentPointIter)).unit() * adjustment;
    for(; currentPointIter != subject.end(); ++currentPointIter) {
        dest.prependPoint(*currentPointIter);
        std::cout << "Iteration B" << std::endl;
    }
    dest.appendPoint(adjustedPoint);
    std::cout << "END" << std::endl;
}

enum SMOOTH_RESULT {
    SMOOTH_ADD,
    SMOOTH_REPLACE
};

SMOOTH_RESULT smoothPoints(const Point2Type& lp1, 
        const Point2Type& lp2, 
        const Point2Type& cp, 
        const Scalar& maxDeviation, 
        const Scalar& factor, 
        Scalar& cumDeviation,
        Point2Type& output);

void smooth(const Loop& input, Scalar smoothness, Loop& output, Scalar factor, 
        bool recurse) {
    if(smoothness == 0 || input.size() <= 3) {
		output = input;
        return;
    }
	Loop::const_finite_cw_iterator current;
	current = input.clockwiseFinite();
    
    typedef std::vector<Point2Type> tmp_collection;
    tmp_collection tmpPoints;
    tmpPoints.push_back(*(current++));
    tmpPoints.push_back(*(current++));
    
	//insert the first two points
    
	Scalar cumulativeError = 0.0;
    
	for(; current != input.clockwiseEnd(); ++current) {
        tmp_collection::reverse_iterator last1 = 
                tmpPoints.rbegin();
        tmp_collection::const_reverse_iterator last2 = 
                tmpPoints.rbegin();
        ++last2;
        const Point2Type& currentPoint = *current;
        const Point2Type& lp1 = *last1;
        const Point2Type& lp2 = *last2;
        Point2Type result;
        SMOOTH_RESULT rslt = smoothPoints(lp1, lp2, currentPoint, smoothness, 
                factor, cumulativeError, result);
        if(rslt == SMOOTH_ADD) {
            tmpPoints.push_back(currentPoint);
        } else {
            tmpPoints.back() = result;
        }
	}
    if(recurse) {
        std::rotate(tmpPoints.begin(), 
                tmpPoints.begin() + tmpPoints.size() / 2, 
                tmpPoints.end());
        
        Loop tmpLoop;
        for(std::vector<Point2Type>::const_iterator iter = tmpPoints.begin(); 
                iter != tmpPoints.end(); 
                ++iter) {
            tmpLoop.insertPointBefore(*iter, tmpLoop.clockwiseEnd());
        }
        smooth(tmpLoop, smoothness, output, factor, false);
    } else {
        for(std::vector<Point2Type>::const_iterator iter = tmpPoints.begin(); 
                iter != tmpPoints.end(); 
                ++iter) {
            output.insertPointBefore(*iter, output.clockwiseEnd());
        }
    }
}

void smooth(const OpenPath& input, Scalar smoothness, OpenPath& output, Scalar factor) {
    if(smoothness == 0 || input.size() <= 3) {
		output = input;
        return;
    }
	OpenPath::const_iterator current;
	current = input.fromStart();
    
    output.appendPoint(*(current++));
    output.appendPoint(*(current++));
    
	//insert the first two points
    
	Scalar cumulativeError = 0.0;
    
	for(; current != input.end(); ++current) {
        OpenPath::reverse_iterator last1 = 
                output.fromEnd();
        OpenPath::const_reverse_iterator last2(
                output.fromEnd());
        ++last2;
        const Point2Type& currentPoint = *current;
        const Point2Type& lp1 = *last1;
        const Point2Type& lp2 = *last2;
        Point2Type result;
        SMOOTH_RESULT rslt = smoothPoints(lp1, lp2, currentPoint, smoothness, 
                factor, cumulativeError, result);
        if(rslt == SMOOTH_ADD) {;
            output.appendPoint(currentPoint);
        } else {
            (*output.fromEnd()) = result;
        }
	}
}


SMOOTH_RESULT smoothPoints(const Point2Type& lp1, 
        const Point2Type& lp2, 
        const Point2Type& cp, 
        const Scalar& maxDeviation, 
        const Scalar& factor, 
        Scalar& cumDeviation, 
        Point2Type& output) {
    Point2Type ldelta = lp1 - lp2;
    Point2Type unit;
    try{
        unit = ldelta.unit();
    } catch (const GeometryException& e) {
        output = cp;
        return SMOOTH_ADD;
    }
    Point2Type delta = cp - lp1;
    Scalar component = delta.dotProduct(unit);
    Scalar deviation = delta.crossProduct(unit);
    deviation = deviation < 0 ? -deviation : deviation;
    Point2Type landingPoint = lp1 + unit*component;
    cumDeviation += deviation;
    if(cumDeviation > maxDeviation) {
        output = cp;
        cumDeviation = 0;
        return SMOOTH_ADD;
    } else {
        output = cp * factor + landingPoint * (1.0 - factor);
        return SMOOTH_REPLACE;
    }
}



}
