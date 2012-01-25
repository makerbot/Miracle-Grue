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

#define GRUE_VERSION  "v0.0.1.0"
#define GRUE_PROGRAM_NAME  "Miracle-Grue"

// #include <stdio.h>
#include <vector>
#include <string>

#include "json-cpp/include/json/value.h"
#include "json-cpp/include/json/writer.h"

//
// This class contains settings for the 3D printer, and user preferences
//
class Configuration {

	public:
		Json::Value root;      	/// root Json Dictionary to contain all settings

    public:
		/// standard constructor
		Configuration();
		/// standard constructor for loading configuration from a json file
        //    Configuration(std::string& srcFilename);
 		/// standard  destructor
         ~Configuration();

         void readFromFile(const char* filename);

    public:


     	/// index function, to read/write values as config["foo"]
     	Json::Value& operator[] (const std::string key)
		{
     		return this->root[key];
		}

     	/// test function for python style key existance checking
     	bool contains(const std::string &key ) const { return this->root.isMember(key); }

     	/// test function for C/C++ style key existance checking
     	bool isMember( const std::string &key ) const { return this->root.isMember(key); }

     	void writeJsonConfig(std::ostream &out) const;

 		/// helper functions to easily print/view the config values as json
      	std::string jsonFromExtruder(Json::Value& extruder) const;
      	std::string asJson(Json::StyledWriter writer = Json::StyledWriter()) const;


};

#endif /* CONFIGURATION_H_ */
