/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/


#ifndef INSETS_H_
#define INSETS_H_

#include <ostream>
#include <algorithm>
#include <list>
#include <set>

#include "mgl.h"

namespace mgl
{




void inshelligence( const libthing::SegmentVector & outlinesSegments,
					unsigned int nbOfShells,
					double layerW,
					unsigned int sliceId,
					Scalar insetDistanceFactor,
					const char *scadFile,
					bool writeDebugScadFiles,
					std::vector<Polygons> & insetPolys,
					libthing::SegmentVector& innerOutlinesSegments);


}

#endif
