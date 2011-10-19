/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/
#include <assert.h>
#include "GCoderOperation.h"


void GCoderOperation::init(Configuration& config)
{
	pConfig = &config;
	cout << "Writing GCODE to file: \"" << config.gcodeFilename << "\""<< endl;
	pStream = new ofstream(config.gcodeFilename.c_str());


}

void GCoderOperation::start()
{
	// this code doesn't belong here
	// init should not mean to start the work
	// should there be a start for the pipeline?

	ostream& ss = stream();
	initMachine(ss);
	initPlatform(ss);
	initExtruders(ss);

	gotoHomePosition(ss);
	waitForWarmup(ss);

}

ostream& GCoderOperation::stream()
{
	assert(pStream);
	return *(pStream);
}

DataEnvelope* GCoderOperation::processEnvelope(const DataEnvelope& envelope)
{
	ostream& ss = stream();
	if( this->isFirstEnvelope(envelope) )
	{
		initalized = true;
	}
	else if (isLastEnvelope(envelope) )
	{
		finishGcode(ss);
		closeFile();
	}
	return NULL;

}

AtomType GCoderOperation::collectsEnvelopeType() {
	return TYPE_PATH_ASCII;
}

AtomType GCoderOperation::emitsEnvelopeType() {
	return TYPE_GCODE_ASCII;
}


std::string GCoderOperation::interrogate() {
	return std::string("GCodeOperator");
}

void GCoderOperation::cleanup()
{
	closeFile();
}

void GCoderOperation::closeFile()
{
	if(pStream != NULL)
		pStream->close();
	pStream = NULL;
}


void GCoderOperation::initMachine(std::ostream &ss) const
{
	const Configuration &config = configuration();

	ss <<  "(Initialization of the machine)\n";
	ss <<  "(http://wiki.makerbot.com/gcode)";
	ss <<  "\n";
	ss <<  "G21 (set units to mm)\n";
	ss <<  "G90 (absolute positioning mode)\n";

	int toolHeadId = 0;
	for (std::vector<Extruder>::const_iterator i= config.extruders.begin(); i!=config.extruders.end(); i++)
	{
		Extruder e = *i;
		int coordinateSystemNb = toolHeadId +1;
		ss << "G10 P" << coordinateSystemNb << " X" <<  e.coordinateSystemOffsetX << " Y0 Z-0.3" << endl;
		toolHeadId ++;
	}
	ss << endl;

}

void GCoderOperation::initPlatform(std::ostream &ss) const
{
	const Configuration &config = configuration();

	double t = config.platform.temperature;
	ss << "M109 S" << t << " T1 (heat the build-platform to "  << t << " Celsius)" << endl;
	ss << endl;

}

void GCoderOperation::initExtruders(std::ostream &ss) const
{
	const Configuration &config = configuration();

	ss << "(setup extruder's')" << endl;
	int toolHeadId = 0;
	for (std::vector<Extruder>::const_iterator i= config.extruders.begin(); i!=config.extruders.end(); i++)
	{
		double t = 999;
		Extruder e = *i;

		ss << "M104 S" << e.extrusionTemperature  << " T" << toolHeadId << " (set temperature of extruder " << toolHeadId <<  " to "  << e.extrusionTemperature << " degrees Celsius)" << endl;
		ss << "M108 R" << e.defaultSpeed << " T" << toolHeadId << " (set extruder " <<  toolHeadId << " speed to the default " << e.defaultSpeed << "mm/s)" << endl;
		ss << "M103 T" << toolHeadId << " (Make sure extruder" << toolHeadId << " is off)" << endl;
		ss << endl;
		toolHeadId ++;
	}
	ss << endl;
}


void GCoderOperation::gotoHomePosition(std::ostream &ss) const
{
	ss << endl;
	ss << "(go to home position)" << endl;
	ss << "G162 Z F500 (home Z axis maximum)" << endl;
	ss << "G92 Z10 (set Z to 10)" << endl;
	ss << "G1 Z0.0 (move Z down 0)" << endl;
	ss << "G162 Z F100 (home Z axis maximum)" << endl;
	ss << "G161 X Y F2500 (home XY axes minimum)" << endl;
	ss << "M132 X Y Z A B (Recall stored home offsets for XYZAB axis)" << endl;
	ss << "G54 (first work coordinate system)" << endl;
	ss << endl;
}


void GCoderOperation::waitForWarmup(std::ostream &ss) const
{
	const Configuration &config = configuration();

	ss << endl;
	for (int i=0; i< config.extruders.size(); i++)
	{
		ss << "M6 T" << i << " (wait for " << i<<" to reach temperature)" << endl;
	}
	ss << "(heated build platform are tied to extruders for now)" << endl;

}

void GCoderOperation::finishGcode(std::ostream &ss) const
{
	const Configuration &config = configuration();

	for (int i=0; i< config.extruders.size(); i++)
	{
		ss << "M104 S0 T" << i << " (set extruder temperature)" << endl;
		ss << "M109 S0 T" << i << " (set heated-build-platform temperature)" << endl;
	}
	ss << "(That's all folks!)" << endl;
}




