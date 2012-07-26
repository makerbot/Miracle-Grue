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
#include <vector>

namespace mgl {

class pather_optimizer {
public:
	bool linkPaths;
	pather_optimizer(bool lp = false);
	typedef std::list<libthing::LineSegment2> BoundaryList;
	typedef std::list<LabeledOpenPath> LabeledPathList;
	typedef std::list<LabeledLoop> LabeledLoopList;
	
	template <template<class, class> class PATHS, typename ALLOC>
	void optimize(PATHS<OpenPath, ALLOC>& paths) {
		//this will empty all the internal containers
		PointType lastPoint;
		LabeledOpenPath currentClosest;
		if(!myLoops.empty())
			lastPoint = *(myLoops.begin()->myPath.entryBegin());
		while(!myLoops.empty() || !myPaths.empty()) {
			try {
				while(closest(lastPoint, currentClosest)) {
					lastPoint = *(currentClosest.myPath.fromEnd());
					paths.push_back(currentClosest.myPath);
				}
			} catch(Exception mixup) {
				Log::severe() << "ERROR: " << mixup.what() << std::endl;
			}
		}
		//if moves don't cross boundaries, ok to extrude them
		if(linkPaths) {
			link(paths); 
		}
	}
	template <template <class, class> class LABELEDPATHS, typename ALLOC>
	void optimize(LABELEDPATHS<LabeledOpenPath, ALLOC>& labeledpaths) {
		PointType lastPoint;
		LabeledOpenPath currentClosest;
		if(!myLoops.empty())
			lastPoint = *(myLoops.begin()->myPath.entryBegin());
		while(!myLoops.empty() || !myPaths.empty()) {
			try {
				while(closest(lastPoint, currentClosest)) {
					lastPoint = *(currentClosest.myPath.fromEnd());
					labeledpaths.push_back(currentClosest);
				}
			} catch(Exception mixup) {
				Log::severe() << "ERROR: " << mixup.what() << std::endl;
			}
		}
		//if moves don't cross boundaries, ok to extrude them
		if(linkPaths) {
			link(labeledpaths);
		}
	}
	template <template <class, class> class PATHS, typename PATH, typename ALLOC>
	void addPaths(const PATHS<PATH, ALLOC>& paths, 
			const PathLabel& label = 
			PathLabel(PathLabel::TYP_INSET, PathLabel::OWN_MODEL, 0)) {
		for(typename PATHS<PATH, ALLOC>::const_iterator iter = paths.begin(); 
				iter != paths.end(); 
				++iter) {
			try {
				addPath(*iter, label); 
			} catch(Exception mixup) {
				Log::severe() << "ERROR: " << mixup.what() << std::endl;
			}
		}
	}
	template <template <class, class> class INSETS, typename ALLOC>
	void addInsets(const INSETS<Loop, ALLOC>& insets, 
			const PathLabel& label = 
			PathLabel(PathLabel::TYP_INSET, PathLabel::OWN_MODEL, 0)) {
		/*
		 insets - a single set of insets, outermost inset first
		 this function will preprocess insets to do them from inside out
		 and connect them in a smart way
		 */
		typedef typename INSETS<Loop, ALLOC>::const_reverse_iterator 
				const_reverse_iterator;
		const_reverse_iterator current = insets.rbegin();
		if(insets.empty())
			return;
		OpenPath result;
		
		LoopPath currentLp(*current, current->clockwise(), 
				current->counterClockwise(*(current->clockwise())));
		result.appendPoints(currentLp.fromStart(), currentLp.end());
		++current;
		for(; current != insets.rend(); ++current) {
			//break;
			const Loop& currentLoop = *current;
			Loop::entry_iterator nearestIter = currentLoop.clockwiseFinite();
			Scalar nearestDist = (*(result.fromEnd()) - 
					(*nearestIter)).magnitude();
			for(Loop::entry_iterator iter = currentLoop.clockwiseFinite(); 
					iter != currentLoop.entryEnd(); 
					++iter) {
				Scalar dist = (*(result.fromEnd()) - 
						(*iter)).magnitude();
				if(dist < nearestDist) {
					nearestDist = dist;
					nearestIter = iter;
				}
			}
			Loop::const_cw_iterator nextNearest(nearestIter);
			++nextNearest;
			PointType nextPoint = *nextNearest;
			currentLp = LoopPath(currentLoop, currentLoop.clockwise(nextPoint), 
					currentLoop.counterClockwise(nextPoint));
			result.appendPoints(currentLp.fromStart(), currentLp.end());
		}
		addPath(result, label);
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
	template <template <class, class> class LABELEDPATHS, typename ALLOC>
	void link(LABELEDPATHS<LabeledOpenPath, ALLOC>& labeledpaths) {
		//connect paths if between them the movement not crosses boundaries
		typedef typename LABELEDPATHS<LabeledOpenPath, ALLOC>::iterator iterator;
		for(iterator iter = labeledpaths.begin(); 
				iter != labeledpaths.end(); 
				++iter) {
			iterator lastIter;
			if(iter != labeledpaths.begin()) {
				lastIter= iter;
				--lastIter;
				LabeledOpenPath& last = *lastIter;
				LabeledOpenPath& current = *iter;
				PointType lastPoint = *(last.myPath.fromEnd());
				PointType currentPoint = *(current.myPath.fromStart());
				if(current.myLabel.myType == PathLabel::TYP_CONNECTION || 
						last.myLabel.myType == PathLabel::TYP_CONNECTION)
					continue;
				if(lastPoint == currentPoint)
					continue;
				libthing::LineSegment2 transition(lastPoint, currentPoint);
				if(crossesBoundaries(transition))
					continue;
				LabeledOpenPath connection;
				connection.myPath.appendPoint(lastPoint);
				connection.myPath.appendPoint(currentPoint);
				connection.myLabel.myType = PathLabel::TYP_CONNECTION;
				if(last.myLabel == current.myLabel && false) {
					//naive case
					//concatenate paths of same label
					last.myPath.appendPoints(current.myPath.fromStart(), 
							current.myPath.end());
					iter = labeledpaths.erase(iter);
					--iter;
				} else {
					//smart case
					//properly identify this as connectivity
					iter = labeledpaths.insert(iter, connection);
				}
			}
		}
	}
	bool crossesBoundaries(const libthing::LineSegment2& seg);
	BoundaryList boundaries;
	LabeledLoopList myLoops;
	LabeledPathList myPaths;
};

}



#endif	/* PATHER_OPTIMIZER_H */

