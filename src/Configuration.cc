/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/
#include "Configuration.h"

#include <iostream>
#include <sstream>

#include "json-cpp/include/json/reader.h"
#include "json-cpp/include/json/writer.h"

using namespace std;

Configuration::Configuration()
{
	this->root["programName"]  = GRUE_PROGRAM_NAME;
	this->root["versionStr"]  = GRUE_VERSION,
	this->root["machineName"] = "";
	this->root["firmware"] = "";

	Json::Value platform;
	platform["temperature"] = 0;
	platform["automated"] = false;
	platform["waitingPositionX"] = 0;
	platform["waitingPositionY"] = 0,
	platform["waitingPositionZ"] = 0;
	this->root["platform"] = platform;

	Json::Value shell;
	shell["enable"] = false;
	shell["distance"] = 3.0;

	root["shell"] = shell;
	cout << "Configuration ctor! " << endl;

	Json::Value gcodeOutput;
	gcodeOutput["filename"] = "";
	this->root["shell"] = shell;
}


Configuration::Configuration(std::string& srcFilename)
{
	this->root["programName"]  = GRUE_PROGRAM_NAME;
	this->root["versionStr"]  = GRUE_VERSION,
	this->root["machineName"] = "";
	this->root["firmware"] = "";

	Json::Reader reader;
	// root built in default constructor
	bool parsedOk = reader.parse(srcFilename, root);
	if(parsedOk){
		cout << "parsed ok";
	}
}


Configuration::~Configuration()
{
	cout << __FUNCTION__ << endl;
	this->root.clear();
}

std::string Configuration::asJson(Json::StyledWriter writer ) const
{
	return writer.write(root);
}


void Configuration::writeJsonConfig(ostream &ss) const
{
	Json::StyledWriter writer;
	ss << this->asJson();
}

