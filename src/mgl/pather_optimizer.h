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

class abstract_optimizer {
public:
    abstract_optimizer(bool j = true) : jsonErrors(j) {}
	typedef std::list<LabeledOpenPath> LabeledOpenPaths;
	//optimize everything you have accumulated
	//calls to the internal optimize
	template <template<class, class> class PATHS, typename ALLOC>
	void optimize(PATHS<OpenPath, ALLOC>& paths) {
		LabeledOpenPaths result;
		optimizeInternal(result);
		for(LabeledOpenPaths::const_iterator iter = result.begin(); 
				iter != result.end(); 
				++iter) {
			paths.push_back(iter->myPath);
		}
	}
	template <template <class, class> class LABELEDPATHS, typename ALLOC>
	void optimize(LABELEDPATHS<LabeledOpenPath, ALLOC>& labeledpaths) {
		LabeledOpenPaths result;
		optimizeInternal(result);
		labeledpaths.insert(labeledpaths.end(), result.begin(), result.end());
	}
	
	//add paths to optimize
	//one label for entire collection
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
                if(jsonErrors) {
                    exceptionToJson(Log::severe(), mixup, true);
                } else {
                    Log::severe() << "WARNING: " << mixup.what() << std::endl;
                }
			}
		}
	}
	//each element has its own label
	template <template <class, class> class PATHS, typename PATH, typename ALLOC>
	void addPaths(const PATHS<basic_labeled_path<PATH>, ALLOC>& paths) {
		for(typename PATHS<basic_labeled_path<PATH>, ALLOC>::const_iterator 
				iter = paths.begin(); 
				iter != paths.end(); 
				++iter) {
			try {
				addPath(*iter); 
			} catch(Exception mixup) {
				if(jsonErrors) {
                    exceptionToJson(Log::severe(), mixup, true);
                } else {
                    Log::severe() << "WARNING: " << mixup.what() << std::endl;
                }
			}
		}
	}
	//singular version of above
	//convenience, calls one of the overloads
	template <typename PATH>
	void addPath(const basic_labeled_path<PATH>& labeledpath) {
		addPath(labeledpath.myPath, labeledpath.myLabel);
	}
	virtual void addPath(const OpenPath& path, 
			const PathLabel& label = 
			PathLabel(PathLabel::TYP_INSET, PathLabel::OWN_MODEL, 0)) = 0;
	virtual void addPath(const Loop& loop, 
			const PathLabel& label = 
			PathLabel(PathLabel::TYP_INSET, PathLabel::OWN_MODEL, 0)) = 0;
	
	//add boundaries
	template <template<class, class> class PATHS, typename PATH, typename ALLOC>
	void addBoundaries(const PATHS<PATH, ALLOC>& paths) {
		for(typename PATHS<PATH, ALLOC>::const_iterator iter = paths.begin(); 
				iter != paths.end(); 
				++iter) {
			try {
				addBoundary(*iter);
			} catch(Exception mixup) {
                if(jsonErrors) {
                    exceptionToJson(Log::severe(), mixup, true);
                } else {
                    std::cout << "WARNING: " << mixup.what() << std::endl;
                }
			}
		}
	}
	virtual void addBoundary(const OpenPath& path) = 0;
	virtual void addBoundary(const Loop& loop) = 0;
	
	//clear internal containers
	virtual void clearBoundaries() = 0;
	virtual void clearPaths() = 0;
protected:
	
	//labeledpaths is the output of optimization
	virtual void optimizeInternal(LabeledOpenPaths& labeledpaths) = 0;
    //do print exceptions as json?
    bool jsonErrors;
};

class pather_optimizer : public abstract_optimizer {
public:
	
	static Scalar DISTANCE_THRESHOLD;

	typedef std::list<libthing::LineSegment2> BoundaryList;
	typedef std::list<LabeledOpenPath> LabeledPathList;
	typedef std::list<LabeledLoop> LabeledLoopList;
	
	void addPath(const OpenPath& path, 
			const PathLabel& label = 
			PathLabel(PathLabel::TYP_INSET, PathLabel::OWN_MODEL, 0));
	void addPath(const Loop& loop, 
			const PathLabel& label = 
			PathLabel(PathLabel::TYP_INSET, PathLabel::OWN_MODEL, 0));
	void addBoundary(const OpenPath& path);
	void addBoundary(const Loop& loop);
	void clearBoundaries();
	void clearPaths();
protected:
	void optimizeInternal(abstract_optimizer::LabeledOpenPaths& labeledpaths);
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
	void link(abstract_optimizer::LabeledOpenPaths& labeledpaths);
	bool crossesBoundaries(const libthing::LineSegment2& seg);
	BoundaryList boundaries;
	LabeledLoopList myLoops;
	LabeledPathList myPaths;
};

}



#endif	/* PATHER_OPTIMIZER_H */

