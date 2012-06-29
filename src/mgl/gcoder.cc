/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/

#include "gcoder.h"

#include "log.h"
#include <math.h>
#include <string>
#include <list>

namespace mgl {

using namespace std;
using namespace libthing;


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
void polygonLeadInAndLeadOut(const mgl::Polygon &polygon, const Extruder &extruder,
		double leadIn, double leadOut,
		Vector2 &start, Vector2 &end) {
	size_t count =  polygon.size();

	const Vector2 &a = polygon[0];	// first element
	const Vector2 &b = polygon[1];

	const Vector2 &c = polygon[count-2];
	const Vector2 &d = polygon[count-1]; // last element

	if (extruder.isVolumetric()) {
		start = a;
		end = d;
		return;
	}

	Vector2 ab = b - a;
	ab.normalise();
	Vector2 cd = d-c;
	cd.normalise();

	start.x = a.x - ab.x * leadIn;
	start.y = a.y - ab.y * leadIn;
	end.x   = d.x + cd.x * leadOut;
	end.y   = d.y + cd.y * leadOut;

}

GCoder::GCoder(const GCoderConfig &gCoderCfg, 
		ProgressBar* progress) : 
		Progressive(progress), 
		gcoderCfg(gCoderCfg), 
		gantry(gCoderCfg.gantryCfg) {
	gantry.init_to_start();
}


/**
 * Writes intial gcode data to start of the gcode file, including setup & startup info
 * @param gout - output stream for the gcode text
 * @param sourceName - source of this gcode (usually the origional stl file)
 */
void GCoder::writeStartDotGCode(std::ostream &gout, const char* sourceName)
{
	gout.precision(3);
	gout.setf(ios::fixed);

	writeGCodeConfig(gout, sourceName);

    const string &header_file = gcoderCfg.header;

	if (header_file.length() > 0) {
		ifstream header_in(header_file.c_str(), ifstream::in);

		if (header_in.fail()) 
			throw GcoderException((string("Unable to open header file [") +
								   header_file + "]").c_str());

		gout << "(header [" << header_file << "] begin)" << endl;

		while (header_in.good()) {
			char buf[1024];

			header_in.read(buf, sizeof(buf));
			gout.write(buf, header_in.gcount());
		}

		if (header_in.fail() && !header_in.eof())
			throw GcoderException((string("Error reading header file [") +
								   header_file + "]").c_str());

		gout << "(header [" << header_file << "] end)" << endl << endl;
	}
}

void GCoder::writeEndDotGCode(std::ostream &ss) const {
    const string &footer_file = gcoderCfg.footer;


	if (footer_file.length() > 0) {
		ifstream footer_in(footer_file.c_str(), ifstream::in);

		if (footer_in.fail()) 
			throw GcoderException((string("Unable to open footer file [") +
								   footer_file + "]").c_str());

		ss << "(footer [" << footer_file << "] begin)" << endl;

		while (footer_in.good()) {
			char buf[1024];

			footer_in.read(buf, sizeof(buf));
			ss.write(buf, footer_in.gcount());
		}

		if (footer_in.fail() && !footer_in.eof())
			throw GcoderException((string("Error reading footer file [") +
								   footer_file + "]").c_str());

		ss << "(footer [" << footer_file << "] end)" << endl << endl;
	} else {
		for (size_t i=0; i< gcoderCfg.extruders.size(); i++)
		{
			ss << "M104 S0 T" << i << " (set extruder temperature to 0)" << endl;
			ss << "M109 S0 T" << i << " (set heated-build-platform id tied an extrusion tool)" << endl;
		}

		if(gcoderCfg.gantryCfg.get_z_max_homing())
			ss << "G162 Z F500 (home Z axis maximum)" << endl;
		ss << "(That's all folks!)" << endl;
	}
}


void GCoder::writePolygon(std::ostream & ss,
		Scalar z,
		const Extruder &extruder,
		const Extrusion &extrusion,
		const Polygon & polygon) {
	Vector2 start(0, 0), stop(0, 0);

	polygonLeadInAndLeadOut(polygon, extruder, extrusion.leadIn, extrusion.leadOut, start, stop);

	// rapid move into position
	gantry.g1(ss, extruder, extrusion,
			start.x, start.y, z,
			gcoderCfg.gantryCfg.get_rapid_move_feed_rate_xy(),
			"move into position");

	// start extruding ahead of time while moving towards the first point
	gantry.squirt(ss, polygon[0], extruder, extrusion);

	Vector2 last = start;
	// for all other points in the polygon
	for (size_t i = 1; i < polygon.size(); i++) {
		// move towards the point
		const Vector2 &p = polygon[i];
		Scalar dist = LineSegment2(last, p).length();
		stringstream comment;
		comment << "d: " << dist;

		gantry.g1(ss, extruder, extrusion, p.x, p.y, z,
				extrusion.feedrate, comment.str().c_str());
	}
	//ss << "(STOP!)" << endl;
	gantry.snort(ss, stop, extruder, extrusion);
	//ss << "(!STOP)" << endl;
	ss << endl;

}

void GCoder::writePolygons(std::ostream& ss,
		Scalar z,
		const Extruder &extruder,
		const Extrusion &extrusion,
		const Polygons &paths) {
	unsigned int pathCount = paths.size();
	for (unsigned int i = 0; i < pathCount; i++) {

		const Polygon &polygon = paths[i];
		ss << "(  path " << i << "/" << pathCount << ", " << polygon.size() << " points, " << " )" << endl;

		unsigned int pointCount = polygon.size();
		if (pointCount < 2) {
			stringstream ss;
			ss << "Can't generate gcode for polygon " << i << " with " << pointCount << " points.";
			GcoderException mixup(ss.str().c_str());
			throw mixup;
		}

		writePolygon(ss, z, extruder, extrusion, polygon);
	}
}

void GCoder::writeInfills(std::ostream& ss, 
		Scalar z, 
		size_t sliceId, 
		const Extruder& extruder,  
		const LayerPaths::Layer::ExtruderLayer& paths) {
	try {
		ss << "(infills: "  << paths.infillPaths.size() << ")"<< endl;
		Extrusion extrusion;
		calcInfillExtrusion(extruder.id, sliceId, extrusion);
		for(LayerPaths::Layer::ExtruderLayer::const_infill_iterator iter = 
				paths.infillPaths.begin(); 
				iter != paths.infillPaths.end(); 
				++iter){
			writePath(ss, z, extruder, extrusion, *iter);
		}
	} catch (GcoderException& mixup) {
		Log::info() << "ERROR writing infills in slice " <<
				sliceId << " for extruder " <<
				extruder.id << " : " << mixup.error << endl;
		Log::severe() << "ERROR writing infills in slice " <<
				sliceId << " for extruder " <<
				extruder.id << " : " << mixup.error << endl;
	}
}

void GCoder::writeInsets(std::ostream& ss, 
		Scalar z, 
		size_t sliceId, 
		const Extruder& extruder, 
		const LayerPaths& layerpaths, 
		LayerPaths::layer_iterator layerId, 
		const LayerPaths::Layer::ExtruderLayer& paths) {
	try {
		ss << "(insets: "  << paths.insetPaths.size() << ")"<< endl;
		Extrusion extrusion;
		for(LayerPaths::Layer::ExtruderLayer::const_inset_iterator i = 
				paths.insetPaths.begin(); 
				i != paths.insetPaths.end(); 
				++i){
			calcInSetExtrusion(layerpaths, extruder.id, layerId, i, 
					extrusion);
			for (LoopPathList::const_iterator j = i->begin();
				 j != i->end(); ++j) {
				writePath(ss, z, extruder, extrusion, *j);
			}
		}
	} catch (GcoderException& mixup) {
		Log::info() << "ERROR writing insets in slice " <<
				sliceId << " for extruder " <<
				extruder.id << " : " << mixup.error << endl;
		Log::severe() << "ERROR writing insets in slice " <<
				sliceId << " for extruder " <<
				extruder.id << " : " << mixup.error << endl;
	} 
}

void GCoder::writeOutlines(std::ostream& ss, 
		Scalar z, 
		size_t sliceId, 
		const Extruder& extruder,  
		const LayerPaths::Layer::ExtruderLayer& paths) {
	try {
		ss << "(outlines: "  << paths.outlinePaths.size() << ")"<< endl;
		Extrusion extrusion;
		calcInfillExtrusion(extruder.id, sliceId, extrusion);
		for(LayerPaths::Layer::ExtruderLayer::const_outline_iterator iter = 
				paths.outlinePaths.begin(); 
				iter != paths.outlinePaths.end(); 
				++iter){
			writePath(ss, z, extruder, extrusion, *iter);
		}
	} catch (GcoderException& mixup) {
		Log::info() << "ERROR writing outlines in slice " <<
				sliceId << " for extruder " <<
				extruder.id << " : " << mixup.error << endl;
		Log::severe() << "ERROR writing outlines in slice " <<
				sliceId << " for extruder " <<
				extruder.id << " : " << mixup.error << endl;
	}
}

void GCoder::moveZ(ostream & ss, Scalar z, unsigned int, Scalar zFeedrate) {
	bool doX = false;
	bool doY = false;
	bool doZ = true;
	bool doE = false;
	bool doFeed = true;


	gantry.g1Motion(ss, 0, 0, z, 0, zFeedrate, "move Z", doX, doY, doZ, doE, doFeed);

}


void GCoder::calcInfillExtrusion(unsigned int extruderId, unsigned int sliceId, Extrusion &extrusion) const
{
	string profileName;
	if(sliceId == 0)
	{
		profileName = gcoderCfg.extruders[extruderId].firstLayerExtrusionProfile;
	}
	else
	{
		profileName = gcoderCfg.extruders[extruderId].infillsExtrusionProfile;
	}

	const std::map<std::string, Extrusion>::const_iterator it = gcoderCfg.extrusionProfiles.find(profileName);
	if(it == gcoderCfg.extrusionProfiles.end()){
		Log::severe() << "Failed to find extrusion profile <name>" << 
				profileName  << "</name>" << endl;
//		GcoderException mixup((string("Failed to find extrusion profile ") + 
//				profileName).c_str());
//		throw mixup;
	} else {
		extrusion = it->second;
	}
	extrusion.feedrate *= gcoderCfg.gantryCfg.get_scaling_factor();
	extrusion.flow *= gcoderCfg.gantryCfg.get_scaling_factor();
}
void GCoder::calcInfillExtrusion(const LayerPaths& layerpaths, 
		unsigned int extruderId, 
		LayerPaths::const_layer_iterator layerId, 
		Extrusion& extrusionParams) const {
	string profileName = layerId == layerpaths.begin() ? 
			gcoderCfg.extruders[extruderId].firstLayerExtrusionProfile :
			gcoderCfg.extruders[extruderId].infillsExtrusionProfile;
	
	const std::map<std::string, Extrusion>::const_iterator it = 
			gcoderCfg.extrusionProfiles.find(profileName);
	if(it == gcoderCfg.extrusionProfiles.end()){
		Log::severe() << "Failed to find extrusion profile <name>" << 
				profileName  << "</name>" << endl;
	} else {
		extrusionParams = it->second;
	}
	extrusionParams.feedrate *= gcoderCfg.gantryCfg.get_scaling_factor();
	extrusionParams.flow *= gcoderCfg.gantryCfg.get_scaling_factor();
}

void GCoder::calcInSetExtrusion(unsigned int extruderId,
		unsigned int sliceId,
		unsigned int, // insetId,
		unsigned int, // insetCount,
		Extrusion &extrusion) const {
	string profileName;
	if (sliceId == 0) {
		profileName = gcoderCfg.extruders[extruderId].firstLayerExtrusionProfile;
	} else {
		profileName = gcoderCfg.extruders[extruderId].insetsExtrusionProfile;
	}

	const std::map<std::string, Extrusion>::const_iterator &it = 
			gcoderCfg.extrusionProfiles.find(profileName);
	if(it == gcoderCfg.extrusionProfiles.end()){
		Log::severe() << "Failed to find extrusion profile <name>" << 
				profileName  << "</name>" << endl;
	} else {
		extrusion = it->second;
	}
	extrusion = it->second;
	extrusion.feedrate *= gcoderCfg.gantryCfg.get_scaling_factor();
	extrusion.flow *= gcoderCfg.gantryCfg.get_scaling_factor();
}

void GCoder::calcInSetExtrusion(const LayerPaths& layerpaths, 
		unsigned int extruderId, 
		LayerPaths::const_layer_iterator layerId, 
		LayerPaths::Layer::ExtruderLayer::const_inset_iterator insetId, 
		Extrusion& extrusionParams) const {
	string profileName = layerId == layerpaths.begin() ? 
			gcoderCfg.extruders[extruderId].firstLayerExtrusionProfile :
			gcoderCfg.extruders[extruderId].infillsExtrusionProfile;
	
	const std::map<std::string, Extrusion>::const_iterator it = 
			gcoderCfg.extrusionProfiles.find(profileName);
	if(it == gcoderCfg.extrusionProfiles.end()){
		Log::severe() << "Failed to find extrusion profile <name>" << 
				profileName  << "</name>" << endl;
	} else {
		extrusionParams = it->second;
	}
	extrusionParams.feedrate *= gcoderCfg.gantryCfg.get_scaling_factor();
	extrusionParams.flow *= gcoderCfg.gantryCfg.get_scaling_factor();
}

void GCoder::writeGcodeFile(std::vector <SliceData>& slices,
		const mgl::LayerMeasure& layerMeasure,
		std::ostream &gout,
		const char* title,
		int iFirstSliceIdx,
		int iLastSliceIdx) {
	writeStartDotGCode(gout, title);

	size_t sliceCount = slices.size();
	size_t firstSliceIdx = 0;
	size_t lastSliceIdx = sliceCount - 1;

	if (iFirstSliceIdx > 0) firstSliceIdx = (size_t) iFirstSliceIdx;
	if (iLastSliceIdx < 0) lastSliceIdx = (size_t) iLastSliceIdx;

	Extruder &extruder = gcoderCfg.extruders[0];
	Extrusion extrusion;
	calcInfillExtrusion(0, 0, extrusion);
	Vector2 start = startPoint(slices[0]);
	gantry.squirt(gout, start, extruder, extrusion);
	gantry.g1(gout, extruder, extrusion, start.x, start.y,
			gantry.get_z(), extrusion.feedrate,
			"Extrude into position");

	initProgress("gcode", sliceCount);
	size_t codeSlice = 0;

	for (size_t sliceId = 0; sliceId < sliceCount; sliceId++) {
		tick();
		if (sliceId < firstSliceIdx) continue;
		if (sliceId > lastSliceIdx) break;

		Scalar z = layerMeasure.sliceIndexToHeight(codeSlice);
		SliceData &slice = slices[sliceId];
		slice.updatePosition(z, sliceId);
		writeSlice(gout, slice);
		codeSlice++;
	}

	writeEndDotGCode(gout);
}
void GCoder::writeGcodeFile(LayerPaths& layerpaths, 
		const LayerMeasure& layerMeasure, 
		std::ostream& gout, 
		const std::string& title) {
	writeGcodeFile(layerpaths, 
			layerMeasure,
			gout, 
			title, 
			layerpaths.begin(), 
			layerpaths.end());
}
void GCoder::writeGcodeFile(LayerPaths& layerpaths, 
		const LayerMeasure& layerMeasure, 
		std::ostream& gout, 
		const std::string& title, 
		LayerPaths::layer_iterator begin, 
		LayerPaths::layer_iterator end) {
	writeStartDotGCode(gout, title.c_str());
	size_t sliceCount = 0;
	for(LayerPaths::const_layer_iterator it = begin; 
			it != end;
			++it, ++sliceCount);
	initProgress("gcode", sliceCount);
	size_t codeSlice = 0;
	size_t layerIndex = 0;
	for(LayerPaths::const_layer_iterator it = layerpaths.begin(); 
			it != begin;
			++it, ++layerIndex);
	for(LayerPaths::layer_iterator it = begin; 
			it != end; ++it, ++codeSlice, ++layerIndex){
		tick();
		LayerPaths::Layer& currentLayer = *it;
		Scalar z = layerMeasure.sliceIndexToHeight(codeSlice);
		currentLayer.layerZ = z;
		currentLayer.layerId = layerIndex;
		writeSlice(gout, layerpaths, it);
	}
	writeEndDotGCode(gout);
}

Vector2 GCoder::startPoint(const SliceData& sliceData) {
	if (gcoderCfg.doInfills && gcoderCfg.doInfillsFirst) {
		return sliceData.extruderSlices[0].infills[0][0];
	}
	else if (gcoderCfg.doOutlines) {
		return sliceData.extruderSlices[0].boundary[0][0];
	}
	else if (gcoderCfg.doInsets) {
		if (sliceData.extruderSlices.size() < 1)
			throw Exception("zero extruder slices for finding start point");

		if (sliceData.extruderSlices[0].insetLoopsList.size() < 1)
			throw Exception("zero inset loops for finding start point");

		if (sliceData.extruderSlices[0].insetLoopsList[0].size() < 1)
			throw Exception("zero loops for finding start point");

		return sliceData.extruderSlices[0].insetLoopsList[0][0][0];
	}
	else {
		return sliceData.extruderSlices[0].infills[0][0];
	}
}

void GCoder::writeSlice(ostream& ss, const SliceData& sliceData )
{
	Scalar layerZ = sliceData.getZHeight();
	unsigned int sliceIndex = sliceData.getIndex();
	unsigned int extruderCount = sliceData.extruderSlices.size();

	ss << "(Slice " << sliceIndex << ", " << extruderCount << 
			" " << plural("Extruder", extruderCount) << ")"<< endl;
	if(gcoderCfg.doPrintLayerMessages){
		ss << "M70 P20 (Layer: " << sliceIndex << ")" << endl;
	}
	// to each extruder its speed
	Scalar zFeedrate = gcoderCfg.gantryCfg.get_scaling_factor() * gcoderCfg.extruders[0].zFeedRate;
	// moving all up. This is the first move for every new layer

	for(unsigned int extruderId = 0; extruderId < extruderCount; extruderId++)
	{
	    Scalar z = layerZ + gcoderCfg.extruders[extruderId].nozzleZ;
		Extruder &extruder = gcoderCfg.extruders[extruderId];

		try {
			moveZ(ss, z, extruderId, zFeedrate);
		} catch (GcoderException &mixup) {
			Log::info() << "ERROR writing Z move in slice " <<
					sliceIndex << " for extruder " << extruderId <<
					" : " << mixup.error << endl;
		}

	  	ss << "(   Extruder " <<  extruderId << ")" << endl;
		const Polygons &loops = sliceData.extruderSlices[extruderId].boundary;
		const Polygons &infills = sliceData.extruderSlices[extruderId].infills;
		const vector<Polygons> &insets = sliceData.extruderSlices[extruderId].insetLoopsList;

                // Log::often() << endl <<  "Slice " << sliceData.sliceIndex << endl;

		try {
			if (extruderCount > 0) {
				gantry.writeSwitchExtruder(ss, extruder);
			}
			if(gcoderCfg.doInfills && gcoderCfg.doInfillsFirst) {
				Extrusion extrusion;
				calcInfillExtrusion(extruderId, sliceIndex, extrusion);
				ss << "(infills: "  << infills.size() << ")"<< endl;
				writePolygons(ss, z, extruder, extrusion, infills);
			}
		} catch(GcoderException &mixup)	{
			Log::info() << "ERROR writing infills in slice " << 
					sliceIndex  << " for extruder " << 
					extruderId << " : " << mixup.error << endl;
			Log::severe() << "ERROR writing infills in slice " << 
					sliceIndex  << " for extruder " << 
					extruderId << " : " << mixup.error << endl;
		}
		/// Write outlines? outlines == skirt
		try
		{

			if(gcoderCfg.doOutlines)
			{
				Extrusion extrusion;
				calcInfillExtrusion(extruderId, sliceIndex, extrusion);
								//Log::often()  << "   Write OUTLINE" << endl;
				ss << "(outlines: " << loops.size() << " )"<< endl;
				writePolygons(ss, z, extruder, extrusion, loops);
			}
		}
		catch(GcoderException &mixup)
		{
                    Log::info()  << "ERROR writing loops in slice " << 
							sliceIndex  << " for extruder " << 
							extruderId << " : " << mixup.error << endl;
                    Log::severe() << "ERROR writing loops in slice " << 
							sliceIndex  << " for extruder " << 
							extruderId << " : " << mixup.error << endl;
                    cerr << mixup.error << endl;
		}

		try
		{
			if( gcoderCfg.doInsets )
			{
				// each iteration is for a shell
				unsigned int insetCount = insets.size();
				for(unsigned int i=0; i < insetCount; i++)
				{
					Extrusion extrusion;
					calcInSetExtrusion(extruderId, sliceIndex, i, 
							insetCount, extrusion);
					const Polygons &inset = insets[i];
                                        // Log::often() << "   Write INSETS " << i << endl;
					ss << "(inset " << i << "/"<<  insetCount<< " )"<< endl;
					writePolygons(ss, z, extruder, extrusion, inset);

				}
			}
		} catch(GcoderException &mixup) {
            Log::info() << "ERROR writing insets in slice " << 
					sliceIndex  << " for extruder " << 
					extruderId << " : " << mixup.error << endl;
			cerr << "ERROR writing insets in slice " << 
					sliceIndex  << " for extruder " << 
					extruderId << " : " << mixup.error << endl;
		}

		try
		{
			if(gcoderCfg.doInfills && ! gcoderCfg.doInfillsFirst)
			{
                                //Log::often() << "   Write INFILLS" << endl;
				ss << "(infills: "  << infills.size() << ")"<< endl;
				Extrusion extrusion;
				calcInfillExtrusion(extruderId, sliceIndex, extrusion);
				writePolygons(ss, z, extruder, extrusion, infills);
			}
		}
		catch(GcoderException &mixup)
		{
            Log::info() << "ERROR writing infills in slice " << 
					sliceIndex  << " for extruder " << 
					extruderId << " : " << mixup.error << endl;
			cerr << "ERROR writing infills in slice " << 
					sliceIndex  << " for extruder " << 
					extruderId << " : " << mixup.error << endl;
		}


		extruderId ++;
	}
}

void GCoder::writeSlice(std::ostream& ss, 
		LayerPaths& layerpaths, 
		LayerPaths::layer_iterator layerId) {
	LayerPaths::Layer& currentLayer = *layerId;
	unsigned int sliceIndex = currentLayer.layerId;
	unsigned int extruderCount = currentLayer.extruders.size();
	ss << "(Slice " << sliceIndex << ", " << extruderCount << 
			" " << plural("Extruder", extruderCount) << ")"<< endl;
	if(gcoderCfg.doPrintLayerMessages){
		//print layer message to printer screen if config enabled
		ss << "M70 P20 (Layer: " << sliceIndex << ")" << endl;
	}
	//iterate over all extruders invoked in this layer
	for(LayerPaths::Layer::const_extruder_iterator it = 
			currentLayer.extruders.begin(); 
			it != currentLayer.extruders.end(); 
			++it){
		//this is the current extruder
		Extruder& currentExtruder = gcoderCfg.extruders[it->extruderId];
		//this is the current extruder's zFeedrate
		Scalar zFeedrate = gcoderCfg.gantryCfg.get_scaling_factor() * 
				currentExtruder.zFeedRate;
		try {
			moveZ(ss, currentLayer.layerZ, currentExtruder.id, zFeedrate);
		} catch(GcoderException& mixup) {
			Log::info() << "ERROR writing Z move in slice " <<
					sliceIndex << " for extruder " << currentExtruder.id <<
					" : " << mixup.error << endl;
		}
		if(gcoderCfg.doInfills && gcoderCfg.doInfillsFirst) {
			writeInfills(ss, currentLayer.layerZ, currentLayer.layerId, 
					currentExtruder, *it);
		}
		if(gcoderCfg.doOutlines) {
			writeOutlines(ss, currentLayer.layerZ, currentLayer.layerId, 
					currentExtruder, *it);
		}
		if(gcoderCfg.doInsets) {
			writeInsets(ss, currentLayer.layerZ, currentLayer.layerId, 
					currentExtruder, layerpaths, layerId, *it);			
		}
		if(gcoderCfg.doInfills && !gcoderCfg.doInfillsFirst) {
			writeInfills(ss, currentLayer.layerZ, currentLayer.layerId, 
					currentExtruder, *it);
		}
	}
}

Scalar Extrusion::crossSectionArea(Scalar height) const {
	Scalar width = height * extrudedDimensionsRatio;

	//two semicircles joined by a rectangle
	Scalar radius = height / 2;
	return (M_TAU / 2) * radius * radius + height * (width - height);
	//LONG LIVE TAU!
}

Scalar Extruder::feedCrossSectionArea()  const {
	Scalar radius = feedDiameter / 2;
	//feedstock should be a cylinder
	return (M_TAU / 2) * radius * radius;
	//LONG LIVE TAU!
}

/**
 * Writes config header metadata into a gcode file
 * @param ss Stream to write config data to
 * @param sourceName - Name of source of this model. Usually the original .stl filename
 */
void GCoder::writeGCodeConfig(std::ostream &ss, const char* title="unknown source") const
{
	std::string indent = "* ";
	ss << endl;
	ss << "(Makerbot Industries)" << endl;
	ss << "(This file contains digital fabrication directives in gcode format)" 
			<< endl;
	ss << "(For your 3D printer)" << endl;
	ss << "(http://wiki.makerbot.com/gcode)" <<  endl;

	MyComputer hal9000;

	ss << "(" << indent << "Generated by "<<  
			gcoderCfg.programName << " " << 
			getMiracleGrueVersionStr() << ")"<< endl;
	ss << "(" << indent << hal9000.clock.now() <<  ")" << endl;
	ss << "(" << indent << "machine name: " << gcoderCfg.machineName << ")"<< endl;
	ss << "(" << indent << "firmware revision:" << gcoderCfg.firmware << ")" << endl;
	ss << "(" << indent << title << ")" << endl;

	std::string plurial = gcoderCfg.extruders.size()? "":"s";
	ss << "(" << indent << gcoderCfg.extruders.size() << " extruder" << plurial << ")" << endl;

	ss << "(" << indent << "Extrude infills: " << gcoderCfg.doInfills <<  ")" << endl;
	ss << "(" << indent;
	if(gcoderCfg.doInfillsFirst )
		ss << "first operation: Infill";
	else
		ss << "first operation: Insets";
	ss << ")" << endl;
	ss << "(" << indent << "Extrude insets: " << gcoderCfg.doInsets << ")" << endl;
	ss << "(" << indent << "Extrude outlines: " << gcoderCfg.doOutlines << ")" << endl;
	ss << endl;
}

}



