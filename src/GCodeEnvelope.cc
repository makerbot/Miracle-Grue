/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/
#include <iostream>
#include "GCodeEnvelope.h"

using namespace std;

GCodeEnvelope::GCodeEnvelope(const char* str)
	:DataEnvelope(TYPE_ASCII_GCODE), gString(str)
{
//	cout << "GCodeEnvelope() @"  << this << endl;
//	cout << gString << endl << endl << endl;
}

GCodeEnvelope::~GCodeEnvelope()
{
//	cout << "~GCodeEnvelope() @"  << this << endl;
}




