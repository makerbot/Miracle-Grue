/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

**/
#include <assert.h>
#include <sstream>

#include "GCoderOperation.h"

using namespace std;



void moveTo(std::ostream &ss,  double x, double y, double z, double feed, const char*comment = NULL)
{
	std::string msg;
	if(comment)
	{
		msg = " (";
		msg += comment;
		msg += ")";
	}
	ss << "G1 X" << x << " Y" << y << " Z" << z << " F" << feed << msg  << endl;
}

void reversal(std::ostream &ss, double feedrate)
{
	ss << "M108 R" << feedrate << endl;
	ss << "M102 (reverse)" << endl;
}

GCoderOperation::GCoderOperation()
{
	std::cout << __FUNCTION__ << std::endl;
	std::cout << "(Miracle Grue)" << std::endl;
};


void GCoderOperation::start()
{
	cout << "GCoderOperation::start() !!" << endl;
	stringstream ss;

	writeGCodeConfig(ss);
	writeMachineInitialization(ss);
	writeExtrudersInitialization(ss);
	writePlatformInitialization(ss);

	writeHomingSequence(ss);
	writeWarmupSequence(ss);
	writeAnchor(ss);

	const char *msg = ss.str().c_str();
	emit(msg);
}

void GCoderOperation::finish()
{
	stringstream ss;
	writeGcodeEndOfFile(ss);

	const char *msg = ss.str().c_str();
	emit(msg);
	Operation::finish();
}


void GCoderOperation::writePaths(ostream& ss, const PathData& pathData) const
{
	double nozzleZ = 0.28; // distance above mid layer position of extruzion
	double pathFeedrate = 1080.0;
	cout << endl << "GCoderOperation::writePaths()" << endl;
	ss << "(PATHS for: " << pathData.paths.size() << " Extruder" << ")"<< endl;
	double z = pathData.positionZ + nozzleZ;
	int extruderId = 0;
	for(std::vector<Paths>::const_iterator extruderIt = pathData.paths.begin(); extruderIt != pathData.paths.end(); extruderIt++)
	{
		ss << "( extruder " << extruderId << ")" << endl;
		const Paths &paths = *extruderIt;
		for (Paths::const_iterator pathIt = paths.begin() ; pathIt != paths.end();  pathIt ++)
		{
			const Polygon &polygon = *pathIt;
			ss << "(  POLYGON " << polygon.size() << " Points)" << endl;
			for(Polygon::const_iterator i= polygon.begin(); i!= polygon.end(); i++ )
			{
				const Point2D &p = *i;
				ss << "(      POINT [" << p.x << ", " << p.y << "] )" << endl;
				moveTo(ss, p.x, p.y, z, pathFeedrate);
			}
		}
		ss << endl;
		extruderId ++;
	}
}

void GCoderOperation::processEnvelope(const DataEnvelope& envelope)
{

	// check that we are getting the right kind of data
	const PathData &pathData = *(dynamic_cast<const PathData* > (&envelope) );
	assert(&pathData != NULL);

	stringstream ss;
	writePaths(ss, pathData);
	emit(ss.str().c_str());

	// if this was the last envelope, then close the file
	if (isLastEnvelope(envelope) )
	{
		cout << "LAST PATH" << endl;
	}
}


void GCoderOperation::cleanup()
{

	//closeFile();
}



void GCoderOperation::writeGCodeConfig(std::ostream &ss) const
{
	const Configuration &config = configuration();

	ss << endl;
	ss << "(Makerbot Industries 2011)" << endl;
	ss << "(This file contains digital fabrication directives in gcode format)"<< endl;
	ss << "(What's gcode? http://wiki.makerbot.com/gcode)" <<  endl;
	ss << "(For your 3D printer)" << endl;
	config.writeGcodeConfig(ss, "* ");


	ss << endl;
}

void GCoderOperation::writeMachineInitialization(std::ostream &ss) const
{
	const Configuration &config = configuration();

	ss <<  "G21 (set units to mm)" << endl;
	ss <<  "G90 (absolute positioning mode)" << endl;

	int toolHeadId = 0;
	if (config.extruders.size() > 1)
	{
		for (std::vector<Extruder>::const_iterator i= config.extruders.begin(); i!=config.extruders.end(); i++)
		{
			Extruder e = *i;
			int coordinateSystemNb = toolHeadId +1;
			ss << "G10 P" << coordinateSystemNb << " X" <<  e.coordinateSystemOffsetX << " Y0 Z-0.3" << endl;
			toolHeadId ++;
		}
	}
	ss << endl;

}

void GCoderOperation::writeExtrudersInitialization(std::ostream &ss) const
{
	const Configuration &config = configuration();
	string plural = "";
	if(config.extruders.size()>1) plural = "s";
	ss << "(setup extruder" << plural <<")" <<endl;
	int toolHeadId = 0;
	for (std::vector<Extruder>::const_iterator i= config.extruders.begin(); i!=config.extruders.end(); i++)
	{
		double t = 999;
		Extruder e = *i;
		ss << "M103 T" << toolHeadId << " (Make sure motor for extruder " << toolHeadId << " is stopped)" << endl;
		ss << "M108 R" << e.defaultSpeed << " T" << toolHeadId << " (set extruder " <<  toolHeadId << " speed to the default " << e.defaultSpeed << " RPM)" << endl;
		ss << "M104 S" << e.extrusionTemperature  << " T" << toolHeadId << " (set temperature of extruder " << toolHeadId <<  " to "  << e.extrusionTemperature << " degrees Celsius)" << endl;
		ss << endl;
		toolHeadId ++;
	}
	ss << endl;
}


void GCoderOperation::writePlatformInitialization(std::ostream &ss) const
{
	const Configuration &config = configuration();

	double t = config.platform.temperature;
	ss << "M109 S" << t << " T0 (heat the build-platform to "  << t << " Celsius)" << endl;
	ss << endl;

}

void GCoderOperation::writeHomingSequence(std::ostream &ss) const
{
	const Configuration &config = configuration();

	ss << endl;
	ss << "(go to home position)" << endl;
	ss << "G162 Z F800 (home Z axis maximum)" << endl;
	ss << "G92 Z5 (set Z to 5)" << endl;
	ss << "G1 Z0.0 (move Z down 0)" << endl;
	ss << "G162 Z F100 (home Z axis maximum)" << endl;
	ss << "G161 X Y F2500 (home XY axes minimum)" << endl;
	ss << "M132 X Y Z A B (Recall stored home offsets for XYZAB axis)" << endl;
	if (config.extruders.size() > 1)
		ss << "G54 (first work coordinate system)" << endl;
	ss << endl;
}



void GCoderOperation::writeWarmupSequence(std::ostream &ss) const
{
	const Configuration &config = configuration();

	ss << endl;

	moveTo(ss, config.platform.waitingPositionX, config.platform.waitingPositionY, config.platform.waitingPositionZ, config.fastFeed, "go to waiting position" );
	for (int i=0; i< config.extruders.size(); i++)
	{
		ss << "M6 T" << i << " (wait for tool " << i<<" to reach temperature)" << endl;
	}
	ss << "(heated build platform temperature is tied to tool 0 for now)" << endl;
	ss << endl;
	ss << endl;
}

void GCoderOperation::writeGcodeEndOfFile(std::ostream &ss) const
{
	const Configuration &config = configuration();

	for (int i=0; i< config.extruders.size(); i++)
	{
		ss << "M104 S0 T" << i << " (set extruder temperature to 0)" << endl;
		ss << "M109 S0 T" << i << " (set heated-build-platform temperature to 0)" << endl;
	}

	ss << "G162 Z F500 (home Z axis maximum)" << endl;
	ss << "(That's all folks!)" << endl;
}


void GCoderOperation::writeAnchor(std::ostream &ss) const
{
	const Configuration &config = configuration();
	ss << "(Create Anchor)" << endl;
	// moveTo(ss, config.platform.waitingPositionX, config.platform.waitingPositionY, config.platform.waitingPositionZ, config.fastFeed, "nozzle down" );
	ss << "G1 Z0.6 F300    (Position Height)" << endl;
	ss << "M108 R4.0   (Set Extruder Speed)" << endl;
	ss << "M101        (Start Extruder)" << endl;
	ss << "G4 P1500" << endl;
	ss << endl;

}

void GCoderOperation::emit(const char* msg)
{
	/*
	cout << endl;
	cout << "********************************** GCODE DATA ***************************" << endl;
	cout << msg;
	cout << "*************************************** END *****************************" << endl;
	cout << endl;
	*/
	Operation::emit(new GCodeData(msg));
}


