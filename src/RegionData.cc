/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/
#include <iostream>
#include "RegionData.h"

using namespace std;

RegionData::RegionData(double z, double thick)
		 :positionZ(z),
		 layerThickness(thick)
{
	cout << "RegionData() @"  << this<< endl;
}

RegionData::~RegionData()
{
	cout << "~RegionData() @"  << this<< endl;

}

void RegionData::writeJsonContour(std::ostream &out) const
{
	cout << "writing json contours"<<endl;
}



