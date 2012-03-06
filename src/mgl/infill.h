/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/


#ifndef INFILL_H_
#define INFILL_H_

#include "core.h"
#include "limits.h"
#include "segment.h"

namespace mgl
{
//
// Pathology: the operation that generate paths from polygons
//
void infillPathology( SegmentTable &outlineSegments,
				const Limits& limits,
				double z,
				double tubeSpacing ,
				Scalar infillShrinking,
				Polygons &tubes);


}
#endif
