/* 
 * File:   pather_optimizer.h
 * Author: Dev
 *
 * Created on July 20, 2012, 12:27 PM
 */

#ifndef PATHER_OPTIMIZER_H
#define	PATHER_OPTIMIZER_H


#include "loop_path.h"
#include "labeled_path.h"
#include <list>

namespace mgl {

class pather_optimizer {
public:
	bool linkPaths;
	pather_optimizer(bool lp = false);
	
	template <template<class, class> class PATHS, typename ALLOC>
	void optimize(PATHS<OpenPath, ALLOC>& paths) {
		//this will empty all the internal containers
		std::list<LabeledLoop>::iterator loopIter;
		Loop::entry_iterator loopEntry;
		std::list<LabeledOpenPath>::iterator pathIter;
		OpenPath::entry_iterator pathEntry;
		PointType lastPoint;
		
		findClosestLoop(lastPoint, loopIter, loopEntry);
		findClosestPath(lastPoint, pathIter, pathEntry);
		
		for(; loopIter != myLoops.end() || pathIter != myPaths.end(); 
				findClosestLoop(lastPoint, loopIter, loopEntry), 
				findClosestPath(lastPoint, pathIter, pathEntry)) {
			if(loopIter != myLoops.end() && pathIter != myPaths.end()) {
				//pick best
				Scalar loopDistance = (lastPoint - *loopEntry).magnitude();
				Scalar pathDistance = (lastPoint - *pathEntry).magnitude();
				if(loopDistance < pathDistance) {
					//loop wins
					closestLoop(loopIter, loopEntry);
				} else {
					closestPath(pathIter, pathEntry);
				}
			} else if(loopIter != myLoops.end()) {
				//pick loop
				closestLoop(loopIter, loopEntry);
			} else if(pathIter != myPaths.end()) {
				//pick path
				closestPath(pathIter, pathEntry);
			} else {
				Exception mixup("Something went wrong in pather_optimizer");
				throw mixup;
			}
			lastPoint = *(paths.back().fromEnd());
		}
		//if moves don't cross boundaries, ok to extrude them
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
	void addPath(const OpenPath& path, 
			const PathLabel& label = 
			PathLabel(PathLabel::TYP_INSET, PathLabel::OWN_MODEL, 0));
	void addPath(const Loop& loop, 
			const PathLabel& label = 
			PathLabel(PathLabel::TYP_INSET, PathLabel::OWN_MODEL, 0));
	template <template<class, class> class PATHS, typename PATH, typename ALLOC>
	void addBoundaries(const PATHS<PATH, ALLOC>& paths) {
		for(typename PATHS<PATH, ALLOC>::const_iterator iter = paths.begin(); 
				iter != paths.end(); 
				++iter) {
			addBoundary(*iter);
		}
	}
	void addBoundary(const OpenPath& path);
	void addBoundary(const Loop& loop);
	void clearBoundaries();
	void clearPaths();
private:
	OpenPath closestLoop(std::list<LabeledLoop>::iterator loopIter, 
			Loop::entry_iterator entryIter);
	OpenPath closestPath(std::list<LabeledOpenPath>::iterator pathIter, 
			OpenPath::entry_iterator entryIter);
	void findClosestLoop(const PointType& point, 
			std::list<LabeledLoop>::iterator loopIter, 
			Loop::entry_iterator entryIter);
	void findClosestPath(const PointType& point, 
			std::list<LabeledOpenPath>::iterator pathIter, 
			OpenPath::entry_iterator entryIter);
	template <template<class, class> class PATHS, typename ALLOC>
	void link(PATHS<OpenPath, ALLOC>& paths) {
		//connect paths if between them the movement not crosses boundaries
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
	bool crossesBoundaries(const libthing::LineSegment2& seg);
	std::list<libthing::LineSegment2> boundaries;
	std::list<LabeledLoop> myLoops;
	std::list<LabeledOpenPath> myPaths;
};

}



#endif	/* PATHER_OPTIMIZER_H */

