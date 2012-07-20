/* 
 * File:   pather_optimizer.h
 * Author: Dev
 *
 * Created on July 20, 2012, 12:27 PM
 */

#ifndef PATHER_OPTIMIZER_H
#define	PATHER_OPTIMIZER_H


#include "loop_path.h"
#include <list>

namespace mgl {

class pather_optimizer {
public:
	bool linkPaths;
	pather_optimizer(bool lp = false) : linkPaths(lp) {}
	
	template <template<class, class> class PATHS, typename ALLOC>
	void optimize(PATHS<OpenPath, ALLOC>& paths) {
		//this will empty all the internal containers
		while(!myLoops.empty()) {
			if(paths.empty()) {
				paths.push_back(closestLoop(myLoops.begin(), 
						myLoops.begin()->entryBegin()));
			} else {
				PointType lastPoint = *(paths.back().fromEnd());
				LoopList::iterator closestLoopIter = myLoops.begin();
				Loop::entry_iterator closestEntry = closestLoopIter->entryBegin();
				Scalar closestDistance = (lastPoint - 
						*(closestEntry)).magnitude();
				//find the nearest possible thing
				for(LoopList::iterator loopIter = myLoops.begin(); 
						loopIter != myLoops.end(); 
						++loopIter) {
					for(Loop::entry_iterator entryIter = loopIter->entryBegin(); 
							entryIter != loopIter->entryEnd(); 
							++entryIter) {
						Scalar distance = (lastPoint - 
								*(entryIter)).magnitude();
						if(distance < closestDistance) {
							closestDistance = distance;
							closestEntry = entryIter;
							closestLoopIter = loopIter;
						}
					}
				}
				//add it
				paths.push_back(closestLoop(closestLoopIter, closestEntry));
				
			}
		}
		while(!myPaths.empty()) {
			if(paths.empty()) {
				paths.push_back(closestPath(myPaths.begin(), 
						myPaths.begin()->entryBegin()));
			} else {
				PointType lastPoint = *(paths.back().fromEnd());
				OpenPathList::iterator closestPathIter = myPaths.begin();
				OpenPath::entry_iterator closestEntry = closestPathIter->entryBegin();
				Scalar closestDistance = (lastPoint - 
						*(closestEntry)).magnitude();
				//find the nearest possible thing
				for(OpenPathList::iterator pathIter = myPaths.begin(); 
						pathIter != myPaths.end(); 
						++pathIter) {
					for(OpenPath::entry_iterator entryIter = pathIter->entryBegin(); 
							entryIter != pathIter->entryEnd(); 
							++entryIter) {
						Scalar distance = (lastPoint - 
								*(entryIter)).magnitude();
						if(distance < closestDistance) {
							closestDistance = distance;
							closestEntry = entryIter;
							closestPathIter = pathIter;
						}
					}
				}
				//add it
				paths.push_back(closestPath(closestPathIter, closestEntry));
			}
		}
		if(linkPaths)
			link(paths);
	}
	template <template<class, class> class PATHS, typename PATH, typename ALLOC>
	void addPaths(const PATHS<PATH, ALLOC>& paths) {
		for(typename PATHS<PATH, ALLOC>::const_iterator iter = paths.begin(); 
				iter != paths.end(); 
				++iter) {
			addPath(*iter);
		}
	}
	void addPath(const OpenPath& path){
		if(path.size() > 1)
			myPaths.push_back(path);
	}
	void addPath(const Loop& loop) {
		if(loop.size() > 2)
			myLoops.push_back(loop);
	}
	template <template<class, class> class PATHS, typename PATH, typename ALLOC>
	void addBoundaries(const PATHS<PATH, ALLOC>& paths) {
		for(typename PATHS<PATH, ALLOC>::const_iterator iter = paths.begin(); 
				iter != paths.end(); 
				++iter) {
			addBoundary(*iter);
		}
	}
	void addBoundary(const OpenPath& path) {
		for(OpenPath::const_iterator iter = path.fromStart(); 
				iter != path.end(); 
				++iter) {
			boundaries.push_back(path.segmentAfterPoint(iter));
		}
	}
	void addBoundary(const Loop& loop) {
		for(Loop::const_finite_cw_iterator iter = loop.clockwiseFinite(); 
				iter != loop.clockwiseEnd(); 
				++iter) {
			boundaries.push_back(loop.segmentAfterPoint(iter));
		}
	}
	void clearBoundaries() {
		boundaries.clear();
	}
	void clearPaths() {
		myLoops.clear();
		myPaths.clear();
	}
private:
	OpenPath closestLoop(LoopList::iterator loopIter, 
			Loop::entry_iterator entryIter) {
		OpenPath ret;
		LoopPath lp(*loopIter, 
				loopIter->clockwise(*entryIter), 
				loopIter->counterClockwise(*entryIter));
		ret.appendPoints(lp.fromStart(), lp.end());
		myLoops.erase(loopIter);
		return ret;
	}
	OpenPath closestPath(OpenPathList::iterator pathIter, 
			OpenPath::entry_iterator entryIter) {
		OpenPath ret;
		if(*entryIter == *(pathIter->fromStart())) {
			ret = *pathIter;
		} else {
			ret.appendPoints(pathIter->fromEnd(), pathIter->rend());
		}
		myPaths.erase(pathIter);
		return ret;
	}
	template <template<class, class> class PATHS, typename ALLOC>
	void link(PATHS<OpenPath, ALLOC>& paths) {
		typedef typename PATHS<OpenPath, ALLOC>::iterator iterator;
		for(iterator iter = paths.begin(); 
				iter != paths.end(); 
				++iter) {
			iterator lastIter;
			if(iter != paths.begin()) {
				lastIter= iter;
				--lastIter;
				libthing::LineSegment2 transition(*(lastIter->fromEnd()), 
						*(iter->fromStart()));
				if(crossesBoundaries(transition))
					continue;
				lastIter->appendPoints(iter->fromStart(), iter->end());
				iter = paths.erase(iter);
				--iter;
			}
		}
	}
	bool crossesBoundaries(const libthing::LineSegment2& seg) {
		for(std::list<libthing::LineSegment2>::const_iterator iter = 
				boundaries.begin(); 
				iter != boundaries.end(); 
				++iter) {
			if(seg.intersects(*iter))
				return true;
		}
		return false;
	}
	std::list<libthing::LineSegment2> boundaries;
	LoopList myLoops;
	OpenPathList myPaths;
};

}



#endif	/* PATHER_OPTIMIZER_H */

