#ifndef UNIT_TEST_UTIL_H_
#define UNIT_TEST_UTIL_H_

#include "libthing/LineSegment2.h"

void clip(const std::vector<libthing::LineSegment2> &in,
			unsigned int min,
			unsigned int max,
			std::vector<libthing::LineSegment2> &out);

void  mkDebugPath(const char *path);


#endif

