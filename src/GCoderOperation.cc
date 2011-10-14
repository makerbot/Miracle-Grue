/*
 * GCoderOperation.cc
 *
 *  Created on: Oct 14, 2011
 *      Author: farmckon
 */
#include "DataEnvelope.h"
#include "GCoderOperation.h"


void GCoderOperation::init(Configuration& config)
{
	std::string outFilename("out.gcode");
	outstream = new ofstream(outFilename.c_str());
	ostream& ss = *(this->outstream);
	init_machine(ss);
	init_platform(ss);
	init_extruders(ss);
	initalized = true;
	return;
}


void GCoderOperation::collect(const  DataEnvelope& envelope)
{
	// --
	// finishedData = doDataCrap(envelope);
	//this->nextOperation.collect(finishedData);
	if(initalized == false || outstream  == 0x00)
	{
		cout << "cannot collect, not initalized" << endl;
		return ;
	}
	if(envelope.typeID != TYPE_ASCII_PATHER)
		cout << "data type failure for envelope X" << endl;

	if(envelope.lastFlag == true)
		closeFile(this->outstream);

	// always call emit data, even if just with dummy data!
	emitData(envelope);

}

AtomType GCoderOperation::collectsEnvelopeType() {
	return TYPE_INVALID;
}
AtomType GCoderOperation::emitsEnvelopeType() {
	return TYPE_ASCII_GCODE;
}


std::string GCoderOperation::interrogate() {
	return std::string("GCodeOperator");
}

void GCoderOperation::cleanup()
{
	closeFile(this->outstream);
}

void GCoderOperation::closeFile(ofstream *fs) const
{
	fs->close();
}


void GCoderOperation::write(const char*gstring, ostream &ss ) const
{
	//gStrings.push_back(gstring);
	string tmpStr(gstring);
	ss << tmpStr;
}


void GCoderOperation::init_machine(std::ostream &ss) const
{
	ss <<  "(Initialization of the machine)\n";
	ss <<  "(http://wiki.makerbot.com/gcode)";
	ss <<  "\n";
	ss <<  "G21 (set units to mm)\n";
	ss <<  "G90 (absolute positioning mode)\n";
/*
	int toolHeadId = 0;
	for (std::vector<Extruder>::const_iterator i= config.extruders.begin(); i!=config.extruders.end(); i++)
	{
		Extruder e = *i;
		int coordinateSystemNb = toolHeadId +1;
		ss << "G10 P" << coordinateSystemNb << " X%3f" <<  e.coordinateSystemOffsetX << " Y0 Z-0.3" << endl;
		toolHeadId ++;
	}
	*/
	ss << endl;

}

void GCoderOperation::init_platform(std::ostream &ss) const
{
	double t = 7; //this->config.platform.temperature;
	ss << "M109 S" << t << " T1 (heat the build-platform to "  << t << " Celsius)" << endl;
	ss << endl;

}

void GCoderOperation::init_extruders(std::ostream &ss) const
{
	int toolHeadId = 0;
	/*
	for (vector<Extruder>::const_iterator i= config.extruders.begin(); i!=config.extruders.end(); i++)
	{
		double t = 999;
		Extruder e = *i;

		ss << "M104 S" << e.extrusionTemperature  << " T" << toolHeadId << " (set temperature of extruder " << toolHeadId <<  " to "  << e.extrusionTemperature << " degrees Celsius)" << endl;
		ss << "M108 R" << e.defaultSpeed << " T" << toolHeadId << " (set extruder " <<  toolHeadId << " speed to the default " << e.defaultSpeed << "mm/s)" << endl;
		ss << "M103 T" << toolHeadId << " (Make sure extruder is off)" << endl;
		ss << endl;
		toolHeadId ++;
	}*/

}
