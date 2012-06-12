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


static const Scalar SAMESAME_TOL = 1e-6;
static const Scalar MUCH_LARGER_THAN_THE_BUILD_PLATFORM_MM = 100000000;

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
	Scalar temperature;				// temperature of the platform during builds
	bool automated;

	// the wiper(s) are affixed to the platform
    Scalar waitingPositionX;
    Scalar waitingPositionY;
    Scalar waitingPositionZ;
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

	Scalar feedrate;

	Scalar retractDistance;
	Scalar retractRate;
	Scalar restartExtraDistance;
	Scalar extrudedDimensionsRatio;

	Scalar flow; // RPM value for the extruder motor... not a real unit :-(

	Scalar leadIn;
	Scalar leadOut;

	Scalar snortFlow;
	Scalar snortFeedrate;
	Scalar squirtFlow;
	Scalar squirtFeedrate;
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


	Scalar coordinateSystemOffsetX;  // the distance along X between the machine 0 position and the extruder tip
	Scalar extrusionTemperature; 	 // the extrusion temperature in Celsius

	// this determines the gap between the nozzle tip
	// and the layer at position z (measured at the middle of the layer)
	Scalar nozzleZ;
	Scalar zFeedRate;
	extrusionMode_t extrusionMode;
	Scalar feedDiameter;
	unsigned char code;
	int id;

	std::string firstLayerExtrusionProfile;
	std::string insetsExtrusionProfile;
	std::string infillsExtrusionProfile;
};


/// a gantry covers functions of the printer frame,
/// contains the state of x,y,z, feed rate and other state that
// change as the print happens.
class Gantry
{
public:
	
	Gantry();
	
	Scalar get_x() const;
	Scalar get_y() const;
	Scalar get_z() const;
	Scalar get_a() const;
	Scalar get_b() const;
	Scalar get_feed() const;
	bool get_extruding() const;
	unsigned char get_current_extruder_index() const;
	
	void set_x(Scalar nx);
	void set_y(Scalar ny);
	void set_z(Scalar nz);
	void set_a(Scalar na);
	void set_b(Scalar nb);
	void set_feed(Scalar nfeed);
	void set_extruding(bool nextruding);
	void set_current_extruder_index(unsigned char nab);
	
	Scalar get_start_x() const;
	Scalar get_start_y() const;
	Scalar get_start_z() const;
	Scalar get_start_a() const;
	Scalar get_start_b() const;
	Scalar get_start_feed() const;
	
	void set_start_x(Scalar nx);
	void set_start_y(Scalar ny);
	void set_start_z(Scalar nz);
	void set_start_a(Scalar na);
	void set_start_b(Scalar nb);
	void set_start_feed(Scalar nfeed);
	
	/// reinitialize current xyzabf to start xyzabf
	void init_to_start();
	
	Scalar get_rapid_move_feed_rate_xy() const;
	Scalar get_rapid_move_feed_rate_z() const;
	Scalar get_homing_feed_rate_z() const;
	bool get_xy_max_homing() const;
	bool get_z_max_homing() const;
	Scalar get_layer_h() const;
	
	void set_rapid_move_feed_rate_xy(Scalar nxyr);
	void set_rapid_move_feed_rate_z(Scalar nzr);
	void set_homing_feed_rate_z(Scalar nhfrz);
	void set_xy_max_homing(bool mh);
	void set_z_max_homing(bool mh);
	void set_layer_h(Scalar lh);

public:
	Scalar rapidMoveFeedRateXY;
	Scalar rapidMoveFeedRateZ;
	Scalar homingFeedRateZ;
	Scalar layerH;

	bool xyMaxHoming;
	bool zMaxHoming;
	Scalar scalingFactor;

private:
	Scalar x,y,z,a,b,feed;     // current position and feed
	unsigned char ab;
	bool extruding;

private:
	Scalar sx, sy, sz, sa, sb, sfeed;


public:

	/// writes g1 motion command to gcode output stream
	/// TODO: make this lower level function private.
	void g1Motion(std::ostream &ss,
				  Scalar mx, Scalar my, Scalar mz,
				  Scalar e,
				  Scalar mfeed,
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
			Scalar gx,
			Scalar gy,
			Scalar gz,
			Scalar gfeed,
			const char *comment);

	/// g1 public overloaded methods to make interface simpler
	void g1(std::ostream &ss,
			Scalar gx,
			Scalar gy,
			Scalar gz,
			Scalar gfeed,
			const char *comment) {
		g1(ss, NULL, NULL, gx, gy, gz, gfeed, comment);
	};

	/// g1 public overloaded methods to make interface simpler
	void g1(std::ostream &ss,
			const Extruder &extruder,
			const Extrusion &extrusion,
			Scalar gx,
			Scalar gy,
			Scalar gz,
			Scalar gfeed,
			const char *comment) {
		g1(ss, &extruder, &extrusion, gx, gy, gz, gfeed, comment);
	};

	Scalar volumetricE(const Extruder &extruder, const Extrusion &extrusion,
					   Scalar vx, Scalar vy, Scalar vz) const;

	Scalar segmentVolume(const Extruder &extruder, const Extrusion &extrusion,
						 libthing::LineSegment2 &segment) const;

	/// get axis value of the current extruder in(mm)
	/// (aka mm of feedstock since the last reset this print)
	Scalar getCurrentE() const;// { if (ab == 'A') return a; else return b; };

	/// set axis value of the current extruder in(mm)
	/// (aka mm of feedstock since the last reset this print)
	void setCurrentE(Scalar e);// { if (ab == 'A') a = e; else b = e; };
};


//// a line around the print used as a print 'skirt'
///
struct Outline
{
	Outline() :enabled(false), distance(3.0){}
	bool enabled;   // when true, a rectangular ouline of the part will be performed
	Scalar distance; // the distance in mm  between the model and the rectangular outline
};




struct GCoderConfig
{
	GCoderConfig():
	 programName(GRUE_PROGRAM_NAME),
	 versionStr(GRUE_VERSION)
	{}



    std::string programName;
    std::string versionStr;
    std::string machineName;
    std::string firmware;

    Platform platform;
    Outline outline;
    Gantry gantry;

    std::map<std::string, Extrusion> extrusionProfiles;
    std::vector<Extruder> extruders;


    bool doOutlines;
    bool doInsets;
    bool doInfills;
    bool doInfillsFirst;

	Scalar startX;
	Scalar startY;
    std::string header;
    std::string footer;

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
{}


        /// shortcut for doing a G1 that only move Z
        void moveZ( std::ostream & ss, Scalar z,
        		unsigned int  extruderId, Scalar zFeedrate);

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
						Scalar z,
						const Extruder &extruder,
						const Extrusion &extrusion,
						const Polygons &paths);

    void writePolygon(	std::ostream & ss,
						Scalar z,
						const Extruder &extruder,
						const Extrusion &extrusion,
						const Polygon & polygon);

	libthing::Vector2 startPoint(const SliceData &sliceData);
    // void writeWipeExtruder(std::ostream& ss, int extruderId) const {};
};


}
#endif

