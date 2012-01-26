/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/

#include "gcoder.h"

using namespace mgl;
using namespace std;


// function that adds an s to a noun if count is more than 1
std::string plural(const char*noun, int count, const char* ending = "s")
{
	string s(noun);
	if (count>1)
	{
		return s+ending;
	}
	return s;
}

void mgl::writeGcodeFile(const Configuration &config,
					const char* gcodeFilePath,
					const std::vector<SliceData> & slices)
{
	GCoder gcoder = GCoder();
	gcoder.loadData(config);
    std::ofstream gout(gcodeFilePath);
    gcoder.writeStartOfFile(gout);

    ProgressBar progress(slices.size());
    for(int i = 0;i < slices.size();i++){
        progress.tick();
        cout.flush();
        const SliceData &slice = slices[i];
        gcoder.writeSlice(gout, slice);
    }
    gcoder.writeGcodeEndOfFile(gout);
    gout.close();
}


void GCoder::writeSlice(ostream& ss, const SliceData& sliceData)
{

	GCoder &gcoder = *this;
	double layerZ = sliceData.z;
	unsigned int extruderCount = sliceData.extruderSlices.size();

	ss << "(Slice " << sliceData.sliceIndex << ", " << extruderCount << " " << plural("Extruder", extruderCount) << ")"<< endl;
	for(unsigned int extruderId = 0; extruderId < extruderCount; extruderId++)
	{
		ss << "(   Extruder " <<  extruderId << ")" << endl;
		const Polygons &loops = sliceData.extruderSlices[extruderId].loops;
		const Polygons &infills = sliceData.extruderSlices[extruderId].infills;

		double z = layerZ + gcoder.extruders[extruderId].nozzleZ;

		if (extruderCount > 0)
		{
			writeSwitchExtruder(ss, extruderId);
		}

		writePaths(ss, sliceData.sliceIndex, extruderId, z, loops);
		writePaths(ss, sliceData.sliceIndex, extruderId, z, infills);

		if (extruderCount > 0)
		{
			writeWipeExtruder(ss, extruderId);
		}
		extruderId ++;
	}
}

void GCoder::writeSwitchExtruder(ostream& ss, int extruderId) const
{
	ss << "( extruder " << extruderId << " )" << endl;
	ss << "( GSWITCH T" << extruderId << " )" << endl;
	ss << endl;
}

void GCoder::writeWipeExtruder(ostream& ss, int extruderId) const
{
	ss << "( GWIPE my extruder #" << extruderId << " )"<< endl;
	ss << endl;
}

void GCoder::writeGCodeConfig(std::ostream &ss) const
{

	ss << endl;
	ss << "(Makerbot Industries)" << endl;
	ss << "(This file contains digital fabrication directives in gcode format)"<< endl;
	ss << "(For your 3D printer)" << endl;
	ss << "(http://wiki.makerbot.com/gcode)" <<  endl;
	writeGcodeConfig(ss, "* ");
	ss << endl;
}

void GCoder::writeMachineInitialization(std::ostream &ss) const
{
	ss <<  "G21 (set units to mm)" << endl;
	ss <<  "G90 (absolute positioning mode)" << endl;

	const GCoder &gcoder = *this;
	int toolHeadId = 0;
	if (gcoder.extruders.size() > 1)
	{
		for (std::vector<ToolHead>::const_iterator i= gcoder.extruders.begin(); i!=gcoder.extruders.end(); i++)
		{
			ToolHead t = *i;
			int coordinateSystemNb = toolHeadId +1;
			ss << "G10 P" << coordinateSystemNb << " X" <<  t.coordinateSystemOffsetX << " Y0 Z-0.3" << endl;
			toolHeadId ++;
		}
	}
	ss << endl;
}


void GCoder::writeExtrudersInitialization(std::ostream &ss) const
{
	const GCoder &gcoder = *this;
	string plural = "";
	if(gcoder.extruders.size()>1) plural = "s";
	ss << "(setup extruder" << plural <<")" <<endl;
	int toolHeadId = 0;
	for (std::vector<ToolHead>::const_iterator i= gcoder.extruders.begin(); i!=gcoder.extruders.end(); i++)
	{
		ToolHead t = *i;
		ss << "M103 T" << toolHeadId << " (Make sure motor for extruder " << toolHeadId << " is stopped)" << endl;
		ss << "M108 R" << t.defaultExtrusionSpeed << " T" << toolHeadId << " (set extruder " <<  toolHeadId << " speed to the default " << t.defaultExtrusionSpeed << " RPM)" << endl;
		ss << "M104 S" << t.extrusionTemperature  << " T" << toolHeadId << " (set temperature of extruder " << toolHeadId <<  " to "  << t.extrusionTemperature << " degrees Celsius)" << endl;
		ss << endl;
		toolHeadId ++;
	}
	ss << endl;
}


void GCoder::writePlatformInitialization(std::ostream &ss) const
{

	double t = platform.temperature;
	ss << "M109 S" << t << " T0 (heat the build-platform to "  << t << " Celsius)" << endl;
	ss << endl;

}


void GCoder::writeHomingSequence(std::ostream &ss) const
{
	const GCoder &gcoder = *this;

	ss << endl;
	ss << "(go to home position)" << endl;

	if(gcoder.homing.zMaxHoming)
		ss << "G162 Z F800 (home Z axis maximum)" << endl;
	else
		ss << "G161 Z F800 (home Z axis minimum)" << endl;

	ss << "G92 Z5 (set Z to 5)" << endl;
	ss << "G1 Z0.0 (move Z down 0)" << endl;

	if(gcoder.homing.zMaxHoming)
		ss << "G162 Z F100 (home Z axis maximum)" << endl;
	else
		ss << "G161 Z F100 (home Z axis minimum)" << endl;

	if(gcoder.homing.xyMaxHoming)
		ss << "G162 X Y F2500 (home XY axes maximum)" << endl;
	else
		ss << "G161 X Y F2500 (home XY axes minimum)" << endl;

	ss << "M132 X Y Z A B (Recall stored home offsets for XYZAB axis)" << endl;
	if (gcoder.extruders.size() > 1)
		ss << "G54 (first work coordinate system)" << endl;
	ss << endl;
}

void GCoder::writeWarmupSequence(std::ostream &ss)
{
	GCoder &gcoder = *this;
	ss << endl;
	gcoder.extruders[0].g1(ss, 	gcoder.platform.waitingPositionX,
				gcoder.platform.waitingPositionY,
				gcoder.platform.waitingPositionZ,
				gcoder.extruders[0].fastFeedRate,
				"go to waiting position" );

	for (int i=0; i< gcoder.extruders.size(); i++)
	{
		ss << "M6 T" << i << " (wait for tool " << i<<" to reach temperature)" << endl;
	}
	ss << "(note: the heated build platform temperature is tied to tool 0 for now)" << endl;
	ss << endl;
	ss << endl;
}

void GCoder::writeStartOfFile(std::ostream &gout)
{
	writeGCodeConfig(gout);
	writeMachineInitialization(gout);
	writeExtrudersInitialization(gout);
	writePlatformInitialization(gout);
	writeHomingSequence(gout);
	writeWarmupSequence(gout);
	writeAnchor(gout);
}

void GCoder::writeGcodeEndOfFile(std::ostream &ss) const
{
	for (int i=0; i< extruders.size(); i++)
	{
		ss << "M104 S0 T" << i << " (set extruder temperature to 0)" << endl;
		ss << "M109 S0 T" << i << " (set heated-build-platform id tied an extrusion tool)" << endl;
	}

	if(homing.zMaxHoming)
		ss << "G162 Z F500 (home Z axis maximum)" << endl;
	ss << "(That's all folks!)" << endl;
}

void GCoder::writeAnchor(std::ostream &ss)
{
	GCoder &gcoder = *this;
	ss << "(Create Anchor)" << endl;
	ss << "G1 Z0.6 F300    (Position Height)" << endl;
	ss << "M108 R4.0   (Set Extruder Speed)" << endl;
	ss << "M101        (Start Extruder)" << endl;
	ss << "G4 P1600" << endl;

	gcoder.extruders[0].g1( ss,
							gcoder.platform.waitingPositionX,
							gcoder.platform.waitingPositionY,
							0.6,
							gcoder.extruders[0].slowFeedRate,
							NULL );

	double dx = gcoder.platform.waitingPositionX - 3.0;
	double dy = gcoder.platform.waitingPositionY - 0.0;

	gcoder.extruders[0].g1(ss, dx, dy, 0.6, 0.2 * gcoder.extruders[0].slowFeedRate , NULL);
	ss << endl;
}


