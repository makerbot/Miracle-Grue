#ifndef LOOP_UTILS_H
#define	LOOP_UTILS_H

#include "loop_path.h"
#include <set>

namespace mgl {

template <typename COMPARE = std::less<Scalar> >
class basic_anglefunctor {
public:
	basic_anglefunctor(PointType ref, const COMPARE& comp = COMPARE()) 
			: my_compare(comp), reference(ref) {}
	bool operator () (const PointType& l, const PointType& r) const {
		if(l == r || l == reference || r == reference)
			return false;
		PointType lvec = (l-reference).unit();
		PointType rvec = (r-reference).unit();
//		Scalar lang = atan2f(lvec.y, lvec.x);
//		Scalar rang = atan2f(rvec.y, rvec.x);
		return my_compare(rvec.crossProduct(lvec), 0);
	}
private:
	COMPARE my_compare;
	PointType reference;
};

template <typename COMPARE = std::less<Scalar> >
class basic_axisfunctor {
public:
	basic_axisfunctor(axis_e a = X_AXIS, const COMPARE& comp = COMPARE()) 
			: my_compare(comp), axis(a) {}
	bool operator () (const PointType& l, const PointType& r) const {
		switch(axis) {
		case X_AXIS:
			return l.x == r.x ? my_compare(l.y, r.y) : my_compare(l.x, r.x);
			break;
		case Y_AXIS:
			return l.y == r.y ? my_compare(l.x, r.x) : my_compare(l.y, r.y);
			break;
		default:
//			Exception mixup("Invalid axis enumeration in Axis Functor");
//			throw mixup;
			return false;
		}
	}
private:
	COMPARE my_compare;
	axis_e axis;
};

typedef basic_anglefunctor<> AngleFunctor;
typedef basic_axisfunctor<> AxisFunctor;

template <template <class, class> class COLLECTION, class T, class ALLOC, 
		class COMPARE >
void stripDuplicates(COLLECTION<T, ALLOC>& collection, 
		const COMPARE& comp) {
	std::set<T, COMPARE> uniqueset(comp);
	typedef typename COLLECTION<T, ALLOC>::iterator iterator;
	uniqueset.insert(collection.begin(), collection.end());
	collection.clear();
	collection.insert(collection.end(), uniqueset.begin(), uniqueset.end());
}

template <template <class, class> class COLLECTION, class ALLOC>
Loop createConvexLoop(const COLLECTION<Loop, ALLOC>& input){	
	std::vector<PointType> points;
	/* Assemble all points in a vector */
	for(typename COLLECTION<Loop, ALLOC>::const_iterator iter = input.begin(); 
			iter != input.end(); 
			++iter) {
		for(Loop::const_finite_cw_iterator loopiter = iter->clockwiseFinite(); 
				loopiter != iter->clockwiseEnd(); 
				++loopiter) {
			points.push_back(*loopiter);
		}
	}
	if(points.empty()) {
		Exception mixup(
				"Attempting convex hull on empty list of loops or empty loops!");
		throw mixup;
	}
	/* Remove duplicates */
	stripDuplicates(points, AxisFunctor());
	/* Sort on X */
	std::sort(points.begin(), points.end(), AxisFunctor(Y_AXIS));
	
	std::cout << "Point Count: " << points.size() << std::endl;
	for(std::vector<PointType>::iterator iter = points.begin(); 
			iter != points.end(); 
			++iter) {
		std::cout << "Point: " << *iter << std::endl;
	}
	std::cout << std::endl;
	
	Loop retLoop;
		
	std::vector<PointType>::iterator startIter = points.begin();
	std::vector<PointType>::iterator lastIter = points.begin();
	
	retLoop.insertPointBefore(*lastIter, retLoop.clockwiseEnd());
	
	do {
		std::vector<PointType>::iterator bestIter = lastIter;
		++bestIter;
		if(bestIter == points.end())
			bestIter = points.begin();
		std::cout << "Last Point:  " << *lastIter << std::endl;
		for(std::vector<PointType>::iterator iter = points.begin(); 
				iter != points.end(); 
				++iter ){
			if(AngleFunctor(*lastIter).operator ()(
					*bestIter, *iter)) {
				std::cout << *iter << " \t better than " << *bestIter << std::endl;
				bestIter = iter;
			} else {
				std::cout << *iter << " \t worse  than " << *bestIter << std::endl;
			}
//			retLoop.insertPointBefore(*iter, retLoop.clockwiseEnd());
		}
		std::cout << "Best Point:  " << *bestIter << std::endl;
		std::cout << "Start Point: " << *startIter << std::endl;
		lastIter = bestIter;
		retLoop.insertPointBefore(*lastIter, retLoop.clockwiseEnd());
	} while(lastIter != startIter);
	
	std::cout << "RetLoop Contents: " << retLoop.size() << std::endl;
	return retLoop;
}

}


#endif	/* LOOP_UTILS_H */

