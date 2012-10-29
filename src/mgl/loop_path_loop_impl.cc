#include <vector>

#include "loop_path_decl.h"
#include "loop_path_iter_impl.h"

namespace mgl {

Loop::PointNormal::PointNormal(const Point2Type& point)
		: point(point), normalDirty(true), myIteratorPointer(NULL) {}

Loop::PointNormal::PointNormal()
		: normalDirty(true), myIteratorPointer(NULL) {}

Loop::PointNormal::PointNormal(const PointNormal& orig)
		: point(orig.point), normalDirty(orig.normalDirty),
		normal(orig.normal), myIteratorPointer(NULL) {
	if (orig.myIteratorPointer)
		myIteratorPointer = new myIteratorType(*orig.myIteratorPointer);
}

Loop::PointNormal& Loop::PointNormal::operator =(const PointNormal& orig) {
	if (&orig == this)
		return *this;
	point = orig.point;
	normalDirty = orig.normalDirty;
	normal = orig.normal;
	delete myIteratorPointer;
	if (orig.myIteratorPointer)
		myIteratorPointer = new myIteratorType(*orig.myIteratorPointer);
	return *this;
}

Loop::PointNormal::~PointNormal() {
	delete myIteratorPointer;
}

Loop::PointNormal::operator Point2Type() const {
	return point;
}

const Point2Type& Loop::PointNormal::getPoint() const {
	return point;
}

const Point2Type& Loop::PointNormal::getNormal() const {
	if(normalDirty)
		recalculateNormal();
	return normal;
}

void Loop::PointNormal::setPoint(const Point2Type& npoint) {
	point = npoint;
	normalDirty = true;
	if(myIteratorPointer){
		myIteratorType Bi = *myIteratorPointer;
		myIteratorType Ai = Bi;
		myIteratorType Ci = Bi;
		--Ai;	//point to previous
		++Ci;	//point to next
		Ai->normalDirty = true;
		Ci->normalDirty = true;
	}
}

void Loop::PointNormal::setIterator(const myIteratorType& iter) {
	if(myIteratorPointer)
		*myIteratorPointer = iter;
	else
		myIteratorPointer = new myIteratorType(iter);
}

void Loop::PointNormal::recalculateNormal() const {
	normalDirty = false;
	/* A------B------C
		* Assume we are point B. Normal is (B-A).rotate(90 degrees cw)
		* normalized average with (C-B).rotate(90 degrees cw) normalized
		* Then normalize the average
		*/
	if(myIteratorPointer){
		/* If we have a valid iterator, use it to find the point
			* before and after, then call the computation on those
			*
			*/
		myIteratorType Bi = *myIteratorPointer;
		myIteratorType Ai = Bi;
		myIteratorType Ci = Bi;
		--Ai;	//point to previous
		++Ci;	//point to next
		recalculateNormal(*Ai, *Bi);
	} else {
		normal = Point2Type();
	}
}

void Loop::PointNormal::recalculateNormal(const PointNormal& A, 
		const PointNormal& C) const {
	// A------B------C
	// this is B
	Point2Type ba = point-A;
	Point2Type cb = static_cast<Point2Type>(C)-point;
	// rotate and normalize both vectors
	ba = ba.rotate2d(M_PI_2).unit();
	cb = cb.rotate2d(M_PI_2).unit();
	// normal is the unit vector of the sum (same as unit of average)
	normal = (ba+cb).unit();
}

Loop::Loop() {}

Loop::Loop(const Point2Type& first) {
	insertPointBefore(first, clockwiseEnd());
}

Loop::cw_iterator Loop::clockwise(const Point2Type& startpoint) {
	for (PointNormalList::iterator i = pointNormals.begin();
			i != pointNormals.end(); i++) {
		if (i->getPoint() == startpoint)
			return cw_iterator(i, pointNormals.begin(), 
					pointNormals.end());
	}
	return clockwiseEnd();
}

Loop::const_cw_iterator Loop::clockwise(const Point2Type& startpoint) const {
	for (PointNormalList::const_iterator i = pointNormals.begin();
			i != pointNormals.end(); i++) {
		if (i->getPoint() == startpoint)
			return const_cw_iterator(i, pointNormals.begin(), 
					pointNormals.end());
	}
	return clockwiseEnd();
}

Loop::cw_iterator Loop::clockwise() { 
	return cw_iterator(pointNormals.begin(), pointNormals.begin(), 
			pointNormals.end());
}

Loop::const_cw_iterator Loop::clockwise() const { 
	return const_cw_iterator(pointNormals.begin(), pointNormals.begin(), 
			pointNormals.end());
}

Loop::finite_cw_iterator Loop::clockwiseFinite() {
	return finite_cw_iterator(clockwise());
}

Loop::const_finite_cw_iterator Loop::clockwiseFinite() const {
	return const_finite_cw_iterator(clockwise());
}

Loop::cw_iterator Loop::clockwiseEnd() { 
	return cw_iterator(pointNormals.end(), 
			pointNormals.begin(), pointNormals.end()); 
}

Loop::const_cw_iterator Loop::clockwiseEnd() const { 
	return const_cw_iterator( pointNormals.end(), 
			pointNormals.begin(), pointNormals.end()); 
}

Loop::ccw_iterator Loop::counterClockwise(const Point2Type& startpoint) {
	for (PointNormalList::reverse_iterator i = pointNormals.rbegin();
			i != pointNormals.rend(); i++) {
		if (i->getPoint() == startpoint)
			return ccw_iterator(i, pointNormals.rbegin(), 
					pointNormals.rend());
	}
	return counterClockwiseEnd();
}

Loop::const_ccw_iterator Loop::counterClockwise(
		const Point2Type& startpoint) const {
	for (PointNormalList::const_reverse_iterator i = pointNormals.rbegin();
			i != pointNormals.rend(); i++) {
		if (i->getPoint() == startpoint)
			return const_ccw_iterator(i, pointNormals.rbegin(), 
					pointNormals.rend());
	}
	return counterClockwiseEnd();
}

Loop::ccw_iterator Loop::counterClockwise() {
	return ccw_iterator(pointNormals.rbegin(), pointNormals.rbegin(), 
			pointNormals.rend());
}

Loop::const_ccw_iterator Loop::counterClockwise() const {
	return const_ccw_iterator(pointNormals.rbegin(), pointNormals.rbegin(), 
			pointNormals.rend());
}

Loop::finite_ccw_iterator Loop::counterClockwiseFinite() {
	return finite_ccw_iterator(counterClockwise());
}

Loop::const_finite_ccw_iterator Loop::counterClockwiseFinite() const {
	return const_finite_ccw_iterator(counterClockwise());
}

Loop::ccw_iterator Loop::counterClockwiseEnd() { 
	return ccw_iterator(pointNormals.rend(), 
			pointNormals.rbegin(), pointNormals.rend()); 
}

Loop::const_ccw_iterator Loop::counterClockwiseEnd() const { 
	return const_ccw_iterator(pointNormals.rend(), 
		pointNormals.rbegin(), pointNormals.rend()); 
}

Loop::entry_iterator Loop::entryBegin() const  {
	return entry_iterator(pointNormals.begin(), 
			pointNormals.begin(), pointNormals.end());
}

Point2Type Loop::getExitPoint(entry_iterator entry) const {
	return *entry;
}

Loop::entry_iterator Loop::entryEnd() const {
	return entry_iterator(pointNormals.end(), 
			pointNormals.begin(), pointNormals.end());
}

Loop::cw_iterator Loop::getSuspendedPoints() { 
	return clockwise(pointNormals.front()); 
}

Loop::const_cw_iterator Loop::getSuspendedPoints() const { 
	return clockwise(pointNormals.front()); 
}

bool Loop::empty() const {
	return pointNormals.empty();
}

bool Loop::windingContains(const Point2Type& point) const {
    int accum = 0;
    for(const_finite_cw_iterator iter = clockwiseFinite(); 
            iter != clockwiseEnd(); 
            ++iter) {
        Segment2Type segment = segmentAfterPoint(iter);
        if(segment.a.y <= point.y) {
            if(segment.b.y > point.y) {
                if(segment.testLeft(point) > 0)
                    ++accum;
            }
        } else {
            if(segment.b.y <= point.y) {
                if(segment.testRight(point) > 0)
                    --accum;
            }
        }
    }
    return accum != 0;
}

Scalar Loop::curl() const {
	Scalar accum = 0;
	for(const_finite_ccw_iterator curIter = counterClockwiseFinite(); 
			curIter != counterClockwiseEnd(); 
			++curIter) {
		const_ccw_iterator curCcw(curIter);
		const_ccw_iterator nextCcw(curIter);
		++nextCcw;
		Segment2Type curSegment = segmentAfterPoint(curCcw);
		Segment2Type nextSegment = segmentAfterPoint(nextCcw);
		Point2Type curRel = curSegment.b - curSegment.a;
		Point2Type nextRel = nextSegment.b - nextSegment.a;
		accum += curRel.crossProduct(nextRel);
	}
	return accum;
}

void Loop::refreshIteratorRefs() {
	for(PointNormalList::iterator it = pointNormals.begin(); 
			it != pointNormals.end(); 
			++it){
		it->setIterator(PointNormal::myIteratorType(it, 
				pointNormals.begin(), 
				pointNormals.end()));
	}
}

bool operator==(const Loop::PointNormal& lhs, const Loop::PointNormal& rhs) {
	return lhs.getPoint() == rhs.getPoint();
}
bool operator!=(const Loop::PointNormal& lhs, const Loop::PointNormal& rhs) {
	return !(lhs == rhs);
}

}






