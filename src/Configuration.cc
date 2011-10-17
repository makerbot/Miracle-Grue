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



using namespace std;

Configuration::Configuration()
{
	cout << "Configuration ctor! " << endl;

}

Configuration::~Configuration()
{
	cout << "Configuration dtor! " << endl;

}

std::string jsonFromExtruder(const Extruder &e)
{
	stringstream ss (stringstream::in | stringstream::out);
	ss << "{";
	ss << "'extrusionTemperature':" 	<< e.extrusionTemperature << ", ";
	ss << "'coordinateSystemOffsetX':" 	<< e.coordinateSystemOffsetX << ",";
	ss << "'extrusionTemperature':" 	<< e.extrusionTemperature  << ",";
	ss << "'defaultSpeed':" << e.defaultSpeed <<"} ";

	return ss.str();
}

void Configuration::writeJsonConfig(ostream &ss) const
{

	ss << "{" << endl;
	ss << "   // version info" << endl;
	ss << "   'version':'v_1_0_0_0',\n" << endl;
	ss << "    [" << endl;

	for (std::vector<Extruder>::const_iterator i= extruders.begin(); i!=extruders.end(); i++)
	{
		Extruder e = *i;
		if(i != extruders.begin() )
			ss << ", ";
		ss << jsonFromExtruder(e); // printJson(fp, e);
	}
	ss << endl <<  "] // extruders" << endl;
	ss << "}" << endl << endl;
}
