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
#include "configuration.h"
#include "mgl.h"
#include "pather.h"


#include "gcoder_gantry.h"
#include "log.h"

namespace mgl {

class GcoderException : public Exception {
public:
    template <typename T>
    GcoderException(const T& arg) : Exception(arg) {}
};

class GCoderConfig {
public:

    GCoderConfig() : programName(GRUE_PROGRAM_NAME),
    versionStr(GRUE_VERSION),
    startX(BAD_SCALAR),
    startY(BAD_SCALAR) {
    }

    static const Scalar BAD_SCALAR = 999999;
    static const unsigned int BAD_UINT = -1;

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
    bool doFanCommand;
    bool doPrintProgress;

    unsigned int fanLayer;
    
    
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

class GCoder : public Progressive {
public:

    const GrueConfig& grueCfg;
    Gantry gantry;
    unsigned int progressTotal;    //how many paths we will be doing
    unsigned int progressCurrent;  //which path the current one is
    unsigned int progressPercent;

    GCoder(const GrueConfig& grueConf, ProgressBar* progress = NULL);

    /// shortcut for doing a G1 that only move Z
    void moveZ(std::ostream & ss, Scalar z,
            unsigned int extruderId, Scalar zFeedrate);

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
    
    /**
     @brief Calculate a profile given all parameters, and indicate if this 
     path should be printed
     @param extruderId index into the extruder array
     @param layerSequence the number of the current layer
     @param label the label of the current path
     @param extrusionParams write the profile into this variable
     @return true if this path should be printed, false otherwise
     */
    bool calcExtrusion(unsigned int extruderId, 
            unsigned int layerSequence, 
            const PathLabel& label, 
            Extrusion& extrusionParams) const;


    /// Writes the start.gcode file, otherwise generates a
    /// start.gcode if needed
    void writeStartDotGCode(std::ostream & ss, const char* filename);

    /// Writes the end.gcode file, otherwise generates a
    /// end.gcode if needed
    void writeEndDotGCode(std::ostream & ss) const;
    
    void writeProgressPercent(std::ostream& ss, unsigned int current, 
            unsigned int total);


    // todo: return the gCoderCfg instead

    const GrueConfig::extruderVector & readExtruders() const {
        return grueCfg.get_extruders();
    }
    void writeSlice(std::ostream& ss,
            LayerPaths& layerpaths,
            LayerPaths::layer_iterator layerIter,
            size_t layerSequence);

private:

    void writeGCodeConfig(std::ostream & ss, const char* filename) const;
    template <typename PATH>
    void writePath(std::ostream& ss,
            Scalar z, Scalar h, Scalar w,
            const Extruder& extruder,
            const Extrusion& extrusion,
            const PATH& path, 
            Scalar feedScale = 1.0);
    template <template <class, class> class LABELEDPATHS, class ALLOC>
    void writePaths(std::ostream& ss,
            Scalar z, Scalar h, Scalar w,
            size_t layerSequence,
            const Extruder& extruder,
            const LABELEDPATHS<LabeledOpenPath, ALLOC>& labeledPaths, 
            Scalar feedScale = 1.0);
    /**
     @brief approximate how long it would take to extrude this path using 
     the extrusion profile given
     @param PATH the type of path
     @param extrusion the profile
     @param path the path
     @return Approximate time in seconds for this path
     */
    template <typename PATH>
    Scalar calcPath(const Extrusion& extrusion, 
            const PATH& path);
    /**
     @brief invoke calcPath on each element in @a labeledPaths and return 
     the sum of the results
     @param LABELEDPATHS the type of collection we're using (STL collections)
     @param ALLOC the type of allocator @a LABELEDPATHS uses
     @param layerSequence the number of the current layer
     @param extruder the current extruder to use
     @param labeledPaths the paths for which to calculate things
     */
    template <template <class, class> class LABELEDPATHS, class ALLOC>
    Scalar calcPaths(size_t layerSequence, 
            const Extruder& extruder, 
            const LABELEDPATHS<LabeledOpenPath, ALLOC>& labeledPaths);

    Point2Type startPoint(const SliceData &sliceData);
    // void writeWipeExtruder(std::ostream& ss, int extruderId) const {};
};

template <typename PATH>
void GCoder::writePath(std::ostream& ss,
        Scalar z, Scalar h, Scalar w,
        const Extruder& extruder,
        const Extrusion& extrusion,
        const PATH& path, 
        Scalar feedScale) {
    if (path.size() < 2) {
        GcoderException mixup("Attempted to write path with no points");
        throw mixup;
    }
    typename PATH::const_iterator current = path.fromStart();
    Point2Type last = *current;
    ++current;
    // rapid move into position
    Point2Type gantryPos(gantry.get_x(), gantry.get_y());
    if ((gantryPos - last).magnitude() >= grueCfg.get_coarseness()) {
        gantry.snort(ss, extruder, extrusion);
        gantry.g1(ss, extruder, extrusion,
                last.x, last.y, z,
                grueCfg.get_rapidMoveFeedRateXY() *
                grueCfg.get_scalingFactor(),
                0, 0,
                "move into position");
    }
    gantry.squirt(ss, extruder, extrusion);
    for (; current != path.end(); ++current) {
        Point2Type relative = (*current) - last;

        std::stringstream comment;
        Scalar distance = relative.magnitude();
        comment << "d: " << distance;
        gantry.g1(ss, extruder, extrusion,
                current->x, current->y, z,
                extrusion.feedrate * feedScale, h, w, comment.str().c_str());
        last = *current;
    }
}

template <template <class, class> class LABELEDPATHS, class ALLOC>
void GCoder::writePaths(std::ostream& ss,
        Scalar z, Scalar h, Scalar w,
        size_t layerSequence,
        const Extruder& extruder,
        const LABELEDPATHS<LabeledOpenPath, ALLOC>& labeledPaths, 
        Scalar feedScale) {
    typedef typename LABELEDPATHS<LabeledOpenPath, ALLOC>::const_iterator
    const_iterator;
    Extrusion fluidstrusion;
    PathLabel fluidLabel(PathLabel::TYP_CONNECTION, PathLabel::OWN_MODEL, 0);
    calcExtrusion(extruder.id, layerSequence, fluidLabel, fluidstrusion);
    gantry.snort(ss, extruder, fluidstrusion);
    bool didLastPath = true;
    for (const_iterator iter = labeledPaths.begin();
            iter != labeledPaths.end();
            ++iter) {
        const LabeledOpenPath& currentLP = *iter;
        writeProgressPercent(ss, progressCurrent+=currentLP.myPath.size(), 
                progressTotal);
        Extrusion extrusion;
        Scalar currentH = h;
        Scalar currentW = w;
        bool doCurrentPath = calcExtrusion(extruder.id, layerSequence, 
                currentLP.myLabel, extrusion);
        if(currentLP.myLabel.isConnection() && !didLastPath)
            continue;
        didLastPath = doCurrentPath;
        if(doCurrentPath) {
            writePath(ss, z, currentH, currentW, extruder, 
                    extrusion, currentLP.myPath, feedScale);
        }
    }
    gantry.snort(ss, extruder, fluidstrusion);
    ss << std::endl << std::endl;
}

template <typename PATH>
Scalar GCoder::calcPath(const Extrusion& extrusion, const PATH& path) {
    typedef typename PATH::const_iterator const_iterator;
    if(path.empty())
        return 0;
    const_iterator current = path.fromStart();
    Point2Type lastPoint = *current;
    Scalar accum = 0;
    for(++current; current != path.end(); ++current) {
        //distance in mm
        Scalar distance = Point2Type((*current) - lastPoint).magnitude();
        lastPoint = *current;
        //time in seconds
        Scalar time = (distance / extrusion.feedrate) * grueCfg.get_scalingFactor();
        accum += time;
    }
    return accum;
}

template <template <class, class> class LABELEDPATHS, class ALLOC>
Scalar GCoder::calcPaths(size_t layerSequence, const Extruder& extruder, 
        const LABELEDPATHS<LabeledOpenPath,ALLOC>& labeledPaths) {
    typedef typename LABELEDPATHS<LabeledOpenPath, ALLOC>::const_iterator 
            const_iterator;
    Scalar accum = 0;
    for(const_iterator iter = labeledPaths.begin(); 
            iter != labeledPaths.end(); 
            ++iter) {
        Extrusion extrusion;
        if(calcExtrusion(extruder.id, layerSequence, iter->myLabel, 
                extrusion)) {
            accum += calcPath(extrusion, iter->myPath);
        }
    }
    return accum;
}


}
#endif

