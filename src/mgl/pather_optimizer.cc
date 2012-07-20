#include <list>
#include <limits.h>

#include "pather_optimizer.h"

namespace mgl {

pather_optimizer::pather_optimizer(bool lp) : linkPaths(lp) {}

void pather_optimizer::addPath(const OpenPath& path) {
	//we don't consider paths of a single point
	if(path.size() > 1)
		myPaths.push_back(path);
}

void pather_optimizer::addPath(const Loop& loop)  {
	//we only consider loops that are polygons, not lines or points
	if(loop.size() > 2)
		myLoops.push_back(loop);
}

void pather_optimizer::addBoundary(const OpenPath& path) {
	//boundaries are broken down into linesegments
	for(OpenPath::const_iterator iter = path.fromStart(); 
			iter != path.end(); 
			++iter) {
		boundaries.push_back(path.segmentAfterPoint(iter));
	}
}

void pather_optimizer::addBoundary(const Loop& loop) {
		//boundaries are broken down into linesegments
		for(Loop::const_finite_cw_iterator iter = loop.clockwiseFinite(); 
				iter != loop.clockwiseEnd(); 
				++iter) {
			boundaries.push_back(loop.segmentAfterPoint(iter));
		}
}

void pather_optimizer::clearBoundaries() {
	boundaries.clear();
}

void pather_optimizer::clearPaths() {
	myLoops.clear();
	myPaths.clear();
}

OpenPath pather_optimizer::closestLoop(LoopList::iterator loopIter, 
		Loop::entry_iterator entryIter) {
	//we have found a closest loop
	OpenPath ret;
	LoopPath lp(*loopIter, 
			loopIter->clockwise(*entryIter), 
			loopIter->counterClockwise(*entryIter));
	//turn into into a path
	ret.appendPoints(lp.fromStart(), lp.end());
	//remove it from our "things to optimize"
	myLoops.erase(loopIter);
	return ret;
}

OpenPath pather_optimizer::closestPath(OpenPathList::iterator pathIter, 
		OpenPath::entry_iterator entryIter) {
	//we have found a closest path
	OpenPath ret;
	//extract it in the right order
	if(*entryIter == *(pathIter->fromStart())) {
		ret = *pathIter;
	} else {
		ret.appendPoints(pathIter->fromEnd(), pathIter->rend());
	}
	//remove it from our "things to optimize"
	myPaths.erase(pathIter);
	return ret;
}

bool pather_optimizer::crossesBoundaries(const libthing::LineSegment2& seg) {
	//test if this linesegment crosses any boundaries
	for(std::list<libthing::LineSegment2>::const_iterator iter = 
			boundaries.begin(); 
			iter != boundaries.end(); 
			++iter) {
		if(seg.intersects(*iter))
			return true;
	}
	return false;
}

}



