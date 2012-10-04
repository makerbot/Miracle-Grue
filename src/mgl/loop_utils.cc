#include <vector>

#include "loop_utils.h"
#include "clipper.h"

namespace mgl {

static const Scalar DBLTOINT = 20000;

void PointTypeToIntPoint(const PointType pt, ClipperLib::IntPoint &ip) {
	ip.X = pt.x * DBLTOINT;
	ip.Y = pt.y * DBLTOINT;
}

void IntPointToPointType(const ClipperLib::IntPoint ip, PointType &pt) {
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

		PointTypeToIntPoint(pn->getPoint(), ip);
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
		PointType pt;
		IntPointToPointType(*ip, pt);

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

void loopsOffset(LoopList& dest, const LoopList& subject, Scalar distance,
				 bool square) {
	ClipperLib::Polygons subjectPolys, destPolys;
	loopToClPolygon(subject, subjectPolys);
	ClipperLib::OffsetPolygons(subjectPolys, destPolys, distance * DBLTOINT, 
							   square ? ClipperLib::jtSquare
							   :ClipperLib::jtMiter, 2.0);

	ClPolygonToLoop(destPolys, dest);
}

}
