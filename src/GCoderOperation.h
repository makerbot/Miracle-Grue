/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/
#ifndef GCODE_OPERATION_H_
#define GCODE_OPERATION_H_

#include <iostream>
#include <fstream>

#include <string>
#include <assert.h>
#include <sstream>

#include "Operation.h"
#include "PathData.h"

#include "GCodeEnvelope.h"


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

	// the reversal distance to break the tube
	// in the xy plane.
	double reversalXY;
};


#define MUCH_LARGER_THAN_THE_BUILD_PLATFORM 10000000000000

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

	bool sameSame(double a, double b)const
	{
		return (a*a + b*b) < 0.00000001;
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
	void squirt(std::ostream &ss);
	void snort(std::ostream &ss);


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


// configuration
struct Outline
{
	Outline() :enabled(false), distance(3.0){}
	bool enabled;   // when true, a rectangular ouline of the part will be performed
	float distance; // the distance in mm  between the model and the rectangular outline
};



//
// This class contains settings for the 3D printer,
// user preferences as well as runtime information
//
struct GCoderData
{
    std::string programName;
    std::string versionStr;
    std::string machineName;	// 3D printer identifier
    std::string firmware;		// firmware revision
    // double fastFeed;
    double scalingFactor;
    std::string gcodeFilename;			// output file name

    Platform platform;
    Outline outline;					// outline operation configuration
    std::vector<ToolHead> extruders;	// list of extruder tools


	void loadData(const Configuration& config);
	void writeGcodeConfig(std::ostream &ss, const std::string indent) const;
         // ~GCoderConfig()
};

/**
 * GCoderOperation creates gcode from a stream of path envelopes.
 *
 */
class GCoderOperation : public Operation
{
	GCoderData config;

/************** Start of Functions each <NAME_OF>Operation must contain***********************/
public:

	///Standard Constructor
	GCoderOperation();


	///Standard Destructor
	~GCoderOperation();


	/**
	 * This function returns a global static pointer to a list of Configuration Requirements
	 * See details in implementation
	 * @return global static Json::Value pointer
	 */
	static Json::Value* getStaticConfigRequirements();

	/**
	 * This is the heart of data processing.  This is the core of the envelope accepting system.
	 * See details in implementation
	 * @param envelope reference to a DataEnvelope, or related subclass
	 */
	void processEnvelope(const DataEnvelope& envelope);

	/**
	 * This initalizes an operation with specific settings for processing this stream.
	 * See details in implementation
	 * @param config configuration for this stream
	 * @param outputs A vector of operations that receive output envelopes of data from this object
	 */
	void init(Configuration& config,const std::vector<Operation*> &outputs);

	///This un-initalizes an operation.
	void deinit();

	///This function must be called to start the processing chain
	void start();

	///This function must be called to wrap up the processing chain
	void finish();

/************** End of Functions each <NAME_OF>Operation must contain***********************/


/************** Start of Functions custom to this <NAME_OF>Operation ***********************/
private:
	bool isValidConfig(Configuration& config) const;
	void wrapAndEmit(const std::stringstream &ss);


	// write important config information in gcode file
    void writeGCodeConfig(std::ostream &ss) const;
	void writeMachineInitialization(std::ostream &ss) const;
    void writePlatformInitialization(std::ostream &ss) const;
    void writeExtrudersInitialization(std::ostream &ss) const;
    void writeHomingSequence(std::ostream &ss) const;
    void writeWarmupSequence(std::ostream &ss);
    void writeAnchor(std::ostream &ss);

    void writeLayer(std::ostream &ss, const PathData& pathData);
    void writePaths(std::ostream& ss, int extruderId, double z, const Paths &paths); // paths for an extruder in a layer
    void writeSwitchExtruder(std::ostream& ss, int extruderId) const;
    void writeWipeExtruder(std::ostream& ss, int extruderId) const;
    void writeGcodeEndOfFile(std::ostream &ss) const;



/************** End of Functions custom to this <NAME_OF>Operation ***********************/

};




#endif /* GCODE_OPERATION_H_ */

