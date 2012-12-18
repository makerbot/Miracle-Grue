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
#include <map>
#include <vector>
#include <sstream>

namespace mgl {

using namespace std;



// function that adds an s to a noun if count is more than 1

std::string plural(const char*noun, int count, const char* ending = "s") {
    string s(noun);
    if (count > 1) {
        return s + ending;
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
        Point2Type &start, Point2Type &end) {
    size_t count = polygon.size();

    const Point2Type &a = polygon[0]; // first element
    const Point2Type &b = polygon[1];

    const Point2Type &c = polygon[count - 2];
    const Point2Type &d = polygon[count - 1]; // last element

    if (extruder.isVolumetric()) {
        start = a;
        end = d;
        return;
    }

    Point2Type ab = b - a;
    ab.normalise();
    Point2Type cd = d - c;
    cd.normalise();

    start.x = a.x - ab.x * leadIn;
    start.y = a.y - ab.y * leadIn;
    end.x = d.x + cd.x * leadOut;
    end.y = d.y + cd.y * leadOut;

}

GCoder::GCoder(const GrueConfig& grueConf, ProgressBar* progress)
        : Progressive(progress), grueCfg(grueConf), gantry(grueCfg), 
        progressTotal(0), progressCurrent(0), 
        progressPercent(0) {
    gantry.init_to_start();
}

/**
 * Writes intial gcode data to start of the gcode file, including setup & startup info
 * @param gout - output stream for the gcode text
 * @param sourceName - source of this gcode (usually the origional stl file)
 */
void GCoder::writeStartDotGCode(std::ostream &gout, const char* sourceName) {
    gout.precision(3);
    gout.setf(ios::fixed);

    writeGCodeConfig(gout, sourceName);

    const string &header_file = grueCfg.get_header();

    if (header_file.length() > 0) {
        ifstream header_in(header_file.c_str(), ifstream::in);

        if (header_in.fail())
            throw GcoderException((string("Unable to open gcode header file [") +
                header_file + "]").c_str());

        gout << grueCfg.get_commentOpen()
             << "header [" << header_file << "] begin"
             << grueCfg.get_commentClose() << endl;

        while (header_in.good()) {
            char buf[1024];

            header_in.read(buf, sizeof (buf));
            gout.write(buf, header_in.gcount());
        }

        if (header_in.fail() && !header_in.eof())
            throw GcoderException((string("Error reading gcode header file [") +
                header_file + "]").c_str());

        gout << grueCfg.get_commentOpen()
             << "header [" << header_file << "] end"
             << grueCfg.get_commentClose() <<endl << endl;
    }
}

void GCoder::writeEndDotGCode(std::ostream &ss) const {
    const string &footer_file = grueCfg.get_footer();


    if (footer_file.length() > 0) {
        ifstream footer_in(footer_file.c_str(), ifstream::in);

        if (footer_in.fail())
            throw GcoderException((string("Unable to open footer file [") +
                footer_file + "]").c_str());

        ss << grueCfg.get_commentOpen()
           << "footer [" << footer_file << "] begin"
           << grueCfg.get_commentClose() << endl;

        while (footer_in.good()) {
            char buf[1024];

            footer_in.read(buf, sizeof (buf));
            ss.write(buf, footer_in.gcount());
        }

        if (footer_in.fail() && !footer_in.eof())
            throw GcoderException((string("Error reading footer file [") +
                footer_file + "]").c_str());

        ss << grueCfg.get_commentOpen()
           << "footer [" << footer_file << "] end"
           << grueCfg.get_commentClose() << endl << endl;
    }
}

void GCoder::writeProgressPercent(std::ostream& ss, unsigned int current, 
        unsigned int total) {
    if(!grueCfg.get_doPrintProgress())
        return;
    unsigned int curPercent = (current--*100)/total;
    if(curPercent != progressPercent) {
        ss << "M73 P" << curPercent << " " << grueCfg.get_commentOpen()
           << "progress (" << curPercent << "%): " << current 
                << "/" << total << 
            grueCfg.get_commentClose() << std::endl;
        progressPercent = curPercent;
    }
}

void GCoder::moveZ(ostream & ss, Scalar z, unsigned int, Scalar zFeedrate) {
    bool doX = false;
    bool doY = false;
    bool doZ = true;
    bool doE = false;
    bool doFeed = true;


    gantry.g1Motion(ss, 0, 0, z, 0, zFeedrate, 0, 0,
            "move Z", doX, doY, doZ, doE, doFeed);

}
bool GCoder::calcExtrusion(unsigned int extruderId, 
        unsigned int layerSequence, const PathLabel& label, 
        Extrusion& extrusionParams) const {
    const Extruder& currentExtruder = grueCfg.get_extruders()[extruderId];
    std::string profileName;
    if(layerSequence == 0) {
        profileName = currentExtruder.firstLayerExtrusionProfile;
    } else {
        if(label.isOutline() || (label.isInset() && 
                label.myValue == 
                LayerPaths::Layer::ExtruderLayer::OUTLINE_LABEL_VALUE)) {
            profileName = currentExtruder.outlinesExtrusionProfile;
        } else if(label.isInset()) {
            profileName = currentExtruder.insetsExtrusionProfile;
        } else if(label.isInfill() || label.isConnection() || 
                label.isSupport()) {
            profileName = currentExtruder.infillsExtrusionProfile;
        } else {
            std::stringstream errorMsg;
            errorMsg << "Invalid label for extruder " << extruderId << 
                    " at layer " << layerSequence;
            throw GcoderException(errorMsg.str());
        }
    }
    GrueConfig::profileNameMap::const_iterator profileIter = 
            grueCfg.get_extrusionProfiles().find(profileName);
    if(profileIter == grueCfg.get_extrusionProfiles().end()) {
        std::stringstream errorMsg;
        errorMsg << "Cannot find profile " << profileName << 
                " for extruder " << extruderId << 
                " at layer " << layerSequence;
        throw GcoderException(errorMsg.str());
    }
    extrusionParams = profileIter->second;
    extrusionParams.feedrate *= grueCfg.get_scalingFactor();
    
    return ((label.isOutline() && grueCfg.get_doOutlines()) || 
            (label.isInset() && grueCfg.get_doInsets()) || 
            (label.isInfill() && grueCfg.get_doInfills()) || 
            (label.isSupport() && grueCfg.get_doSupport()) || 
            (label.isConnection()));
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
        const LayerMeasure&, // layerMeasure, 
        std::ostream& gout,
        const std::string& title,
        LayerPaths::layer_iterator begin,
        LayerPaths::layer_iterator end) {
    writeStartDotGCode(gout, title.c_str());
    size_t sliceCount = 0;
    progressTotal = 1;
    progressCurrent = 0;
    progressPercent = 0;
    for (LayerPaths::const_layer_iterator it = begin;
            it != end;
            ++it, ++sliceCount){
        for(LayerPaths::Layer::const_extruder_iterator exit = 
                it->extruders.begin(); 
                exit != it->extruders.end(); 
                ++exit) {
            for(LayerPaths::Layer::ExtruderLayer::const_path_iterator pathiter = 
                    exit->paths.begin(); 
                    pathiter != exit->paths.end(); 
                    ++pathiter) {
                progressTotal += pathiter->myPath.size();
            }
        }
    }
    initProgress("gcode", sliceCount);
    size_t layerSequence = 0;
    bool wasAnchorDone = !grueCfg.get_doAnchor();
    bool wasRaftDone = !grueCfg.get_doRaft();
    for (LayerPaths::layer_iterator it = begin;
            it != end; ++it, ++layerSequence) {
        tick();
        //Scalar z = layerMeasure.sliceIndexToHeight(codeSlice);
        if(!wasAnchorDone && layerSequence == 0) {
            wasAnchorDone = true;
            Extrusion strusion;
            PathLabel slabel(PathLabel::TYP_CONNECTION, PathLabel::OWN_MODEL, 0);
            const Extruder& struder = grueCfg.get_extruders()[
                    it->extruders.front().extruderId];
            calcExtrusion(struder.id, 0, slabel, strusion);
            gantry.set_current_extruder_index(struder.code);
            Point2Type startPoint;
            if(!it->extruders.empty() && 
                    !it->extruders.front().paths.empty() && 
                    !it->extruders.front().paths.front().myPath.empty()) {
                startPoint = *(it->extruders.front().paths.front().myPath.fromStart());
            }
            gantry.snort(gout, struder, 
                    strusion);
            const Scalar currentZ = it->layerZ + it->layerHeight;
            const Scalar currentH = it->layerHeight;
            const Scalar currentW = it->layerW * 2.0;
            gantry.g1(gout, struder, 
                    strusion, grueCfg.get_startingX(), 
                    grueCfg.get_startingY(), currentZ, 
                    strusion.feedrate, 
                    currentH, currentW, "Anchor Start");
            gantry.squirt(gout, struder, 
                    strusion);
            gantry.g1(gout, struder, 
                    strusion, grueCfg.get_startingX(), 
                    grueCfg.get_startingY(), currentZ, 
                    strusion.feedrate, 
                    currentH, currentW, "Anchor Start");
            gantry.g1(gout, struder, 
                    strusion, startPoint.x, startPoint.y, currentZ, 
                    strusion.feedrate, 
                    currentH, currentW, "Anchor End");
        }
        if(!wasRaftDone && layerSequence == grueCfg.get_raftLayers()) {
            wasRaftDone = true;
            layerSequence = 0;
        }
        writeSlice(gout, layerpaths, it, layerSequence);
    }
    if(grueCfg.get_doFanCommand()) {
        //print command to disable fan
        if (grueCfg.get_weightedFanCommand() != -1)
            gout << "M106 S0";
        else 
            gout << "M127 T" << grueCfg.get_defaultExtruder();
        
        gout << " " << grueCfg.get_commentOpen()
             << "Turn off the fan"
             << grueCfg.get_commentClose() << endl;
    }
    writeEndDotGCode(gout);
}

Point2Type GCoder::startPoint(const SliceData& sliceData) {
    if (grueCfg.get_doOutlines()) {
        return sliceData.extruderSlices[0].boundary[0][0];
    } else if (grueCfg.get_doInsets()) {
        if (sliceData.extruderSlices.size() < 1)
            throw Exception("zero extruder slices for finding start point");

        if (sliceData.extruderSlices[0].insetLoopsList.size() < 1)
            throw Exception("zero inset loops for finding start point");

        if (sliceData.extruderSlices[0].insetLoopsList[0].size() < 1)
            throw Exception("zero loops for finding start point");

        return sliceData.extruderSlices[0].insetLoopsList[0][0][0];
    } else {
        return sliceData.extruderSlices[0].infills[0][0];
    }
}

void GCoder::writeSlice(std::ostream& ss,
        LayerPaths& layerpaths,
        LayerPaths::layer_iterator layerIter,
        size_t layerSequence) {
    LayerPaths::Layer& currentLayer = *layerIter;
    unsigned int extruderCount = currentLayer.extruders.size();

    ss << grueCfg.get_commentOpen()
       << "Slice " << layerSequence << ", " << extruderCount
       << " " << plural("Extruder", extruderCount)
       << grueCfg.get_commentClose() << endl;

    ss << grueCfg.get_commentOpen()
       << "Layer Height: \t" << layerIter->layerHeight
       << grueCfg.get_commentClose() << endl;

    ss << grueCfg.get_commentOpen()
       << "Layer Width: \t" << layerIter->layerW
       << grueCfg.get_commentClose() << endl;

    if (grueCfg.get_doPrintLayerMessages()) {
        //print layer message to printer screen if config enabled
        ss << "M70 P20 " << grueCfg.get_commentOpen()
           << "Layer: " << layerSequence 
           << grueCfg.get_commentClose() << endl;
    }
    if (grueCfg.get_doFanCommand()&& layerSequence == grueCfg.get_fanLayer()) {
        //print command to enable fan
        if (grueCfg.get_weightedFanCommand() != -1)
            ss << "M106 S" << grueCfg.get_weightedFanCommand();
        else 
            ss << "M126 T" << grueCfg.get_defaultExtruder();
        
        ss << " " << grueCfg.get_commentOpen()
           << "Turn on the fan"
           << grueCfg.get_commentClose() << endl;
    }
    //iterate over all extruders invoked in this layer
    for (LayerPaths::Layer::const_extruder_iterator it =
            currentLayer.extruders.begin();
            it != currentLayer.extruders.end();
            ++it) {
        //this is the current extruder
        const Extruder& currentExtruder = grueCfg.get_extruders()[it->extruderId];
        gantry.set_current_extruder_index(currentExtruder.code);
        //this is the current extruder's zFeedrate
        Scalar zFeedrate = grueCfg.get_scalingFactor() *
                grueCfg.get_rapidMoveFeedRateZ();
        const Scalar currentZ = currentLayer.layerZ + currentLayer.layerHeight;
        const Scalar currentH = currentLayer.layerHeight;
        const Scalar currentW = currentLayer.layerW;
        try {
            moveZ(ss, currentZ, currentExtruder.id, zFeedrate);
        } catch (GcoderException& mixup) {
            Log::info() << "ERROR writing Z move in slice " <<
                    layerSequence << " for extruder " << currentExtruder.id <<
                    " : " << mixup.error << endl;
        }
        Scalar feedScale = 1.0;
        bool calculateSlowing = !grueCfg.get_doRaft() || 
                layerSequence >= grueCfg.get_raftLayers();
        if(calculateSlowing) {
            Scalar duration = calcPaths(layerSequence, currentExtruder, it->paths);
            if(duration < grueCfg.get_minLayerDuration()) {
                feedScale = duration / grueCfg.get_minLayerDuration();
                int speedDecrease(feedScale * 100);
                ss << grueCfg.get_commentOpen()
                   << "Slowing to " << speedDecrease << "% of nominal speeds" 
                   << grueCfg.get_commentClose() << std::endl;
            }
        }
        writePaths(ss, currentZ, currentH, currentW, layerSequence,
                currentExtruder, it->paths, feedScale);
    }
}

Scalar Extrusion::crossSectionArea(Scalar height, Scalar width) const {


    //two semicircles joined by a rectangle
    Scalar radius = height / 2;
    return (M_TAU / 2) * (radius * radius) + height * (width - height);
    //LONG LIVE TAU!
}

Scalar Extruder::feedCrossSectionArea() const {
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
void GCoder::writeGCodeConfig(std::ostream &ss, const char* title = "unknown source") const {
    std::string indent = "* ";
    ss << endl;

    ss << grueCfg.get_commentOpen()
       << "Makerbot Industries"
       << grueCfg.get_commentClose() << endl;

    ss << grueCfg.get_commentOpen()
       << "This file contains digital fabrication directives in gcode format"
       << grueCfg.get_commentClose() << endl;

    ss << grueCfg.get_commentOpen()
       << "For your 3D printer"
       << grueCfg.get_commentClose() << endl;

    ss << grueCfg.get_commentOpen()
       << "http://wiki.makerbot.com/gcode"
       << grueCfg.get_commentClose() << endl;

    MyComputer hal9000;

    ss << grueCfg.get_commentOpen()
       << indent << "Generated by " << getMiracleGrueProgramName()
       << " " << getMiracleGrueVersionStr()
       << grueCfg.get_commentClose() << endl;

    ss << grueCfg.get_commentOpen()
       << indent << hal9000.clock.now()
       << grueCfg.get_commentClose() << endl;

    ss << grueCfg.get_commentOpen()
       << indent << title
       << grueCfg.get_commentClose() << endl;

    std::string plurial = grueCfg.get_extruders().size() ? "" : "s";

    ss << grueCfg.get_commentOpen()
       << indent << grueCfg.get_extruders().size() << " extruder" << plurial
       << grueCfg.get_commentClose() << endl;

    ss << grueCfg.get_commentOpen()
       << indent << "Extrude infills: " << grueCfg.get_doInfills()
       << grueCfg.get_commentClose() << endl;

    ss << grueCfg.get_commentOpen()
       << indent << "Extrude insets: " << grueCfg.get_doInsets()
       << grueCfg.get_commentClose() << endl;

    ss << grueCfg.get_commentOpen()
       << indent << "Extrude outlines: " << grueCfg.get_doOutlines()
       << grueCfg.get_commentClose() << endl;
    ss << endl;
}

}



