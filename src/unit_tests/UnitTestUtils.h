#ifndef UNIT_TEST_UTIL_H_
#define UNIT_TEST_UTIL_H_

#include "mgl/mgl.h"

void clip(const std::vector<mgl::Segment2Type> &in,
			unsigned int min,
			unsigned int max,
			std::vector<mgl::Segment2Type> &out);

void  mkDebugPath(const char *path);


#endif

