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

#define EZLOGGER_OUTPUT_FILENAME "ezlogger.txt"
#include "ezlogger/ezlogger_headers.hpp"


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

//
// computes 2 positions (one before and one at the end of) the polygon and stores them in start and stop.
// These positions are aligned with the fisrt line and last line of the polygon.
// LeadIn is the distance between start and the first point of the polygon (along the first polygon line).
// LeadOut is the distance between the last point of the Polygon and stop (along the last polygon line).
void polygonLeadInAndLeadOut(const Polygon &polygon, double leadIn, double leadOut,
								Vector2 &start, Vector2 &end)
{
	size_t count =  polygon.size();

	const Vector2 &a = polygon[0];	// first element
	const Vector2 &b = polygon[1];

	const Vector2 &c = polygon[count-2];
	const Vector2 &d = polygon[count-1]; // last element

	Vector2 ab = b - a;
	ab.normalise();
	Vector2 cd = d-c;
	cd.normalise();

	start.x = a.x - ab.x * leadIn;
	start.y = a.y - ab.y * leadIn;
	end.x   = d.x + cd.x * leadOut;
	end.y   = d.y + cd.y * leadOut;

}


void Gantry::writeSwitchExtruder(ostream& ss, int extruderId)
{
	ss << "( extruder " << extruderId << " )" << endl;
	ss << "( GSWITCH T" << extruderId << " )" << endl;
	ss << "( TODO: add offset management to Gantry )" << endl;
	ss << endl;
}

void GCoder::writeWipeExtruder(ostream& ss, int extruderId) const
{
	//ss << "( GWIPE my extruder #" << extruderId << " )"<< endl;
	//ss << endl;
}



void GCoder::writeMachineInitialization(std::ostream &ss) const
{
	ss <<  "G21 (set units to mm)" << endl;
	ss <<  "G90 (absolute positioning mode)" << endl;

	const GCoder &gcoder = *this;
	int toolHeadId = 0;
	if (gcoder.extruders.size() > 1)
	{
		for (std::vector<Extruder>::const_iterator i= gcoder.extruders.begin(); i!=gcoder.extruders.end(); i++)
		{
			const Extruder &t = *i;
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
	for (std::vector<Extruder>::const_iterator i= gcoder.extruders.begin(); i!=gcoder.extruders.end(); i++)
	{
		const Extruder &t = *i;
		ss << "M103 T" << toolHeadId << " (Make sure motor for extruder " << toolHeadId << " is stopped)" << endl;
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


void GCoder::writeHomingSequence(std::ostream &ss)
{
	const GCoder &gcoder = *this;

	ss << endl;
	ss << "(go to home position)" << endl;

	if(gcoder.gantry.xyMaxHoming)
		ss << "G162 X Y F" << gantry.rapidMoveFeedRateXY<< " (home XY axes maximum)" << endl;
	else
		ss << "G161 X Y F" << gantry.rapidMoveFeedRateXY<< " (home XY axes minimum)" << endl;

	if(gcoder.gantry.zMaxHoming)
		ss << "G162 Z F" << gantry.rapidMoveFeedRateZ<< " (home Z axis maximum)" << endl;
	else
		ss << "G161 Z F" << gantry.rapidMoveFeedRateZ<< " (home Z axis minimum)" << endl;

	ss << "G92 Z5 (set Z to 5)" << endl;
	ss << "G1 Z0.0 (move Z down 0)" << endl;

	if(gcoder.gantry.zMaxHoming)
		ss << "G162 Z F" << gantry.homingFeedRateZ<< " (home Z axis maximum)" << endl;
	else
		ss << "G161 Z F" << gantry.homingFeedRateZ<< " (home Z axis minimum)" << endl;

	ss << "M132 X Y Z A B (Recall stored home offsets for XYZAB axis)" << endl;

	if (gcoder.extruders.size() > 1)
		ss << "G54 (first work coordinate system)" << endl;
	ss << endl;

	int extruderCount = extruders.size();
	if (extruderCount >0)
	{
		gantry.g1(ss, 	platform.waitingPositionX,
						platform.waitingPositionY,
						platform.waitingPositionZ,
						gantry.rapidMoveFeedRateXY,
						"go to waiting position" );

	}
	else
	{
		stringstream ss;
		ss << "There are no extruders configured. Has the config file been read?";
		GcoderException mixup(ss.str().c_str());
		throw mixup;
	}
}

void GCoder::writeWarmupSequence(std::ostream &ss)
{

	ss << endl;
	size_t extruderCount = extruders.size();
	for (size_t i=0; i< extruderCount; i++)
	{
		ss << "M6 T" << i << " (wait for tool " << i<<" to reach temperature)" << endl;
	}

	ss << "(note: the heated build platform temperature is tied to tool 0 for now)" << endl;
	ss << endl;
	ss << endl;
}

/**
 * Writes intial gcode data to start of the gcode file, including setup & startup info
 * @param gout - output stream for the gcode text
 * @param sourceName - source of this gcode (usually the origional stl file)
 */
void GCoder::writeStartOfFile(std::ostream &gout, const char* sourceName)
{
	gout.precision(3);
	gout.setf(ios::fixed);

	writeGCodeConfig(gout, sourceName);
	writeMachineInitialization(gout);
	writeExtrudersInitialization(gout);
	writePlatformInitialization(gout);
	writeHomingSequence(gout);
	writeWarmupSequence(gout);
	writeAnchor(gout);
}

void GCoder::writeGcodeEndOfFile(std::ostream &ss) const
{
	for (size_t i=0; i< extruders.size(); i++)
	{
		ss << "M104 S0 T" << i << " (set extruder temperature to 0)" << endl;
		ss << "M109 S0 T" << i << " (set heated-build-platform id tied an extrusion tool)" << endl;
	}

	if(gantry.zMaxHoming)
		ss << "G162 Z F500 (home Z axis maximum)" << endl;
	ss << "(That's all folks!)" << endl;
}

void GCoder::writeAnchor(std::ostream &ss)
{
	double anchorFeedRate = 3000;
	double z = 0.6;

	GCoder &gcoder = *this;
	ss << "(Create Anchor)" << endl;
	ss << "G1 Z0.6 F300    (Position Height)" << endl;
	ss << "M108 R4.0   (Set Extruder Speed)" << endl;
	ss << "M101        (Start Extruder)" << endl;
	ss << "G4 P1600" << endl;

	gantry.g1(  ss,
				gcoder.platform.waitingPositionX,
				gcoder.platform.waitingPositionY,
				z,
				gantry.rapidMoveFeedRateXY,
				NULL );

	double dx = gcoder.platform.waitingPositionX - 3.0;
	double dy = gcoder.platform.waitingPositionY - 0.0;

	gantry.g1(ss, dx, dy, z, 0.2 * anchorFeedRate , NULL);
	ss << "M102 (Stop extruder)" << endl;
	ss << endl;
}


void GCoder::writePolygon(	std::ostream & ss,
							double z,
							const Extrusion &extrusion,
							const Polygon & polygon)
{
    Vector2 start(0, 0), stop(0, 0);

    polygonLeadInAndLeadOut(polygon, extrusion.leadIn, extrusion.leadOut, start, stop);

    // rapid move into position
    gantry.g1(ss, start.x, start.y, z, gantry.rapidMoveFeedRateXY, NULL);

    // start extruding ahead of time while moving towards the first point
    gantry.squirt(ss, polygon[0], extrusion.squirtFeedrate, extrusion.squirtFlow, extrusion.flow);

   // for all other points in the polygon
    for(size_t i=1; i < polygon.size(); i++)
	{
    	// move towards the point
		const Vector2 &p = polygon[i];
		gantry.g1(ss, p.x, p.y, z, extrusion.feedrate, NULL);
	}
    //ss << "(STOP!)" << endl;
    gantry.snort(ss, stop, extrusion.snortFeedrate, extrusion.snortFlow);
    //ss << "(!STOP)" << endl;
    ss << endl;

}


void GCoder::writePolygons(std::ostream& ss,
		double z,
		const Extrusion &extrusion,
		const Polygons &paths)
{
	unsigned int pathCount = paths.size();
	for (unsigned int i = 0 ; i < pathCount;  i ++)
	{

		const Polygon &polygon = paths[i];
		ss << "(  path " << i << "/" << pathCount << ", " << polygon.size() << " points, "  << " )" << endl;

		unsigned int pointCount = polygon.size();
		if(pointCount < 2)
		{
			stringstream ss;
			ss << "Can't generate gcode for polygon " << i <<" with " << pointCount << " points.";
			GcoderException mixup(ss.str().c_str());
			throw mixup;
		}

		writePolygon(ss, z, extrusion, polygon);
	}
}

void GCoder::moveZ(ostream & ss, double z, unsigned int  extruderId, double zFeedrate)
{
    bool doX = false;
    bool doY = false;
    bool doZ = true;
    bool doFeed = true;
    const char *comment = NULL;

    gantry.g1Motion(ss, 0, 0, z, zFeedrate, "move Z", doX, doY, doZ, doFeed);

}


void GCoder::calcInfillExtrusion(unsigned int extruderId, unsigned int sliceId, Extrusion &extrusion) const
{
	string profileName;
	if(sliceId == 0)
	{
		profileName = extruders[extruderId].firstLayerExtrusionProfile;
	}
	else
	{
		profileName = extruders[extruderId].infillsExtrusionProfile;
	}

	const std::map<std::string, Extrusion>::const_iterator &it = extrusionProfiles.find(profileName);
	extrusion = it->second;
	extrusion.feedrate *= gantry.scalingFactor;
	extrusion.flow *= gantry.scalingFactor;
}

void GCoder::calcInSetExtrusion (	unsigned int extruderId,
										unsigned int sliceId,
										unsigned int insetId,
										unsigned int insetCount,
										Extrusion &extrusion) const
{
	string profileName;
	if(sliceId == 0)
	{
		profileName = extruders[extruderId].firstLayerExtrusionProfile;
	}
	else
	{
		profileName = extruders[extruderId].insetsExtrusionProfile;
	}

	const std::map<std::string, Extrusion>::const_iterator &it = extrusionProfiles.find(profileName);
	extrusion = it->second;
	extrusion.feedrate *= gantry.scalingFactor;
	extrusion.flow *= gantry.scalingFactor;
}


void GCoder::writeSlice(ostream& ss, const SliceData& sliceData )
{

	double layerZ = sliceData.getZHeight();
	unsigned int sliceIndex = sliceData.getIndex();
	unsigned int extruderCount = sliceData.extruderSlices.size();

	ss << "(Slice " << sliceIndex << ", " << extruderCount << " " << plural("Extruder", extruderCount) << ")"<< endl;
	// to each extruder its speed
	double zFeedrate = gantry.scalingFactor * extruders[0].zFeedRate;
	// moving all up. This is the first move for every new layer

	for(unsigned int extruderId = 0; extruderId < extruderCount; extruderId++)
	{
	    double z = layerZ + extruders[extruderId].nozzleZ;

		try
		{
		    moveZ(ss, z, extruderId, zFeedrate);
		}
		catch(GcoderException &mixup)
		{
			EZLOGGERVLSTREAM(axter::log_often) << "ERROR writing Z move in slice " << sliceIndex  << " for extruder " << extruderId << " : " << mixup.error << endl;
		}

		unsigned int dualtrickId =  extruderId;

	  	ss << "(   Extruder " <<  extruderId << ")" << endl;
		const Polygons &loops = sliceData.extruderSlices[extruderId].boundary;
		const Polygons &infills = sliceData.extruderSlices[extruderId].infills;
		const vector<Polygons> &insets = sliceData.extruderSlices[extruderId].insetLoopsList;

		//EZLOGGERVLSTREAM(axter::log_often) << endl <<  "Slice " << sliceData.sliceIndex << endl;

		try
		{
			if (extruderCount > 0)
			{
				gantry.writeSwitchExtruder(ss, extruderId);
			}
			if(gcoding.infills && gcoding.infillFirst)
			{
				Extrusion extrusion;
				calcInfillExtrusion(extruderId, sliceIndex, extrusion);
				ss << "(infills: "  << infills.size() << ")"<< endl;
				writePolygons(ss, z, extrusion, infills);
			}
		}
		catch(GcoderException &mixup)
		{
			EZLOGGERVLSTREAM(axter::log_often) << "ERROR writing infills in slice " << sliceIndex  << " for extruder " << extruderId << " : " << mixup.error << endl;
			cerr << "ERROR writing infills in slice " << sliceIndex  << " for extruder " << extruderId << " : " << mixup.error << endl;
		}
		try
		{
			if(gcoding.outline)
			{
				Extrusion extrusion;
				calcInfillExtrusion(extruderId, sliceIndex, extrusion);
				//EZLOGGERVLSTREAM(axter::log_often)  << "   Write OUTLINE" << endl;
				ss << "(outlines: " << loops.size() << " )"<< endl;
				writePolygons(ss, z, extrusion, loops);
			}
		}
		catch(GcoderException &mixup)
		{
			EZLOGGERVLSTREAM(axter::log_often)  << "ERROR writing loops in slice " << sliceIndex  << " for extruder " << extruderId << " : " << mixup.error << endl;
			cerr << "ERROR writing loops in slice " << sliceIndex  << " for extruder " << extruderId << " : " << mixup.error << endl;
		}

		try
		{
			if(gcoding.insets)
			{
				// each iteration is for a shell
				unsigned int insetCount = insets.size();
				for(unsigned int i=0; i < insetCount; i++)
				{
					Extrusion extrusion;
					calcInSetExtrusion(extruderId, sliceIndex, i, insetCount, extrusion);
					const Polygons &inset = insets[i];
					// EZLOGGERVLSTREAM(axter::log_often) << "   Write INSETS " << i << endl;
					ss << "(inset " << i << "/"<<  insetCount<< " )"<< endl;
					writePolygons(ss, z, extrusion, inset);

				}
			}
		}
		catch(GcoderException &mixup)
		{
			EZLOGGERVLSTREAM(axter::log_often) << "ERROR writing infills in slice " << sliceIndex  << " for extruder " << extruderId << " : " << mixup.error << endl;
			cerr << "ERROR writing infills in slice " << sliceIndex  << " for extruder " << extruderId << " : " << mixup.error << endl;
		}

		try
		{
			if(gcoding.infills && !gcoding.infillFirst)
			{
				//EZLOGGERVLSTREAM(axter::log_often) << "   Write INFILLS" << endl;
				Extrusion extrusion;
				calcInfillExtrusion(extruderId, sliceIndex, extrusion);
				writePolygons(ss, z, extrusion, infills);
			}
		}
		catch(GcoderException &mixup)
		{
			EZLOGGERVLSTREAM(axter::log_often) << "ERROR writing infills in slice " << sliceIndex  << " for extruder " << extruderId << " : " << mixup.error << endl;
			cerr << "ERROR writing infills in slice " << sliceIndex  << " for extruder " << extruderId << " : " << mixup.error << endl;
		}

		if (extruderCount > 0)
		{
			writeWipeExtruder(ss, extruderId);
		}
		extruderId ++;
	}
}



void Gantry::g1(std::ostream &ss, double x, double y, double z, double feed, const char *comment = NULL)
{

	bool doX = true;
	bool doY = true;
	bool doZ = true;
	bool doFeed = true;

	if(!mgl::tequals(this->x, x, SAMESAME_TOL))
	{
		doX = true;
	}
	if(!mgl::tequals(this->y, y, SAMESAME_TOL))
	{
		doY=true;
	}
	if(!mgl::tequals(this->z, z, SAMESAME_TOL))
	{
		doZ=true;
	}

	if(!mgl::tequals(this->feed, feed, SAMESAME_TOL))
	{
		doFeed=true;
	}

	g1Motion(ss, x,y,z,feed,comment,doX,doY,doZ,doFeed);
}

void Gantry::squirt(std::ostream &ss, const Vector2 &lineStart,
						double reversalFeedrate,
						double reversalFlow,
						double extrusionFlow)
{

	ss << "M108 R" <<  reversalFlow << " (squirt)" << endl;
	ss << "M101" << endl;
	g1(ss, lineStart.x, lineStart.y, z, reversalFeedrate, NULL);
	ss << "M108 R" << extrusionFlow << " (good to go)" << endl;
}

void Gantry::snort(std::ostream &ss, const Vector2 &lineEnd, double reversalFeedrate, double reversalExtrusionSpeed)
{
	ss << "M108 R" << reversalExtrusionSpeed << "  (snort)" << endl;
	ss << "M102" << endl;
	g1(ss, lineEnd.x, lineEnd.y, z, reversalFeedrate, NULL);
	ss << "M103" << endl;
}




void Gantry::g1Motion(std::ostream &ss, double x, double y, double z,
								double feed, const char *g1Comment,
								bool doX, bool doY, bool doZ, bool doFeed)
{

	// not do something is not an option .. under certain conditions
	#ifdef STRONG_CHECKING
	if( !(doX || doY || doZ || doFeed)   )
	{
		stringstream ss;
		ss << "G1 without moving where x=" << x << ", y=" << y << ", z=" << z << ", feed=" << feed ;
		GcoderException mixup(ss.str().c_str());
		throw mixup;
	}
	#endif

	// our moto: don't be bad!
	bool bad = false;
	if(fabs(x) > MUCH_LARGER_THAN_THE_BUILD_PLATFORM) bad = true;
	if(fabs(y) > MUCH_LARGER_THAN_THE_BUILD_PLATFORM) bad = true;
	if(fabs(z) > MUCH_LARGER_THAN_THE_BUILD_PLATFORM) bad = true;
	if(feed <= 0 || feed > 100000) bad = true;

	if(bad)
	{
		stringstream ss;
		ss << "Illegal G1 move where x=" << x << ", y=" << y << ", z=" << z << ", feed=" << feed ;
		GcoderException mixup(ss.str().c_str());
		throw mixup;
	}


	ss << "G1";
	if(doX) ss << " X" << x;
	if(doY) ss << " Y" << y;
	if(doZ) ss << " Z" << z;
	if(doFeed) ss << " F" << feed;
	if(g1Comment) ss << " (" << g1Comment << ")";

	ss << endl;

	// update state machine
	this->x = x;
	this->y = y;
	this->z = z;
	this->feed = feed;

	if(g1Comment == NULL)
	{
		string msg = "";
		this->comment = msg;
	}
	else
		this->comment = g1Comment;

}

/**
 * Writes config header metadata into a gcode file
 * @param ss Stream to write config data to
 * @param sourceName - Name of source of this model. Usually the original .stl filename
 */
void GCoder::writeGCodeConfig(std::ostream &ss, const char* sourceName="unknown source") const
{
	std::string indent = "* ";
	ss << endl;
	ss << "(Makerbot Industries)" << endl;
	ss << "(This file contains digital fabrication directives in gcode format)"<< endl;
	ss << "(For your 3D printer)" << endl;
	ss << "(http://wiki.makerbot.com/gcode)" <<  endl;

	MyComputer hal9000;

	ss << "(" << indent << "Generated by "<<  programName << " " << getMiracleGrueVersionStr() << ")"<< endl;
	ss << "(" << indent << hal9000.clock.now() <<  ")" << endl;
	ss << "(" << indent << "machine name: " << machineName << ")"<< endl;
	ss << "(" << indent << "firmware revision:" << firmware << ")" << endl;
	ss << "(" << indent << "3D model filename: " << sourceName << ")" << endl;

	std::string plurial = extruders.size()? "":"s";
	ss << "(" << indent << extruders.size() << " extruder" << plurial << ")" << endl;

	ss << "(" << indent << "Extrude infills: " << gcoding.infills <<  ")" << endl;
	ss << "(" << indent;
	if(gcoding.infillFirst)
		ss << "first operation: Infill";
	else
		ss << "first operation: Insets";
	ss << ")" << endl;
	ss << "(" << indent << "Extrude insets: " << gcoding.insets << ")" << endl;
	ss << "(" << indent << "Extrude outlines: " << gcoding.outline << ")" << endl;
	ss << endl;
}





