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

#include "mgl/configuration.h"
#include "mgl/meshy.h"

namespace mgl
{


struct Platform
{
	Platform():temperature(0),
				automated(false),
				waitingPositionX(0),
				waitingPositionY(0),
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
	double feed; // in mm per min
	double flow; // in RPMs! its kinda meaningless
};


struct Extruder
{
	Extruder()
		:coordinateSystemOffsetX(0),
		extrusionTemperature(220),
		defaultExtrusionSpeed(3),
		slowFeedRate(1080),
		slowExtrusionSpeed(1.0),
		fastFeedRate(3000),
		fastExtrusionSpeed(2.682),
		nozzleZ(0.28)
	{}

	double coordinateSystemOffsetX;  // the distance along X between the machine 0 position and the extruder tip
	double extrusionTemperature; 	 // the extrusion temperature in Celsius
	double defaultExtrusionSpeed;

	// first layer settings, for extra stickyness
	double slowFeedRate;
	double slowExtrusionSpeed;

	double reversalExtrusionSpeed;

	// different strokes, for different folks
	double fastFeedRate;
	double fastExtrusionSpeed;

	// this determines the gap between the nozzle tip
	// and the layer at position z (measured at the middle of the layer)
	double nozzleZ;

	// the distance to start before a line (and get the extruder going)
	double leadIn;
	// the reversal distance after a polygon.
	double leadOut;
};

#define SAMESAME_TOL 1e-6
#define MUCH_LARGER_THAN_THE_BUILD_PLATFORM 100000000

// a toolhead extends the extruder and gives it a purpose,
// or at least a position, feed rate and other state that
// change as the print happens.
struct ToolHead : Extruder
{
public:
	//unsigned int nb;
	double x,y,z,feed;     // position


	std::string comment;   // if I'm not useful by xmas please delete me


	ToolHead()
		:x(MUCH_LARGER_THAN_THE_BUILD_PLATFORM),
		 y(MUCH_LARGER_THAN_THE_BUILD_PLATFORM),
		 z(MUCH_LARGER_THAN_THE_BUILD_PLATFORM)
	{

	}



private:
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
	void squirt(std::ostream &ss, const Vector2 &lineStart, double extrusionSpeed);
	void snort(std::ostream &ss,  const Vector2 &lineEnd);


//	// emits a g1 command to the stream, all parameters are explicit
//	void moveToLongForm(std::ostream &ss,
//			double x,
//			double y,
//			double z,
//			double feed,
//			const char *comment);
	// emits a g1 command to the stream, only parameters that have changed since the last g1 are explicit
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
	float distance; // the distance in mm  between the model and the rectangular outline
};

struct Homing
{
	bool xyMaxHoming;
	bool zMaxHoming;
public:
	Homing():xyMaxHoming(true), zMaxHoming(false){}
};

// directives fo the Gcoder
struct GCoding
{
	bool outline;
	bool insets;
	bool infills;
	bool infillFirst;
};

class GcoderMess : public Messup {	public: GcoderMess(const char *msg) :Messup(msg){} };

//
// This class contains settings for the 3D printer,
// user preferences as well as runtime information
//
class GCoder
{
    std::string programName;
    std::string versionStr;
    std::string machineName;	// 3D printer identifier
    std::string firmware;		// firmware revision
    // double fastFeed;
    double scalingFactor;
    std::string gcodeFilename;			// output file name

    Homing homing;
    GCoding gcoding;
    Platform platform;
    Outline outline;					// outline operation configuration
    std::vector<ToolHead> extruders;	// list of extruder tools

public:
    void writeStartOfFile(std::ostream &ss);
    void writeGcodeEndOfFile(std::ostream &ss) const;


    const std::vector<ToolHead> &readExtruders() const
    {
    	return extruders;
    }

	void loadData(const Configuration& config);
	void writeGcodeConfig(std::ostream &ss, const std::string indent) const;

    void writeSlice(std::ostream &ss, const mgl::SliceData& pathData, unsigned int sliceIndex);

private:

	// write important config information in gcode file
    void writeGCodeConfig(std::ostream &ss) const;
	void writeMachineInitialization(std::ostream &ss) const;
    void writePlatformInitialization(std::ostream &ss) const;
    void writeExtrudersInitialization(std::ostream &ss) const;
    void writeHomingSequence(std::ostream &ss) const;

    void writeWarmupSequence(std::ostream &ss);
    void writeAnchor(std::ostream &ss);

    void writePaths(std::ostream& ss,
    		unsigned int sliceIndex,
    		unsigned int extruderId,
    		double z,
    		const std::vector<mgl::Polygon> &paths); // paths for an extruder in a layer

    void writeSwitchExtruder(std::ostream& ss, int extruderId) const;
    void writeWipeExtruder(std::ostream& ss, int extruderId) const;


};

void writeGcodeFile(const Configuration &config,
					const char* gcodeFilePath,
					const std::vector< mgl::SliceData> & allTubes);

}
#endif

