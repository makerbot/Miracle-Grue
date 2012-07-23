#include <list>
#include <limits.h>

#include "pather_optimizer.h"

namespace mgl {

pather_optimizer::pather_optimizer(bool lp) : linkPaths(lp) {}

void pather_optimizer::addPath(const OpenPath& path, const PathLabel& label) {
	//we don't consider paths of a single point
	if(path.size() > 1)
		myPaths.push_back(LabeledOpenPath(label, path));
}

void pather_optimizer::addPath(const Loop& loop, const PathLabel& label) {
	//we only consider loops that are polygons, not lines or points
	if(loop.size() > 2)
		myLoops.push_back(LabeledLoop(label, loop));
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

OpenPath pather_optimizer::closestLoop(std::list<LabeledLoop>::iterator loopIter, 
		Loop::entry_iterator entryIter) {
	//we have found a closest loop
	OpenPath ret;
	LoopPath lp(loopIter->myPath, 
			loopIter->myPath.clockwise(*entryIter), 
			loopIter->myPath.counterClockwise(*entryIter));
	//turn into into a path
	ret.appendPoints(lp.fromStart(), lp.end());
	//remove it from our "things to optimize"
	myLoops.erase(loopIter);
	return ret;
}

OpenPath pather_optimizer::closestPath(std::list<LabeledOpenPath>::iterator pathIter, 
		OpenPath::entry_iterator entryIter) {
	//we have found a closest path
	OpenPath ret;
	//extract it in the right order
	if(*entryIter == *(pathIter->myPath.fromStart())) {
		ret = pathIter->myPath;
	} else {
		ret.appendPoints(pathIter->myPath.fromEnd(), pathIter->myPath.rend());
	}
	//remove it from our "things to optimize"
	myPaths.erase(pathIter);
	return ret;
}

void pather_optimizer::findClosestLoop(const PointType& point, 
		std::list<LabeledLoop>::iterator loopIter, 
		Loop::entry_iterator entryIter) {
	
	if(myLoops.empty()) {
		loopIter = myLoops.end();
		return;
	}
	loopIter = myLoops.begin();
	entryIter = loopIter->myPath.entryBegin();
	Scalar closestDistance = (point - *entryIter).magnitude();
	
	for(std::list<LabeledLoop>::iterator currentIter = myLoops.begin(); 
			currentIter != myLoops.end(); 
			++currentIter) {
		for(Loop::entry_iterator currentEntry = 
				currentIter->myPath.entryBegin(); 
				currentEntry != currentIter->myPath.entryEnd(); 
				++currentEntry) {
			Scalar distance = (point - 
					*(currentEntry)).magnitude();
			if(distance < closestDistance) {
				closestDistance = distance;
				entryIter = entryIter;
				loopIter = currentIter;
			}
		}
	}
}

void pather_optimizer::findClosestPath(const PointType& point, 
		std::list<LabeledOpenPath>::iterator pathIter, 
		OpenPath::entry_iterator entryIter) {
	if(myPaths.empty()) {
		pathIter = myPaths.end();
		return;
	}
	pathIter = myPaths.begin();
	entryIter = pathIter->myPath.entryBegin();
	Scalar closestDistance = (point - *entryIter).magnitude();
	
	for(std::list<LabeledOpenPath>::iterator currentIter = myPaths.begin(); 
			currentIter != myPaths.end(); 
			++currentIter) {
		for(OpenPath::entry_iterator currentEntry = 
				currentIter->myPath.entryBegin(); 
				currentEntry != currentIter->myPath.entryEnd(); 
				++currentEntry) {
			Scalar distance = (point - 
					*(currentEntry)).magnitude();
			if(distance < closestDistance) {
				closestDistance = distance;
				entryIter = entryIter;
				pathIter = currentIter;
			}
		}
	}
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



