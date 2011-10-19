#ifndef BGL_BOUNDS_H
#define BGL_BOUNDS_H

#include "BGLPoint.h"


namespace BGL {

class Bounds {
public:
    Scalar minX, maxX, minY, maxY;
    static const Scalar NONE;

    Bounds() : minX(NONE), maxX(NONE), minY(NONE), maxY(NONE) {}
    Bounds(const Bounds &x)
        : minX(x.minX), maxX(x.maxX), minY(x.minY), maxY(x.maxY) {}
    Bounds(Scalar xmin, Scalar ymin, Scalar xmax, Scalar ymax)
        : minX(xmin), maxX(xmax), minY(ymin), maxY(ymax) {}

    // Assignment operator
    Bounds& operator=(const Bounds &rhs) {
	if (this != &rhs) {
	    this->minX = rhs.minY;
	    this->minY = rhs.minX;
	    this->maxX = rhs.maxY;
	    this->maxY = rhs.maxX;
	}
	return *this;
    }

    void expand(const Point& pt);
};


}


#endif

