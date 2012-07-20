#ifndef LOOP_UTILS_H
#define	LOOP_UTILS_H

#include "loop_path.h"

namespace mgl {

class AngleFunctor {
public:
	AngleFunctor(PointType ref) : reference(ref) {}
	bool operator () (const PointType& l, const PointType& r) const {
		Scalar sinl = (l-reference).unit().crossProduct((r-reference).unit());
		Scalar cosl = (l-reference).unit().dotProduct((r-reference).unit());
		Scalar ret = atan2(sinl, cosl);
		//std::cout << ret << std::endl;
		return ret < 0;
	}
private:
	PointType reference;
};

template <template <class, class> class COLLECTION, class ALLOC>
Loop createConvexLoop(const COLLECTION<Loop, ALLOC>& input){	
	std::vector<PointType> points;
	size_t extremeIndex = 0;
	/* Assemble all points in a vector, also choose the bottom left */
	for(typename COLLECTION<Loop, ALLOC>::const_iterator iter = input.begin(); 
			iter != input.end(); 
			++iter) {
		for(Loop::const_finite_cw_iterator loopiter = iter->clockwiseFinite(); 
				loopiter != iter->clockwiseEnd(); 
				++loopiter) {
			points.push_back(*loopiter);
			if(loopiter->getPoint().y < points[extremeIndex].y || 
					loopiter->getPoint().x < points[extremeIndex].x) {
				extremeIndex = points.size()-1;
			}
		}
	}
	if(points.empty()) {
		Exception mixup("Attempting convex hull on empty list of loops or empty loops!");
		throw mixup;
	}
	/* Place the bottom left point in index 0 */
	std::swap(points[0], points[extremeIndex]);
	extremeIndex = 0;
	
	/* Sort in a counterclockwise way */
	std::vector<PointType>::iterator beginIter = points.begin();
	++beginIter;
	std::vector<PointType>::iterator endIter = points.end();
	std::sort(beginIter, endIter, 
			AngleFunctor(points[extremeIndex]));
	
	std::vector<PointType> pointsUnique;
	pointsUnique.push_back(points.front());
	/* Remove duplicates */
	for(std::vector<PointType>::iterator iter = points.begin() + 1; 
			iter != points.end(); 
			++iter) {
		if(*iter == *(iter - 1)){
			continue;
		} else {
			pointsUnique.push_back(*iter);
		}
	}
	
	Loop retLoop;
	
	for(size_t i = 0; i < pointsUnique.size(); ++i) {
		retLoop.insertPointBefore(pointsUnique[i], retLoop.clockwiseEnd());
	}
	
	bool seenConCave = false;
	do {
		seenConCave = false;
		bool init = false;
		for(Loop::cw_iterator iter = retLoop.clockwise(); 
				iter != retLoop.clockwise() || !init;  
				++iter, init = true) {
			Loop::cw_iterator left = iter;
			Loop::cw_iterator right = iter;
			--left;
			++right;
			if(AngleFunctor(*iter).operator ()(*left, *right)) {
				Loop tmp;
				for(Loop::finite_cw_iterator iter2 = retLoop.clockwiseFinite(); 
						iter2 != retLoop.clockwiseEnd(); 
						++iter2) {
					if(iter2 != iter)
						tmp.insertPointBefore(*iter2, tmp.clockwiseEnd());
				}
				retLoop = tmp;
				iter = retLoop.clockwise();
				seenConCave = true;
			}
		}
	} while(seenConCave);
	
	return retLoop;
}

}


#endif	/* LOOP_UTILS_H */

