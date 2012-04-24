#ifndef INSET_TESTS_H_
#define INSET_TESTS_H_

#include "mgl/shrinky.h"



void clip(const std::vector<libthing::LineSegment2> &in,
			unsigned int min,
			unsigned int max,
			std::vector<libthing::LineSegment2> &out);

#endif
