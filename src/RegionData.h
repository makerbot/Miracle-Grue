/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/

#ifndef REGION_DATA_H_
#define REGION_DATA_H_


#include <vector>
#include <fstream>
#include "DataEnvelope.h"


struct Material
{
	std::string name;
	int extruder;
	bool support;
};

struct Contour
{
	std::string materialName;
	mgl::Polygon contour;
	bool isHole; // true if the contour is a hole
};

//
// The Region data is contains polygons
// polygons for each extruders
//
class RegionData: public DataEnvelope {

public:
	double positionZ;
	double layerThickness;


	RegionData (double z, double thick);
	virtual ~RegionData();

	std::vector<Material> materials;
	std::vector<Contour> contours;

	void writeJsonContour(std::ostream &out) const;

};



#endif // REGION_DATA_H_
