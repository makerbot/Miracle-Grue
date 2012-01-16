/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/

#ifndef __PATH_DATA_H_
#define __PATH_DATA_H_


#include <vector>

#include "DataEnvelope.h"

#include "mgl/core.h"

//
// The Path data is a dictionary of polygons
// there are multiple polygons for each extruders
// There is one PathData per slice
class PathData : public DataEnvelope, public mgl::SliceData
{
public:
	PathData (double z);

};




#endif // __PATH_DATA_H_
