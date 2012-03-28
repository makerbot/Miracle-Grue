/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/


#ifndef GCODER_H_
#define GCODER_H_

#include <map>
#include "mgl/slicy.h"


#define SAMESAME_TOL 1e-6
#define MUCH_LARGER_THAN_THE_BUILD_PLATFORM 100000000

namespace mgl
{

class GcoderException : public Exception {	public: GcoderException(const char *msg) :Exception(msg){} };


struct Platform
{
	Platform():temperature(100),
				automated(false),
				waitingPositionX(50),
				waitingPositionY(-50),
				waitingPositionZ(0)
	{}
	double temperature;				// temperature of the platform during builds
	bool automated;

	// the wiper(s) are affixed to the platform
    double waitingPositionX;
    double waitingPositionY;
    double waitingPositionZ;
};

struct Extrusion
{
	Extrusion()
		:feedrate(2400),
		 flow(2.8),
		 leadIn(0),
		 leadOut(0),
		 snortFlow(35),
		 squirtFlow(35),
		 snortFeedrate(600),
		 squirtFeedrate(600)
	{}

	double feedrate;
	double flow; // RPM value for the extruder motor... not a real unit :-(

	double leadIn;
	double leadOut;

	double snortFlow;
	double snortFeedrate;
	double squirtFlow;
	double squirtFeedrate;
};

struct Extruder
{
	Extruder()
		:coordinateSystemOffsetX(0),
		extrusionTemperature(220),
		nozzleZ(0),
		zFeedRate(100)
	{}

	double coordinateSystemOffsetX;  // the distance along X between the machine 0 position and the extruder tip
	double extrusionTemperature; 	 // the extrusion temperature in Celsius

	// this determines the gap between the nozzle tip
	// and the layer at position z (measured at the middle of the layer)
	double nozzleZ;
	double zFeedRate;

	std::string firstLayerExtrusionProfile;
	std::string insetsExtrusionProfile;
	std::string infillsExtrusionProfile;
};


// a gantry has a purpose,
// or at least a position in space, feed rate and other state that
// change as the print happens.
struct Gantry
{

	//unsigned int nb;
	double x,y,z,feed;     // current position and feed
	std::string comment;   // if I'm not useful by xmas please delete me

public:
	double rapidMoveFeedRateXY;
	double rapidMoveFeedRateZ;
	double homingFeedRateZ;

	bool xyMaxHoming;
	bool zMaxHoming;
	double scalingFactor;


	Gantry()
		:x(MUCH_LARGER_THAN_THE_BUILD_PLATFORM),
		 y(MUCH_LARGER_THAN_THE_BUILD_PLATFORM),
		 z(MUCH_LARGER_THAN_THE_BUILD_PLATFORM),
		 rapidMoveFeedRateXY(5000),
		 rapidMoveFeedRateZ(1400),
		 xyMaxHoming(true),
		 zMaxHoming(false),
		 scalingFactor(1),
		 homingFeedRateZ(100)
	{

	}



public:
	// emits a g1 command
	void g1Motion(std::ostream &ss,
			double x,
			double y,
			double z,
			double feed,
			const char *comment,
			bool doX,
			bool doY,
			bool doZ,
			bool doFeed);

public:
	void squirt(std::ostream &ss, const Vector2 &lineStart, double reversalFeedrate, double reversalExtrusionSpeed,  double extrusionSpeed);
	void snort(std::ostream &ss, const Vector2 &lineEnd, double reversalFeedrate, double reversalExtrusionSpeed);

    void writeSwitchExtruder(std::ostream& ss, int extruderId);

	// emits a g1 command to the stream, only writing the parameters that have changed since the last g1.
	void g1(std::ostream &ss,
			double x,
			double y,
			double z,
			double feed,
			const char *comment);

};


// a line around the print
struct Outline
{
	Outline() :enabled(false), distance(3.0){}
	bool enabled;   // when true, a rectangular ouline of the part will be performed
	double distance; // the distance in mm  between the model and the rectangular outline
};



// directives for the Gcoder
struct GCoding
{
	GCoding()
		:outline(false),
		 insets(true),
		 infills(true),
		 infillFirst(false),
		 dualtrick(false)
		{}
	bool outline;
	bool insets;
	bool infills;
	bool infillFirst;
	bool dualtrick;
};



//
// This class contains settings for the 3D printer,
// user preferences as well as runtime information
//
class GCoder
{

public:
	GCoder()
	:programName("Miracle-Grue"),
	 versionStr("0.0.1")
	{
	}

    std::string programName;
    std::string versionStr;
    std::string machineName;
    std::string firmware;

    GCoding gcoding;
    Platform platform;
    Outline outline;
    Gantry gantry;

    std::map<std::string, Extrusion> extrusionProfiles;
    std::vector<Extruder> extruders;
    void moveZ( std::ostream & ss, double z, unsigned int  extruderId, double zFeedrate);

public:

    void calcInfillExtrusion(	unsigned int extruderId, unsigned int sliceId,
    								Extrusion &extrusionParams) const;
    void calcInSetExtrusion (	unsigned int extruderId, unsigned int sliceId,
    								unsigned int insetId,	 unsigned int insetCount,
    								Extrusion &extrusionParams) const;

    void writeStartOfFile(std::ostream & ss, const char* filename);
    void writeGcodeEndOfFile(std::ostream & ss) const;
    const std::vector<Extruder> & readExtruders() const
    {
        return extruders;
    }

    void writeSlice(std::ostream & ss, const mgl::SliceData & pathData, double layerZ, unsigned int sliceIndex);

private:

    void writeGCodeConfig(std::ostream & ss, const char* filename) const;
    void writeMachineInitialization(std::ostream & ss) const;
    void writePlatformInitialization(std::ostream & ss) const;
    void writeExtrudersInitialization(std::ostream & ss) const;
    void writeHomingSequence(std::ostream & ss);
    void writeWarmupSequence(std::ostream & ss);
    void writeAnchor(std::ostream & ss);
    void writePolygons(	std::ostream& ss,
						double z,
						const Extrusion &extrusionParams,
						const Polygons &paths);

    void writePolygon(	std::ostream & ss,
						double z,
						const Extrusion &extrusionParams,
						const Polygon & polygon);

    void writeWipeExtruder(std::ostream& ss, int extruderId) const;
};


}
#endif

