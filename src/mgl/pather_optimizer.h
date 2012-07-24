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
#include "log.h"
#include <list>

namespace mgl {

class pather_optimizer {
public:
	bool linkPaths;
	pather_optimizer(bool lp = false);
	
	template <template<class, class> class PATHS, typename ALLOC>
	void optimize(PATHS<OpenPath, ALLOC>& paths) {
		//this will empty all the internal containers
		PointType lastPoint;
		LabeledOpenPath currentClosest;
		std::cout << "Optimizing " << myLoops.size() << " loops and " 
				<< myPaths.size() << " paths" << std::endl;
		std::cout << "Sizes of Loops: " << std::endl;
		for(std::list<LabeledLoop>::iterator iter = myLoops.begin(); 
				iter != myLoops.end(); 
				++iter) {
			std::cout << iter->myPath.size() << std::endl;
		}
		std::cout << "Sizes of Paths: " << std::endl;
		for(std::list<LabeledOpenPath>::iterator iter = myPaths.begin(); 
				iter != myPaths.end(); 
				++iter) {
			std::cout << iter->myPath.size() << std::endl;
		}
		while(closest(lastPoint, currentClosest)) {
			std::cout << "Size of current closest path: " << 
					currentClosest.myPath.size() << std::endl;
			lastPoint = *(currentClosest.myPath.fromEnd());
			paths.push_back(currentClosest.myPath);
		}
		//if moves don't cross boundaries, ok to extrude them
		//if(linkPaths)
		//	link(paths);
	}
	template <template<class, class> class PATHS, typename PATH, typename ALLOC>
	void addPaths(const PATHS<PATH, ALLOC>& paths) {
		for(typename PATHS<PATH, ALLOC>::const_iterator iter = paths.begin(); 
				iter != paths.end(); 
				++iter) {
			try {
				addPath(*iter); 
			} catch(Exception mixup) {
				Log::severe() << "ERROR: " << mixup.what() << std::endl;
			}
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
			try {
				addBoundary(*iter);
			} catch(Exception mixup) {
				Log::severe() << "ERROR: " << mixup.what() << std::endl;
			}
		}
	}
	void addBoundary(const OpenPath& path);
	void addBoundary(const Loop& loop);
	void clearBoundaries();
	void clearPaths();
private:
	LabeledOpenPath closestLoop(std::list<LabeledLoop>::iterator loopIter, 
			Loop::entry_iterator entryIter);
	LabeledOpenPath closestPath(std::list<LabeledOpenPath>::iterator pathIter, 
			OpenPath::entry_iterator entryIter);
	void findClosestLoop(const PointType& point, 
			std::list<LabeledLoop>::iterator& loopIter, 
			Loop::entry_iterator& entryIter);
	void findClosestPath(const PointType& point, 
			std::list<LabeledOpenPath>::iterator& pathIter, 
			OpenPath::entry_iterator& entryIter);
	bool closest(const PointType& point, LabeledOpenPath& result);
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
				OpenPath& last = *lastIter;
				OpenPath& current = *iter;
				libthing::LineSegment2 transition(*(last.fromEnd()), 
						*(current.fromStart()));
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

