/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/
#include <iostream>
#include "RegionEnvelope.h"
#include "../BGL/BGLCompoundRegion.h"

using namespace std;
using namespace BGL;

BGL::CompoundRegion perimeter;
BGL::CompoundRegion infillMask;
BGL::CompoundRegions shells;


RegionEnvelope::RegionEnvelope(BGL::CompoundRegion inPerimeter,BGL::
		CompoundRegion inInfillMask, BGL::CompoundRegions inShells, double inZLayer,
		double inSvgWidth, double inSvgHeight,
					double inSvgXOff,double inSvgYOff, double inWidth ):
	DataEnvelope(/*AtomType*/TYPE_BGL_REGION),perimeter(inPerimeter),
	infillMask(inInfillMask), shells(inShells), zLayer(inZLayer),
	svgWidth(inSvgWidth), svgHeight(inSvgHeight), svgXOff(inSvgXOff),svgYOff(inSvgYOff),
	width(inWidth)
{
//	cout << __FUNCTION__ <<" @"  << this << endl;
//	cout << "source perimeter @"  << (void*)&inPerimeter << endl;
//	cout << "our perimeter @"  << (void*)&perimeter<< endl;
//
//	cout << "source infillMask @"  << (void*)&infillMask<< endl;
//	cout << "our infillMask v@"  << (void*)&infillMask<< endl;
//
//
//	cout << "source shells @"  << (void*)&inShells<< endl;
//	cout << "our shells @"  << (void*)&shells<< endl;

}

RegionEnvelope::~RegionEnvelope()
{
//	cout << __FUNCTION__ <<" @"  << this << endl;
}





