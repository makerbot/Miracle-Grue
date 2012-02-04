/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/

#ifndef SHELL_ENVELOPE_H
#define SHELL_ENVELOPE_H

#include <string>
#include "../DataEnvelope.h"

#include "../BGL/BGLCompoundRegion.h"


class ShellEnvelope: public DataEnvelope {

public:
	BGL::CompoundRegion perimeter;
	double zLayer;
	double svgWidth, svgHeight, svgXOff, svgYOff;
public:

	ShellEnvelope(BGL::CompoundRegion inPerimeter, double inZLayer,
			double inSvgWidth, double inSvgHeight, double inSvgXOff, double inSvgYOff);
	~ShellEnvelope();
};


#endif // SHELL_ENVELOPE_H
