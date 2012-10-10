#ifndef UNIT_TEST_UTIL_H_
#define UNIT_TEST_UTIL_H_

#include "mgl/mgl.h"

void clip(const std::vector<libthing::Segment2Type> &in,
			unsigned int min,
			unsigned int max,
			std::vector<libthing::Segment2Type> &out);

void  mkDebugPath(const char *path);


#endif

