/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_


// #include <stdio.h>
#include <vector>
#include <string>



/*
class Configuration
{


public:
	JSON::Value data;


};
*/

struct Platform
{
	double temperature;
};

struct Extruder
{
	//std::string color;
	//std::string plastic_type;
//public:
	double coordinateSystemOffsetX;
	double extrusionTemperature; // unit Celsius
	double defaultSpeed;
};

struct Outline
{
	bool enabled;
	float distance;
};

//
// This class contains settings for the 3D printer, and user preferences
//
class Configuration {

    public:
         Configuration();
         ~Configuration();

         void writeJsonConfig(std::ostream &out) const;

         std::string machineName;
         std::string firmware;

         Platform platform;
         std::vector<Extruder> extruders;

         std::string gcodeFilename;

         Outline outline;
};

#endif /* CONFIGURATION_H_ */
