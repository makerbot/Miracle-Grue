/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/

#ifndef CARVE_ENVELOPE_H
#define CARVE_ENVELOPE_H

#include <string>
#include "../DataEnvelope.h"
#include "../BGL/BGLCompoundRegion.h"


class RegionEnvelope: public DataEnvelope {

public:
	// perimeter contains the outer
	BGL::CompoundRegion perimeter;
	BGL::CompoundRegion infillMask;
	BGL::CompoundRegions shells;
    BGL::Paths infill;
    double zLayer;
	double svgWidth, svgHeight, svgXOff,svgYOff;
	double width;

public:

	RegionEnvelope(BGL::CompoundRegion inPerimeter,BGL::CompoundRegion inInfillMask,
			BGL::CompoundRegions inShells, double inZLayer,
			double inSvgWidth, double inSvgHeight,
			double inSvgXOff,double inSvgYOff, double inWidth );
	~RegionEnvelope();
};


#endif // CARVE_ENVELOPE_H
