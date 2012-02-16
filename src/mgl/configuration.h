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

#include <sstream>
#include <vector>
#include <string>

#include "json-cpp/include/json/reader.h"
#include "json-cpp/include/json/writer.h"
#include "json-cpp/include/json/value.h"
#include "json-cpp/include/json/writer.h"

#include "core.h"

namespace mgl
{

class ConfigMess : public Messup {public: ConfigMess(const char *msg):Messup(msg) {	} };

// checks that the value exist before returning it
double doubleCheck(const Json::Value &value, const char *name);
unsigned int uintCheck(const Json::Value &value, const char *name);
std::string stringCheck(const Json::Value &value, const char *name);
bool boolCheck(const Json::Value &value, const char *name);
//
// This class contains settings for the 3D printer, and user preferences
//
class Configuration {

	std::string filename;
public:
	Json::Value root;

	public:
	//	Json::Value &readJsonValue() {return root;}
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
     	Json::Value& operator[] (const char* key)
		{

     		if(filename.length() ==0)
     		{
     			ConfigMess mixup("Configuration file has not been read");
     		    throw mixup;
     		}

     		if( !root.isMember(key) )
     		{
     			std::stringstream ss;
     			ss << "Can't find \"" << key << "\" in " << filename;
     			ConfigMess mixup(ss.str().c_str());
     			throw mixup;
     		}
     		return this->root[key];
		}

      	std::string asJson(Json::StyledWriter writer = Json::StyledWriter()) const;


};

}
#endif /* CONFIGURATION_H_ */
