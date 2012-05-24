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
#include "mgl.h"
#include "pather.h"


#define SAMESAME_TOL 1e-6
#define MUCH_LARGER_THAN_THE_BUILD_PLATFORM_MM 100000000

namespace mgl
{

class GcoderException : public Exception {	public: GcoderException(const char *msg) :Exception(msg){} };


/// Properties of a print platform
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

/// Properties of an extrusion profile
/// an extruder may have multiple extrusion profiles
/// EG: large, fast, 'first layer'
struct Extrusion
{
	Extrusion()
		:feedrate(2400),
 		 retractDistance(0),
		 retractRate(3000),
		 restartExtraDistance(0),
		 extrudedDimensionsRatio(0),
		 flow(2.8),
		 leadIn(0),
		 leadOut(0),
		 snortFlow(35),
                 snortFeedrate(600),
                 squirtFlow(35),
                 squirtFeedrate(600)
	{}

	Scalar crossSectionArea(Scalar height) const;

	double feedrate;

	double retractDistance;
	double retractRate;
	double restartExtraDistance;
	double extrudedDimensionsRatio;

	double flow; // RPM value for the extruder motor... not a real unit :-(

	double leadIn;
	double leadOut;

	double snortFlow;
	double snortFeedrate;
	double squirtFlow;
	double squirtFeedrate;
};


/// Properties common to a single hardware extruder
struct Extruder
{
	typedef enum {RPM_MODE, VOLUMETRIC_MODE} extrusionMode_t; 

	Extruder()
		:coordinateSystemOffsetX(0),
		extrusionTemperature(220),
		nozzleZ(0),
		zFeedRate(100),
		extrusionMode(VOLUMETRIC_MODE),
		feedDiameter(3),
		code('A'),
		id(0)
	{}

	Scalar feedCrossSectionArea() const;
	bool isVolumetric() const { return  extrusionMode == VOLUMETRIC_MODE; };


	double coordinateSystemOffsetX;  // the distance along X between the machine 0 position and the extruder tip
	double extrusionTemperature; 	 // the extrusion temperature in Celsius

	// this determines the gap between the nozzle tip
	// and the layer at position z (measured at the middle of the layer)
	double nozzleZ;
	double zFeedRate;
	extrusionMode_t extrusionMode;
	double feedDiameter;
	char code;
	int id;

	std::string firstLayerExtrusionProfile;
	std::string insetsExtrusionProfile;
	std::string infillsExtrusionProfile;
};


/// a gantry covers functions of the printer frame,
/// contains the state of x,y,z, feed rate and other state that
// change as the print happens.
struct Gantry
{

	double x,y,z,a,b,feed;     // current position and feed


public:
	double rapidMoveFeedRateXY;
	double rapidMoveFeedRateZ;
	double homingFeedRateZ;
	double layerH;

	bool xyMaxHoming;
	bool zMaxHoming;
	bool extruding;
	double scalingFactor;
	char ab;

	Gantry()
		:x(MUCH_LARGER_THAN_THE_BUILD_PLATFORM_MM),
		 y(MUCH_LARGER_THAN_THE_BUILD_PLATFORM_MM),
		 z(MUCH_LARGER_THAN_THE_BUILD_PLATFORM_MM),
		 a(0),
         b(0),
		 feed(0),
		 rapidMoveFeedRateXY(5000),
		 rapidMoveFeedRateZ(1400),
		 homingFeedRateZ(100),
		 layerH(.27),
		 xyMaxHoming(true),
		 zMaxHoming(false),
		 extruding(false),
		 scalingFactor(1),
		 ab('A')
	{

	}



public:

	/// writes g1 motion command to gcode output stream
	/// TODO: make this lower level function private.
	void g1Motion(std::ostream &ss,
				  double x, double y, double z,
				  double e,
				  double feed,
				  const char *comment,
				  bool doX, bool doY, bool doZ,
				  bool doE,
				  bool doFeed);

public:
	void squirt(std::ostream &ss, const libthing::Vector2 &lineStart,
				const Extruder &extruder, const Extrusion &extrusion);
	void snort(std::ostream &ss, const libthing::Vector2 &lineEnd,
			   const Extruder &extruder, const Extrusion &extrusion);

    void writeSwitchExtruder(std::ostream& ss, Extruder &extruder);

	/// public method emits a g1 command to the stream,
    /// only writing the parameters that have changed since the last g1.
	void g1(std::ostream &ss,
			const Extruder *extruder,
			const Extrusion *extrusion,
			double x,
			double y,
			double z,
			double feed,
			const char *comment);

	/// g1 public overloaded methods to make interface simpler
	void g1(std::ostream &ss,
			double x,
			double y,
			double z,
			double feed,
			const char *comment) {
		g1(ss, NULL, NULL, x, y, z, feed, comment);
	};

	/// g1 public overloaded methods to make interface simpler
	void g1(std::ostream &ss,
			const Extruder &extruder,
			const Extrusion &extrusion,
			double x,
			double y,
			double z,
			double feed,
			const char *comment) {
		g1(ss, &extruder, &extrusion, x, y, z, feed, comment);
	};

	Scalar volumetricE(const Extruder &extruder, const Extrusion &extrusion,
					   Scalar x, Scalar y, Scalar z) const;

	Scalar segmentVolume(const Extruder &extruder, const Extrusion &extrusion,
						 libthing::LineSegment2 &segment) const;

	/// get axis value of the current extruder in(mm)
	/// (aka mm of feedstock since the last reset this print)
	Scalar getCurrentE() const { if (ab == 'A') return a; else return b; };

	/// set axis value of the current extruder in(mm)
	/// (aka mm of feedstock since the last reset this print)
	void setCurrentE(Scalar e) { if (ab == 'A') a = e; else b = e; };
};


//// a line around the print used as a print 'skirt'
///
struct Outline
{
	Outline() :enabled(false), distance(3.0){}
	bool enabled;   // when true, a rectangular ouline of the part will be performed
	double distance; // the distance in mm  between the model and the rectangular outline
};




struct GCoderConfig
{
	GCoderConfig()
	:root(),
	 programName(GRUE_PROGRAM_NAME),
	 versionStr(GRUE_VERSION)

{}

    Json::Value root;

    std::string programName;
    std::string versionStr;
    std::string machineName;
    std::string firmware;

    Platform platform;
    Outline outline;
    Gantry gantry;

    std::map<std::string, Extrusion> extrusionProfiles;
    std::vector<Extruder> extruders;
};


//
// This class contains settings for the 3D printer,
// user preferences as well as runtime information
//
class GCoder : public Progressive
{
public:

	GCoderConfig gcoderCfg;

        GCoder(const GCoderConfig &gCoderCfg, ProgressBar* progress=NULL)
            :Progressive(progress), gcoderCfg(gCoderCfg)
	{	}


        /// shortcut for doing a G1 that only move Z
        void moveZ( std::ostream & ss, double z,
        		unsigned int  extruderId, double zFeedrate);

public:
        /// top level entry point for writing a gcode file
        /// @param slices: list of slices to write into a gcode
        /// @param layerMeasure:  tool to calc layer Z
        /// @param gout: stream to write gcode to
        /// @param title: name of the model to write?
        /// @param firstSliceIdx: starting slice index, -1 if you want the whole model
        /// @param lastSliceIdx: ending slice index, -1 if you want the whole model
        void writeGcodeFile(std::vector <SliceData>& slices,
                            const mgl::LayerMeasure& layerMeasure,
                            std::ostream &gout,
    						const char *title,
    						int firstSliceIdx=-1,
    						int lastSliceIdx=-1);

    ///  returns extrusionParams set based on the extruder id, and where you
    /// are in the model
    void calcInfillExtrusion(	unsigned int extruderId, unsigned int sliceId,
    								Extrusion &extrusionParams) const;

    ///  returns extrusionParams set based on the extruder id, and where you
    /// are in the model
    void calcInSetExtrusion (	unsigned int extruderId, unsigned int sliceId,
    								unsigned int insetId,	 unsigned int insetCount,
    								Extrusion &extrusionParams) const;

    /// Writes the start.gcode file, otherwise generates a
    /// start.gcode if needed
    void writeStartDotGCode(std::ostream & ss, const char* filename);

    /// Writes the end.gcode file, otherwise generates a
    /// end.gcode if needed
    void writeEndDotGCode(std::ostream & ss) const;


    // todo: return the gCoderCfg instead
    const std::vector<Extruder> & readExtruders() const
    {
        return gcoderCfg.extruders;
    }

    void writeSlice(std::ostream & ss, const mgl::SliceData & pathData);

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
						const Extruder &extruder,
						const Extrusion &extrusion,
						const Polygons &paths);

    void writePolygon(	std::ostream & ss,
						double z,
						const Extruder &extruder,
						const Extrusion &extrusion,
						const Polygon & polygon);

    // void writeWipeExtruder(std::ostream& ss, int extruderId) const {};
};


}
#endif

