/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

 */

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#define GRUE_VERSION  "v0.0.4.0"
#define GRUE_PROGRAM_NAME  "Miracle-Grue"

#include <sstream>
#include <vector>
#include <string>

#include <json/reader.h>
#include <json/writer.h>
#include <json/value.h>
#include <json/writer.h>

#include "mgl.h"

#include <limits>

namespace mgl {

class ConfigException : public Exception {
public:

    ConfigException(const char *msg) : Exception(msg) {
    }
};

// checks that the value exist before returning it
double doubleCheck(const Json::Value &value, const char *name);
double doubleCheck(const Json::Value &value, const char *name,
        const double defaultVal);
unsigned int uintCheck(const Json::Value &value, const char *name);
unsigned int uintCheck(const Json::Value &value, const char *name,
        const unsigned int defaultVal);
std::string stringCheck(const Json::Value &value, const char *name);
std::string pathCheck(const Json::Value &value, const char *name,
        const std::string &defaultval);
std::string pathCheck(const Json::Value &value, const char *name);
bool boolCheck(const Json::Value &value, const char *name, bool defaultval);
bool boolCheck(const Json::Value &value, const char *name);
//
// This class contains settings for the 3D printer, and user preferences
//

class Configuration {
    std::string filename;
public:
    Json::Value root;

public:
    //	Json::Value &readJsonValue() {return root;}
public:
    /// standard constructor
    Configuration();
    /// standard constructor for loading configuration from a json file
    //    Configuration(std::string& srcFilename);
    /// standard  destructor

    ~Configuration();

    void readFromFile(const char* filename);

    void readFromFile(const std::string &filename) {
        readFromFile(filename.c_str());
    };

    void readFromDefault() {
        readFromFile(defaultFilename());
    };


public:

    bool isMember(const char* key) {
        return this->root.isMember(key);
    }

    /// index function, to read/write values as config["foo"]

    Json::Value& operator[] (const char* key) {

        //     		if(filename.length() ==0)
        //     		{
        //     			ConfigException mixup("Configuration file has not been read");
        //     		    throw mixup;
        //     		}

        //     		if( !root.isMember(key) )
        //     		{
        //     			std::stringstream ss;
        //     			ss << "Can't find \"" << key << "\" in " << filename;
        //     			ConfigException mixup(ss.str().c_str());
        //     			throw mixup;
        //     		}
        return this->root[key];
    }

    const Json::Value& operator[] (const char* key) const {
        return this->root[key];
    }

    std::string asJson(Json::StyledWriter writer = Json::StyledWriter()) const;

private:
    std::string defaultFilename();
};

class Extruder;
class Extrusion;

////master configuration object
class GrueConfig {
public:
    GrueConfig();
    void loadFromFile(const Configuration& config);
    Scalar segmentVolume(const Extruder& extruder, const Extrusion& extrusion, 
            const Segment2Type& segment, Scalar h, Scalar w) const;
#define GRUECONFIG_PUBLIC_CONST_ACCESSOR(TYPE, NAME) \
    protected: \
    TYPE NAME; \
    public: \
    TYPE get_##NAME() const { return NAME; } \
    TYPE get_##NAME() { return NAME; }

#define GRUECONFIG_PUBLIC_CONSTREF_ACCESSOR(TYPE, NAME) \
    protected: \
    TYPE NAME; \
    public: \
    const TYPE& get_##NAME() const { return NAME; } \
    TYPE& get_##NAME() { return NAME; }

    
    typedef std::map<std::string, Extrusion> profileNameMap;
    typedef std::vector<Extruder> extruderVector;
private:
    static const Scalar INVALID_SCALAR;// = std::numeric_limits<Scalar>::min();
    static const unsigned int INVALID_UINT = -1;
    static const unsigned int INVALID_INT = -1;
    static const bool INVALID_BOOL = false;
    
    void loadRaftParams(const Configuration& config);
    void loadSupportParams(const Configuration& config);
    void loadPathingParams(const Configuration& config);
    void loadGantryParams(const Configuration& config);
    void loadProfileParams(const Configuration& config);
    void loadGcodeParams(const Configuration& config);
    void loadSlicingParams(const Configuration& config);
    
    /* This is called from loadProfileParams */
    void loadExtruderParams(const Configuration& config);
    void loadExtrusionParams(const Configuration& config);
    /* This is called from loadSlicingParams */
    void loadSolidLayerParams(const Configuration& config);
    /* --END-- */
    
    //gcoder stuff
    
    GRUECONFIG_PUBLIC_CONSTREF_ACCESSOR(profileNameMap, extrusionProfiles)
    GRUECONFIG_PUBLIC_CONSTREF_ACCESSOR(extruderVector, extruders)
    
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(unsigned, defaultExtruder)
    GRUECONFIG_PUBLIC_CONSTREF_ACCESSOR(std::string, header)
    GRUECONFIG_PUBLIC_CONSTREF_ACCESSOR(std::string, footer)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(bool, doOutlines)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(bool, doInsets)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(bool, doInfills)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(bool, doFanCommand)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(unsigned, fanLayer)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(bool, doAnchor)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(bool, doPutModelOnPlatform)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(bool, doPrintLayerMessages)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(bool, doPrintProgress)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(Scalar, minLayerDuration)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(Scalar, minSpeedMultiplier)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(Scalar, coarseness)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(Scalar, preCoarseness)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(Scalar, directionWeight)
    //slicer
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(Scalar, layerH)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(Scalar, firstLayerZ)
    //regioner
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(Scalar, infillDensity)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(Scalar, gridSpacingMultiplier)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(unsigned, nbOfShells)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(Scalar, layerWidthRatio)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(Scalar, layerWidthMinimum)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(Scalar, layerWidthMaximum)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(Scalar, insetDistanceMultiplier)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(Scalar, infillShellSpacingMultiplier)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(unsigned, roofLayerCount)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(unsigned, floorLayerCount)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(bool, doExternalSpurs)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(bool, doInternalSpurs)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(Scalar, minSpurWidth)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(Scalar, maxSpurWidth)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(Scalar, spurOverlap)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(Scalar, minSpurLength)
    //raft
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(bool, doRaft)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(unsigned, raftLayers)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(Scalar, raftBaseThickness)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(Scalar, raftInterfaceThickness)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(Scalar, raftOutset)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(Scalar, raftModelSpacing)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(Scalar, raftDensity)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(bool, raftAligned)
    //support
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(bool, doSupport)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(Scalar, supportMargin)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(Scalar, supportDensity)
    //pather
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(bool, doGraphOptimization)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(bool, doFixedLayerStart);
    //gantry
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(Scalar, rapidMoveFeedRateXY)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(Scalar, rapidMoveFeedRateZ)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(bool, useEaxis)
    GRUECONFIG_PUBLIC_CONSTREF_ACCESSOR(std::string, commentOpen)
    GRUECONFIG_PUBLIC_CONSTREF_ACCESSOR(std::string, commentClose)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(int, weightedFanCommand)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(Scalar, scalingFactor)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(Scalar, startingX)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(Scalar, startingY)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(Scalar, startingZ)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(Scalar, startingA)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(Scalar, startingB)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(Scalar, startingFeed)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(Scalar, centerX)
    GRUECONFIG_PUBLIC_CONST_ACCESSOR(Scalar, centerY)
    
#undef GRUECONFIG_PUBLIC_CONST_ACCESSOR
#undef GRUECONFIG_PUBLIC_CONSTREF_ACCESSOR
};
/// Properties common to a single hardware extruder
class Extruder {
public:

    Extruder() {}

    Scalar feedCrossSectionArea() const;

    bool isVolumetric() const {
        return true;
    };

    Scalar feedDiameter;
    Scalar nozzleDiameter;
    Scalar feedstockMultiplier;
    unsigned char code;
    int id;

    Scalar retractDistance;
    Scalar retractRate;
    Scalar restartExtraDistance;

    std::string firstLayerExtrusionProfile;
    std::string insetsExtrusionProfile;
    std::string infillsExtrusionProfile;
    std::string outlinesExtrusionProfile;
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
}
#endif /* CONFIGURATION_H_ */
