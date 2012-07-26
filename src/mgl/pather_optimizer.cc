#include <list>
#include <limits.h>
#include <string>

#include "pather_optimizer.h"

namespace mgl {

Scalar pather_optimizer::DISTANCE_THRESHOLD = 0.2;

pather_optimizer::pather_optimizer(bool lp) : linkPaths(lp) {}

void pather_optimizer::addPath(const OpenPath& path, const PathLabel& label) {
	//we don't consider paths of a single point
	if(path.size() > 1) {
		myPaths.push_back(LabeledOpenPath(label, path));
	} else {
		Exception mixup("Attempted to add degenerate path to optimizer");
		throw mixup;
	}
}

void pather_optimizer::addPath(const Loop& loop, const PathLabel& label) {
	//we only consider loops that are polygons, not lines or points
	if(loop.size() > 2) {
		myLoops.push_back(LabeledLoop(label, loop));
	} else {
		Exception mixup("Attempted to add degenerate loop to optimizer");
		throw mixup;
	}
}

void pather_optimizer::addBoundary(const OpenPath& path) {
	//boundaries are broken down into linesegments
	if(path.size() > 1) {
		for(OpenPath::const_iterator iter = path.fromStart(); 
				iter != path.end(); 
				++iter) {
			boundaries.push_back(path.segmentAfterPoint(iter));
		}
	} else {
		Exception mixup("Attempted to add degenerate path to optimizer boundary");
		throw mixup;
	}
}

void pather_optimizer::addBoundary(const Loop& loop) {
		//boundaries are broken down into linesegments
	if(loop.size() > 2) {
		for(Loop::const_finite_cw_iterator iter = loop.clockwiseFinite(); 
				iter != loop.clockwiseEnd(); 
				++iter) {
			boundaries.push_back(loop.segmentAfterPoint(iter));
		}
	} else {
		Exception mixup("Attempted to add degenerate loop to optimizer boundary");
		throw mixup;
	}
}

void pather_optimizer::clearBoundaries() {
	boundaries.clear();
}

void pather_optimizer::clearPaths() {
	myLoops.clear();
	myPaths.clear();
}

LabeledOpenPath pather_optimizer::closestLoop( 
		LabeledLoopList::iterator loopIter, 
		Loop::entry_iterator entryIter) {
	//we have found a closest loop
	LabeledOpenPath retLabeled;
	Loop::cw_iterator cwIter = loopIter->myPath.clockwise(*entryIter);
	Loop::ccw_iterator ccwIter = loopIter->myPath.counterClockwise(*entryIter);
	if(cwIter == loopIter->myPath.clockwiseEnd() || 
			ccwIter == loopIter->myPath.counterClockwiseEnd()) {
		std::stringstream msg;
		msg << "Invalid entryIterator in pather_optimizer::closestLoop" 
				<< std::endl;
		Exception mixup(msg.str());
		throw mixup;
	}
	LoopPath lp(loopIter->myPath, 
			cwIter, 
			ccwIter);
	//turn into into a path
	retLabeled.myPath.appendPoints(lp.fromStart(), lp.end());
	retLabeled.myLabel = loopIter->myLabel;
	if(retLabeled.myPath.size() < 3) {
		std::stringstream msg;
		msg << "Degenerate path of size " <<retLabeled.myPath.size() << 
				" from loop of size " << loopIter->myPath.size() << std::endl;
		Exception mixup(msg.str());
		myLoops.erase(loopIter);
		throw mixup;
	}
	//remove it from our "things to optimize"
	myLoops.erase(loopIter);
	return retLabeled;
}

LabeledOpenPath pather_optimizer::closestPath( 
		LabeledPathList::iterator pathIter, 
		OpenPath::entry_iterator entryIter) {
	//we have found a closest path
	LabeledOpenPath retLabeled;
	//extract it in the right order
	if(*entryIter == *(pathIter->myPath.fromStart())) {
		retLabeled.myPath = pathIter->myPath;
	} else {
		retLabeled.myPath.appendPoints(
				pathIter->myPath.fromEnd(), pathIter->myPath.rend());
	}
	retLabeled.myLabel = pathIter->myLabel;
	if(retLabeled.myPath.size() < 2) {
		std::stringstream msg;
		msg << "Degenerate path of size " <<retLabeled.myPath.size() << 
				" from path of size " << pathIter->myPath.size() << std::endl;
		Exception mixup(msg.str().c_str());
		myPaths.erase(pathIter);
		throw mixup;
	}
	//remove it from our "things to optimize"
	myPaths.erase(pathIter);
	return retLabeled;
}

void pather_optimizer::findClosestLoop(const PointType& point, 
		LabeledLoopList::iterator& loopIter, 
		Loop::entry_iterator& entryIter) {
	
	if(myLoops.empty()) {
		loopIter = myLoops.end();
		return;
	}
	loopIter = myLoops.begin();
	entryIter = loopIter->myPath.entryBegin();
	Scalar closestDistance = (point - *entryIter).magnitude();
	
	for(LabeledLoopList::iterator currentIter = myLoops.begin(); 
			currentIter != myLoops.end(); 
			++currentIter) {
		for(Loop::entry_iterator currentEntry = 
				currentIter->myPath.entryBegin(); 
				currentEntry != currentIter->myPath.entryEnd(); 
				++currentEntry) {
			Scalar distance = (point - 
					*(currentEntry)).magnitude();
			if(libthing::tlower(distance, closestDistance, 
					DISTANCE_THRESHOLD)) {
				closestDistance = distance;
				entryIter = currentEntry;
				loopIter = currentIter;
			}
		}
	}
}

void pather_optimizer::findClosestPath(const PointType& point, 
		LabeledPathList::iterator& pathIter, 
		OpenPath::entry_iterator& entryIter) {
	if(myPaths.empty()) {
		pathIter = myPaths.end();
		return;
	}
	pathIter = myPaths.begin();
	entryIter = pathIter->myPath.entryBegin();
	Scalar closestDistance = (point - *entryIter).magnitude();
	
	for(LabeledPathList::iterator currentIter = myPaths.begin(); 
			currentIter != myPaths.end(); 
			++currentIter) {
		for(OpenPath::entry_iterator currentEntry = 
				currentIter->myPath.entryBegin(); 
				currentEntry != currentIter->myPath.entryEnd(); 
				++currentEntry) {
			Scalar distance = (point - 
					*(currentEntry)).magnitude();
			if(libthing::tlower(distance, closestDistance, 
					DISTANCE_THRESHOLD)) {
				closestDistance = distance;
				entryIter = currentEntry;
				pathIter = currentIter;
			}
		}
	}
}

bool pather_optimizer::closest(const PointType& point, LabeledOpenPath& result) {
	LabeledLoopList::iterator loopIter;
	Loop::entry_iterator loopEntry;
	LabeledPathList::iterator pathIter;
	OpenPath::entry_iterator pathEntry;
	
	findClosestLoop(point, loopIter, loopEntry);
	findClosestPath(point, pathIter, pathEntry);
	
	if(loopIter != myLoops.end() && pathIter != myPaths.end()) {
		//pick best
		Scalar loopDistance = (point - *loopEntry).magnitude();
		Scalar pathDistance = (point - *pathEntry).magnitude();
		if(libthing::tlower(loopDistance, pathDistance, 
				DISTANCE_THRESHOLD)) {
			//loop wins
			result = closestLoop(loopIter, loopEntry);
		} else {
			result = closestPath(pathIter, pathEntry);
		}
	} else if(loopIter != myLoops.end()) {
		//pick loop
		result = closestLoop(loopIter, loopEntry);
	} else if(pathIter != myPaths.end()) {
		//pick path
		result = closestPath(pathIter, pathEntry);
	} else {
		return false;
	}
	return true;
}

bool pather_optimizer::crossesBoundaries(const libthing::LineSegment2& seg) {
	//test if this linesegment crosses any boundaries
	for(BoundaryList::const_iterator iter = 
			boundaries.begin(); 
			iter != boundaries.end(); 
			++iter) {
		const libthing::LineSegment2& currentBoundary = *iter;
		if(seg.intersects(currentBoundary))
			return true;
	}
	return false;
}

}



