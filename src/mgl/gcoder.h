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


#include "gcoder_gantry.h"
#include "log.h"

namespace mgl
{

class GcoderException : public Exception {	
public: 
	GcoderException(const char *msg) 
			: Exception(msg){} 
};


/// Properties of a print platform
class Platform{
public:
	Platform() : temperature(100),
			automated(false){}
	
	Scalar temperature;				// temperature of the platform during builds
	bool automated;

	// the wiper(s) are affixed to the platform
//    Scalar waitingPositionX;
//    Scalar waitingPositionY;
//    Scalar waitingPositionZ;
};

/// Properties of an extrusion profile
/// an extruder may have multiple extrusion profiles
/// EG: large, fast, 'first layer'
class Extrusion {
public:
	Extrusion() : feedrate(2400),
			retractDistance(0),
			retractRate(3000),
			restartExtraDistance(0), 
			flow(2.8),
			leadIn(0),
			leadOut(0),
			snortFlow(35),
			snortFeedrate(600),
			squirtFlow(35),
			squirtFeedrate(600)	{}

	Scalar crossSectionArea(Scalar height, Scalar width) const;

	Scalar feedrate;

	Scalar retractDistance;
	Scalar retractRate;
	Scalar restartExtraDistance;

	Scalar flow; // RPM value for the extruder motor... not a real unit :-(

	Scalar leadIn;
	Scalar leadOut;

	Scalar snortFlow;
	Scalar snortFeedrate;
	Scalar squirtFlow;
	Scalar squirtFeedrate;
};


/// Properties common to a single hardware extruder
class Extruder {
public:
	
	enum extrusionMode_t {
		RPM_MODE, 
		VOLUMETRIC_MODE
	}; 

	Extruder() : coordinateSystemOffsetX(0),
			extrusionTemperature(220),
			nozzleZ(0),
			zFeedRate(100),
			extrusionMode(VOLUMETRIC_MODE),
			feedDiameter(3),
			code('A'),
			id(0) {}

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


//// a line around the print used as a print 'skirt'
///
class Outline {
public:
	Outline() : enabled(false), distance(3.0) {}
	bool enabled;   // when true, a rectangular ouline of the part will be performed
	Scalar distance; // the distance in mm  between the model and the rectangular outline
};




class GCoderConfig {
public:
	GCoderConfig() : programName(GRUE_PROGRAM_NAME), 
			versionStr(GRUE_VERSION) {}

    std::string programName;
    std::string versionStr;
    std::string machineName;
    std::string firmware;

    Platform platform;
    Outline outline;
    GantryConfig gantryCfg;

    std::map<std::string, Extrusion> extrusionProfiles;
    std::vector<Extruder> extruders;


    bool doOutlines;
    bool doInsets;
	bool doSupport;
    bool doInfills;
    bool doInfillsFirst;
	bool doPrintLayerMessages;

	Scalar startX;
	Scalar startY;
    std::string header;
    std::string footer;

	unsigned int defaultExtruder;
};





//
// This class contains settings for the 3D printer,
// user preferences as well as runtime information
//
class GCoder : public Progressive
{
public:

	GCoderConfig gcoderCfg;
	Gantry gantry;
	Scalar distanceTol;

	GCoder(const GCoderConfig &gCoderCfg, ProgressBar* progress=NULL);

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
	void writeGcodeFile(LayerPaths& layerpaths, 
			const LayerMeasure& layerMeasure, 
			std::ostream& gout, 
			const std::string& title);
	void writeGcodeFile(LayerPaths& layerpaths, 
			const LayerMeasure& layerMeasure, 
			std::ostream& gout, 
			const std::string& title,
			LayerPaths::layer_iterator begin,
			LayerPaths::layer_iterator end);

    ///  returns extrusionParams set based on the extruder id, and where you
    /// are in the model
    void calcInfillExtrusion(unsigned int extruderId,
			unsigned int sliceId,
			Extrusion &extrusionParams) const;
	void calcInfillExtrusion(const LayerPaths& layerpaths, 
			unsigned int extruderId, 
			LayerPaths::const_layer_iterator layerId, 
			Extrusion& extrusionParams) const;

    ///  returns extrusionParams set based on the extruder id, and where you
    /// are in the model
    void calcInSetExtrusion(unsigned int extruderId,
			unsigned int sliceId,
			unsigned int insetId,
			unsigned int insetCount,
			Extrusion &extrusionParams) const;
	void calcInSetExtrusion(const LayerPaths& layerpaths, 
			unsigned int extruderId, 
			LayerPaths::const_layer_iterator layerId, 
			LayerPaths::Layer::ExtruderLayer::const_inset_iterator insetId, 
			Extrusion& extrusionParams) const;
	

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
	void writeSlice(std::ostream& ss, 
			LayerPaths& layerpaths, 
			LayerPaths::layer_iterator layerIter, 
			size_t layerSequence);

private:

    void writeGCodeConfig(std::ostream & ss, const char* filename) const;
//    void writeMachineInitialization(std::ostream & ss) const;
//    void writePlatformInitialization(std::ostream & ss) const;
//    void writeExtrudersInitialization(std::ostream & ss) const;
//    void writeHomingSequence(std::ostream & ss);
//    void writeWarmupSequence(std::ostream & ss);
//    void writeAnchor(std::ostream & ss);
	void writeInfills(std::ostream& ss, 
					  Scalar z, Scalar h, Scalar w, 
					  size_t sliceId, 
					  const Extruder& extruder, 
					  const LayerPaths::Layer::ExtruderLayer& paths);
	void writeSupport(std::ostream& ss,
					  Scalar z, Scalar h, Scalar w, 
					  size_t sliceId,
					  const Extruder& extruder, 
					  const LayerPaths::Layer::ExtruderLayer& paths);
	void writeInsets(std::ostream& ss, 
			Scalar z, Scalar h, Scalar w, 
			size_t sliceId, 
			const Extruder& extruder, 
			const LayerPaths& layerpaths,
			LayerPaths::layer_iterator layerId, 
			const LayerPaths::Layer::ExtruderLayer& paths);
	void writeOutlines(std::ostream& ss, 
			Scalar z, Scalar h, Scalar w, 
			size_t sliceId, 
			const Extruder& extruder, 
			const LayerPaths::Layer::ExtruderLayer& paths);
	template <typename PATH>
	void writePath(std::ostream& ss, 
			Scalar z, Scalar h, Scalar w, 
			const Extruder& extruder,
			const Extrusion& extrusion, 
			const PATH& path);
	template <template <class, class> class LABELEDPATHS, class ALLOC>
	void writePaths(std::ostream& ss, 
			Scalar z, Scalar h, Scalar w, 
			size_t layerSequence, 
			const Extruder& extruder, 
			const LABELEDPATHS<LabeledOpenPath, ALLOC>& labeledPaths);

	libthing::Vector2 startPoint(const SliceData &sliceData);
    // void writeWipeExtruder(std::ostream& ss, int extruderId) const {};
};

template <typename PATH>
void GCoder::writePath(std::ostream& ss, 
		Scalar z, Scalar h, Scalar w, 
		const Extruder& extruder, 
		const Extrusion& extrusion, 
		const PATH& path) {
	if(path.fromStart() == path.end()){
		GcoderException mixup("Attempted to write path with no points");
		throw mixup;
	}
	typename PATH::const_iterator current = path.fromStart();
	PointType last = *current;
	++current;
	// rapid move into position
	if(gantry.get_x() != last.x || gantry.get_y() != last.y) {
		gantry.snort(ss, extruder, extrusion);
		gantry.g1(ss, extruder, extrusion,
				last.x, last.y, z, 
				gcoderCfg.gantryCfg.get_rapid_move_feed_rate_xy(), 
				0, 0, 
				"move into position");
		gantry.squirt(ss, extruder, extrusion);
	}
	for(; current!=path.end(); ++current){
		PointType relative = (*current)-last;
		typename PATH::const_iterator nextIter = current;
		++nextIter;
		// if the next point is far enough or the last one
		if(nextIter == path.end() || 
				relative.magnitude() > distanceTol) {
			//the output the point
			std::stringstream comment;
			Scalar distance = relative.magnitude();
			comment << "d: " << distance;
			gantry.g1(ss, extruder, extrusion, 
					current->x, current->y, z, 
					extrusion.feedrate, h, w, comment.str().c_str());
			last = *current;
		} else {
			// otherwise don't
			//Log::severe() << "Attempt to extrude segment of length " 
			//		<< relative.magnitude() << std::endl;
		}
	}
	gantry.snort(ss, extruder, extrusion);
	ss << std::endl;
	//Log::severe() << "IMPLEMNT WRITING OF PATHS!" << std::endl;
}

template <template <class, class> class LABELEDPATHS, class ALLOC>
void GCoder::writePaths(std::ostream& ss, 
		Scalar z, Scalar h, Scalar w, 
		size_t layerSequence, 
		const Extruder& extruder, 
		const LABELEDPATHS<LabeledOpenPath, ALLOC>& labeledPaths) {
	typedef typename LABELEDPATHS<LabeledOpenPath, ALLOC>::const_iterator 
			const_iterator;
	Extrusion fluidstrusion;
	calcInfillExtrusion(extruder.id, layerSequence, fluidstrusion);
	gantry.snort(ss, extruder, fluidstrusion);
	for(const_iterator iter = labeledPaths.begin(); 
			iter != labeledPaths.end(); 
			++iter) {
		const LabeledOpenPath& currentLP = *iter;
		Extrusion extrusion;
		switch(currentLP.myLabel.myType) {
		case PathLabel::TYP_OUTLINE:
			if(!gcoderCfg.doOutlines)
				continue;
			calcInSetExtrusion(extruder.id, layerSequence, 
					currentLP.myLabel.myValue, -1, extrusion);
			break;
		case PathLabel::TYP_INSET:
			if(!gcoderCfg.doInsets)
				continue;
			calcInSetExtrusion(extruder.id, layerSequence, 
					currentLP.myLabel.myValue, -1, extrusion);
			break;
		case PathLabel::TYP_INFILL:
			if(!gcoderCfg.doInfills)
				continue;
			calcInfillExtrusion(extruder.id, layerSequence, extrusion);
			break;
		default:
			GcoderException mixup("Invalid path label type");
			throw mixup;
			break;
		}
		writePath(ss, z, h, w, extruder, extrusion, currentLP.myPath);
	}
	gantry.snort(ss, extruder, fluidstrusion);
}


}
#endif

