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



struct Platform
{
	Platform():temperature(0),
				automated(false),
				waitingPositionX(0),
				waitingPositionY(0),
				waitingPositionZ(0)
	{}
	double temperature;				// temperature of the platform during builds
	bool automated;

	// the wiper(s) are affixed to the platform
    double waitingPositionX;
    double waitingPositionY;
    double waitingPositionZ;
};

struct Extruder
{
	Extruder(): coordinateSystemOffsetX(0), extrusionTemperature(0), defaultSpeed(0){}

	double coordinateSystemOffsetX;  // the distance along X between the machine 0 position and the extruder tip
	double extrusionTemperature; 	 // the extrusion temperature in Celsius
	double defaultSpeed;
};

struct Outline
{
	Outline() :enabled(false), distance(3.0){}
	bool enabled;   // when true, a rectangular ouline of the part will be performed
	float distance; // the distance in mm  between the model and the rectangular outline
};




//
// This class contains settings for the 3D printer, and user preferences
//
class Configuration {

    public:
         Configuration();
         ~Configuration();

         void writeJsonConfig(std::ostream &out) const;
         void writeGcodeConfig(std::ostream &out, const std::string indent) const;

         std::string programName;
         std::string versionStr;
         std::string machineName;	// 3D printer identifier
         std::string firmware;		// firmware revision


         Platform platform;
         std::vector<Extruder> extruders;	// list of extruder tools

         std::string gcodeFilename;			// output file name

         Outline outline;					// outline operation configuration

         double fastFeed;
};

#endif /* CONFIGURATION_H_ */
