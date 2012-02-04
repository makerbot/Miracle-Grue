/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/
#include <iostream>
#include "ShellEnvelope.h"
#include "../BGL/BGLCompoundRegion.h"

using namespace std;
using namespace BGL;


ShellEnvelope::ShellEnvelope(CompoundRegion inPerimeter, double inZLayer,
		double inSvgWidth, double inSvgHeight, double inSvgXOff, double inSvgYOff):
	DataEnvelope(/*AtomType*/TYPE_BGL_SHELL),
	perimeter(inPerimeter),zLayer(inZLayer),
	svgWidth(inSvgWidth), svgHeight(inSvgHeight), svgXOff(inSvgXOff),svgYOff(inSvgYOff)
{
//	cout << __FUNCTION__ <<" @"  << this << endl;
//	cout << "source perimeter @"  << (void*)&inPerimeter << endl;
//	cout << "our perimeter @"  << (void*)&perimeter << endl;

}

ShellEnvelope::~ShellEnvelope()
{
	cout << __FUNCTION__ <<" @"  << this << endl;
}





