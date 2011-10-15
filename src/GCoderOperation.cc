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


	pConfig = &config;
	std::string outFilename("out.gcode");
	outstream = new ofstream(outFilename.c_str());

	cout << "Writing GCODE to file: " << outFilename << endl;
}

void GCoderOperation::start()
{
	// this code doesn't belong here
	// init should not mean to start the work
	// should there be a start for the pipeline?

	ostream& ss = stream();

	init_machine(ss);
	init_platform(ss);
	init_extruders(ss);

	goto_home_position(ss);
	wait_for_warm_up(ss);

	initalized = true;
	return;
}


void GCoderOperation::collect(const  DataEnvelope& envelope)
{
	ostream& ss = stream();
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
	{
		finish_gcode(ss);
		closeFile();
	}
	// always call emit data, even if just with dummy data!
	emitData(envelope);

}

AtomType GCoderOperation::collectsEnvelopeType() {
	return TYPE_ASCII_PATHER;
}

AtomType GCoderOperation::emitsEnvelopeType() {
	return TYPE_ASCII_GCODE;
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
	if(outstream != NULL)
		outstream->close();
	outstream = NULL;
}


void GCoderOperation::init_machine(std::ostream &ss) const
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
		ss << "G10 P" << coordinateSystemNb << " X%3f" <<  e.coordinateSystemOffsetX << " Y0 Z-0.3" << endl;
		toolHeadId ++;
	}
	ss << endl;

}

void GCoderOperation::init_platform(std::ostream &ss) const
{
	const Configuration &config = configuration();

	double t = config.platform.temperature;
	ss << "M109 S" << t << " T1 (heat the build-platform to "  << t << " Celsius)" << endl;
	ss << endl;

}

void GCoderOperation::init_extruders(std::ostream &ss) const
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


void GCoderOperation::goto_home_position(std::ostream &ss) const
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


void GCoderOperation::wait_for_warm_up(std::ostream &ss) const
{
	const Configuration &config = configuration();

	ss << endl;
	for (int i=0; i< config.extruders.size(); i++)
	{
		ss << "M6 T" << i << " (wait for " << i<<" to reach temperature)" << endl;
	}
	ss << "(heated build platform are tied to extruders for now)" << endl;

}

void GCoderOperation::finish_gcode(std::ostream &ss) const
{
	const Configuration &config = configuration();

	for (int i=0; i< config.extruders.size(); i++)
	{
		ss << "M104 S0 T" << i << " (set extruder temperature)" << endl;
		ss << "M109 S0 T" << i << " (set heated-build-platform temperature)" << endl;
	}
	ss << "(That's all folks!)" << endl;
}





/*
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


}
*/
