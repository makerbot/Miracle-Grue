/*
 * Configuration.h
 *
 *  Created on: Oct 14, 2011
 *      Author: Hugo, Far
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
};

#endif /* CONFIGURATION_H_ */
