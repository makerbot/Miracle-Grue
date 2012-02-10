/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/


#include <iostream>
#include <sstream>
#include <fstream>


#include "configuration.h"


using namespace mgl;
using namespace std;

Configuration::Configuration()
	:filename("")
{

}

void Configuration::readFromFile(const char* filename)
{
	this->filename = filename;
	std::ifstream file(filename, std::ifstream::binary);
	Json::Reader reader;
	reader.parse(file , root);
}

Configuration::~Configuration()
{
	// not sure we need to clean up here
	// this->root.clear();
}

std::string Configuration::asJson(Json::StyledWriter writer ) const
{
	return writer.write(root);
}

