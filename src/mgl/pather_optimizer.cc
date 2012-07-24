#include <list>
#include <limits.h>

#include "pather_optimizer.h"

namespace mgl {

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
		std::list<LabeledLoop>::iterator loopIter, 
		Loop::entry_iterator entryIter) {
	//we have found a closest loop
	LabeledOpenPath retLabeled;
	LoopPath lp(loopIter->myPath, 
			loopIter->myPath.clockwise(*entryIter), 
			loopIter->myPath.counterClockwise(*entryIter));
	//turn into into a path
	retLabeled.myPath.appendPoints(lp.fromStart(), lp.end());
	retLabeled.myLabel = loopIter->myLabel;
	if(retLabeled.myPath.size() < 2) {
		Exception mixup("Degenerate path in pather_optimizer!");
		throw mixup;
	}
	//remove it from our "things to optimize"
	myLoops.erase(loopIter);
	return retLabeled;
}

LabeledOpenPath pather_optimizer::closestPath( 
		std::list<LabeledOpenPath>::iterator pathIter, 
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
	if(retLabeled.myPath.size() < 3) {
		Exception mixup("Degenerate loop in pather_optimizer!");
		throw mixup;
	}
	//remove it from our "things to optimize"
	myPaths.erase(pathIter);
	return retLabeled;
}

void pather_optimizer::findClosestLoop(const PointType& point, 
		std::list<LabeledLoop>::iterator& loopIter, 
		Loop::entry_iterator& entryIter) {
	
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
		std::list<LabeledOpenPath>::iterator& pathIter, 
		OpenPath::entry_iterator& entryIter) {
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

bool pather_optimizer::closest(const PointType& point, LabeledOpenPath& result) {
	std::list<LabeledLoop>::iterator loopIter;
	Loop::entry_iterator loopEntry;
	std::list<LabeledOpenPath>::iterator pathIter;
	OpenPath::entry_iterator pathEntry;
	
	findClosestLoop(point, loopIter, loopEntry);
	findClosestPath(point, pathIter, pathEntry);

	if(loopIter != myLoops.end() && pathIter != myPaths.end()) {
		//pick best
		Scalar loopDistance = (point - *loopEntry).magnitude();
		Scalar pathDistance = (point - *pathEntry).magnitude();
		if(loopDistance < pathDistance) {
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
	std::cout << "Size of closest thing: " << result.myPath.size() << std::endl;
	return true;
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



