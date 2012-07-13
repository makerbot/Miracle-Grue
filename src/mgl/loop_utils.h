#ifndef LOOP_UTILS_H
#define	LOOP_UTILS_H

#include "loop_path.h"

namespace mgl {

class AngleFunctor {
public:
	AngleFunctor(PointType ref) : reference(ref) {}
	bool operator () (const PointType& l, const PointType& r) const {
		return (l-reference).crossProduct(r - reference) < 0.0;
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
		if(i < 2) {
			retLoop.insertPointBefore(pointsUnique[i], retLoop.clockwiseEnd());
			continue;
		}
		Loop::finite_cw_iterator last1(retLoop.clockwiseEnd());
		--last1;
		Loop::finite_cw_iterator last2 = last1;
		--last2;
		/* Here is where we fill the loop with the points, 
		 optionally dropping some */
		PointType currentPoint = pointsUnique[i];
		if(AngleFunctor(last2->getPoint())(
				currentPoint, last1->getPoint())) {
			/* point at last1 was not on the convex loop */
			last1->setPoint(currentPoint);
		} else {
			/* point at last1 was valid, next we will check currentPoint */
			retLoop.insertPointBefore(currentPoint, retLoop.clockwiseEnd());
		}
	}
	
	return retLoop;
}

}


#endif	/* LOOP_UTILS_H */

