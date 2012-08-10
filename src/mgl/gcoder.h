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


/// Properties of an extrusion profile
/// an extruder may have multiple extrusion profiles
/// EG: large, fast, 'first layer'
class Extrusion {
public:
	Extrusion() {}

	Scalar crossSectionArea(Scalar height, Scalar width) const;

	Scalar feedrate;
	Scalar temperature;
};


/// Properties common to a single hardware extruder
class Extruder {
public:
	
	Extruder() {}

	Scalar feedCrossSectionArea() const;

	Scalar isVolumetric() const { return true; };

	Scalar feedDiameter;
	Scalar nozzleDiameter;
	unsigned char code;
	int id;

	Scalar retractDistance;
	Scalar retractRate;
	Scalar restartExtraDistance;

	std::string firstLayerExtrusionProfile;
	std::string insetsExtrusionProfile;
	std::string infillsExtrusionProfile;
};


class GCoderConfig {
public:
	GCoderConfig() : programName(GRUE_PROGRAM_NAME), 
			versionStr(GRUE_VERSION), 
            startX(BAD_SCALAR), 
            startY(BAD_SCALAR){}
    
    static const Scalar BAD_SCALAR = 999999;

    std::string programName;
    std::string versionStr;
  

    GantryConfig gantryCfg;

    std::map<std::string, Extrusion> extrusionProfiles;
    std::vector<Extruder> extruders;


    bool doOutlines;
    bool doInsets;
	bool doSupport;
    bool doInfills;
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
	if(path.size() < 2){
		GcoderException mixup("Attempted to write path with no points");
		throw mixup;
	}
	typename PATH::const_iterator current = path.fromStart();
	PointType last = *current;
	++current;
	// rapid move into position
	PointType gantryPos(gantry.get_x(), gantry.get_y());
	if((gantryPos - last).magnitude() >= gcoderCfg.gantryCfg.get_coarseness()) {
		gantry.snort(ss, extruder, extrusion);
		gantry.g1(ss, extruder, extrusion,
				last.x, last.y, z, 
				gcoderCfg.gantryCfg.get_rapid_move_feed_rate_xy(), 
				0, 0, 
				"move into position");
	} 
	gantry.squirt(ss, extruder, extrusion);
	for(; current!=path.end(); ++current){
		PointType relative = (*current)-last;
		
		std::stringstream comment;
		Scalar distance = relative.magnitude();
		comment << "d: " << distance;
		gantry.g1(ss, extruder, extrusion, 
				current->x, current->y, z, 
				extrusion.feedrate, h, w, comment.str().c_str());
		last = *current;
	}
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
	bool didLastPath = true;
	for(const_iterator iter = labeledPaths.begin(); 
			iter != labeledPaths.end(); 
			++iter) {
		const LabeledOpenPath& currentLP = *iter;
		Extrusion extrusion;
		Scalar currentH = h;
		Scalar currentW = w;
		if(currentLP.myLabel.isOutline()) {
			if(!gcoderCfg.doOutlines) {
				didLastPath = false;
				continue;
			}
			calcInSetExtrusion(extruder.id, layerSequence, 
					currentLP.myLabel.myValue, -1, extrusion);
			ss << "(outline path, length: " << currentLP.myPath.size() 
					<< ")" << std::endl;
		} else if(currentLP.myLabel.isSupport()) {
			calcInfillExtrusion(extruder.id, layerSequence, extrusion);
			ss << "(support path, length: " << currentLP.myPath.size() 
					<< ")" << std::endl;
		} else if(currentLP.myLabel.isConnection()) {
			if(!didLastPath)
				continue;
			calcInfillExtrusion(extruder.id, layerSequence, extrusion);
			ss << "(connection path, length: " << currentLP.myPath.size() 
					<< ")" << std::endl;
		} else if(currentLP.myLabel.isInset()) {
			if(!gcoderCfg.doInsets) {
				didLastPath = false;
				continue;
			}
			calcInSetExtrusion(extruder.id, layerSequence, 
					currentLP.myLabel.myValue, -1, extrusion);
			ss << "(inset path, length: " << currentLP.myPath.size() 
					<< ")" << std::endl;
		} else if(currentLP.myLabel.isInfill()) {
			if(!gcoderCfg.doInfills) {
				didLastPath = false;
				continue;
			}
			calcInfillExtrusion(extruder.id, layerSequence, extrusion);
			ss << "(infill path, length: " << currentLP.myPath.size() 
					<< ")" << std::endl;
		} else {
			GcoderException mixup("Invalid path label type");
			throw mixup;
		}
		didLastPath = true;
		writePath(ss, z, currentH, currentW, extruder, extrusion, currentLP.myPath);
	}
	gantry.snort(ss, extruder, fluidstrusion);
	ss << std::endl << std::endl;
}


}
#endif

