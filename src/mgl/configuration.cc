/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

 */


#include <iostream>
#include <sstream>
#include <fstream>


#include "configuration.h"
#include "gcoder.h"
#include "pather.h"
#include "abstractable.h"
#include "regioner.h"
#include "pather.h"
#include "loop_processor.h"

namespace mgl {

using namespace std;

string Configuration::defaultFilename() {
    MyComputer computer;
    string file = computer.fileSystem.getConfigFile("miracle.config");

    if (file.length() > 0)
        return file;
    else
        return string("miracle.config");
}

double doubleCheck(const Json::Value &value, const char *name) {
    if (value.isNull()) {
        stringstream ss;
        ss << "Missing required floating point field \"" <<
                name << "\" in configuration file";
        ConfigException mixup(ss.str().c_str());
        throw mixup;
    }
    return value.asDouble();
}

double doubleCheck(const Json::Value &value, const char *name,
        const double defaultVal) {
    if (value.isNull()) {
        return defaultVal;
    } else {
        return doubleCheck(value, name);
    }
}

unsigned int uintCheck(const Json::Value &value, const char *name) {
    if (value.isNull()) {
        stringstream ss;
        ss << "Missing required unsigned integer field \"" <<
                name << "\" in configuration file";
        ConfigException mixup(ss.str().c_str());
        throw mixup;
    }
    return value.asUInt();
}

unsigned int uintCheck(const Json::Value &value, const char *name,
        const unsigned int defaultVal) {
    if (value.isNull()) {
        return defaultVal;
    } else {
        return uintCheck(value, name);
    }
}

int intCheck(const Json::Value &value, const char *name) {
    if (value.isNull()) {
        stringstream ss;
        ss << "Missing required unsigned integer field \"" <<
                name << "\" in configuration file";
        ConfigException mixup(ss.str().c_str());
        throw mixup;
    }
    return value.asInt();
}

int intCheck(const Json::Value &value, const char *name,
        const int defaultVal) {
    if (value.isNull()) {
        return defaultVal;
    } else {
        return intCheck(value, name);
    }
}

string pathCheck(const Json::Value &value, const char *name,
        const std::string &defaultval) {
    if (value.isNull())
        return defaultval;
    else
        return pathCheck(value, name);
}

string pathCheck(const Json::Value &value, const char *name) {
    string result = stringCheck(value, name);
    if (result.substr(0, 10) == "default://") {
        MyComputer computer;
        result = computer.fileSystem.getDataFile(result.substr(10).c_str());
    }

    return result;
}

string stringCheck(const Json::Value &value, const char *name,
                 const string defaultval) {
    if (value.isNull())
        return defaultval;
    else
        return stringCheck(value, name);
}

string stringCheck(const Json::Value &value, const char *name) {
    if (value.isNull()) {
        stringstream ss;
        ss << "Missing required string field \"" <<
                name << "\" in configuration file";
        ConfigException mixup(ss.str().c_str());
        throw mixup;
    }
    return value.asString();
}

bool boolCheck(const Json::Value &value, const char *name,
        const bool defaultval) {
    if (value.isNull())
        return defaultval;
    else
        return boolCheck(value, name);
}

bool boolCheck(const Json::Value &value, const char *name) {
    if (value.isNull()) {
        stringstream ss;
        ss << "Missing required string field \"" <<
                name << "\" in configuration file";
        ConfigException mixup(ss.str().c_str());
        throw mixup;
    }
    return value.asBool();
}

Configuration::Configuration()
: filename("") {
}

void Configuration::readFromFile(const char* filename) {
    this->filename = filename;
    std::ifstream file(filename);
    if (file) {
        Json::Reader reader;
        reader.parse(file, root);
    } else {
        string msg = "Config file: \"";
        msg += filename;
        msg += "\" can't be found";
        ConfigException mixup(msg.c_str());
        throw mixup;
    }

}

Configuration::~Configuration() {
    // not sure we need to clean up here
    // this->root.clear();
}

std::string Configuration::asJson(Json::StyledWriter writer) const {
    return writer.write(root);
}

const Scalar GrueConfig::INVALID_SCALAR(std::numeric_limits<Scalar>::max());

GrueConfig::GrueConfig()
        : defaultExtruder(INVALID_UINT), doOutlines(INVALID_BOOL), 
        doInsets(INVALID_BOOL), doInfills(INVALID_BOOL), 
        doFanCommand(INVALID_BOOL), fanLayer(INVALID_UINT), 
        doAnchor(INVALID_BOOL), doPutModelOnPlatform(INVALID_BOOL), 
        doPrintLayerMessages(INVALID_BOOL), doPrintProgress(INVALID_BOOL), 
        minLayerDuration(INVALID_SCALAR), 
        coarseness(INVALID_SCALAR), preCoarseness(INVALID_SCALAR), 
        directionWeight(INVALID_SCALAR), 
        layerH(INVALID_SCALAR), firstLayerZ(INVALID_SCALAR), 
        infillDensity(INVALID_SCALAR), nbOfShells(INVALID_UINT), 
        layerWidthRatio(INVALID_SCALAR), layerWidthMinimum(INVALID_SCALAR), 
        layerWidthMaximum(INVALID_SCALAR), 
        insetDistanceMultiplier(INVALID_SCALAR), 
        infillShellSpacingMultiplier(INVALID_SCALAR), 
        roofLayerCount(INVALID_UINT), 
        floorLayerCount(INVALID_UINT), 
        //spur stuff begin
        doExternalSpurs(INVALID_BOOL),
        doInternalSpurs(INVALID_BOOL),
        minSpurWidth(INVALID_SCALAR), 
        maxSpurWidth(INVALID_SCALAR), 
        spurOverlap(INVALID_SCALAR), 
        minSpurLength(INVALID_SCALAR), 
        //spur stuff end
        doRaft(INVALID_BOOL), 
        raftLayers(INVALID_UINT), raftBaseThickness(INVALID_SCALAR), 
        raftInterfaceThickness(INVALID_SCALAR), raftOutset(INVALID_SCALAR), 
        raftModelSpacing(INVALID_SCALAR), raftDensity(INVALID_SCALAR), 
        doSupport(INVALID_BOOL), supportMargin(INVALID_SCALAR), 
        supportDensity(INVALID_SCALAR), doGraphOptimization(INVALID_BOOL), 
        rapidMoveFeedRateXY(INVALID_SCALAR), rapidMoveFeedRateZ(INVALID_SCALAR), 
        useEaxis(INVALID_BOOL), 
        /*
        // we don't need these for std::strings
        commentOpen(""), 
        commentClose(""),
        */
        weightedFanCommand(INVALID_INT),
        scalingFactor(INVALID_BOOL), 
        startingX(INVALID_SCALAR), startingY(INVALID_SCALAR), 
        startingZ(INVALID_SCALAR), startingA(INVALID_SCALAR), 
        startingB(INVALID_SCALAR), startingFeed(INVALID_SCALAR),
        centerX(INVALID_SCALAR), centerY(INVALID_SCALAR) {}
void GrueConfig::loadFromFile(const Configuration& config) {
    loadSlicingParams(config);
    doRaft = boolCheck(config["doRaft"], "doRaft");
    if(doRaft)
        loadRaftParams(config);
    doSupport = boolCheck(config["doSupport"], "doSupport");
    if(doSupport)
        loadSupportParams(config);
    doGraphOptimization = boolCheck(
            config["doGraphOptimization"], "doGraphOptimization", true);
    if(doGraphOptimization)
        loadPathingParams(config);
    loadGantryParams(config);
    loadGcodeParams(config);
    loadProfileParams(config);
}
void GrueConfig::loadSlicingParams(const Configuration& config) {
    coarseness = (doubleCheck(
            config["coarseness"], "coarseness"));
    preCoarseness = (doubleCheck(
            config["preCoarseness"], "preCoarseness"));
    directionWeight = doubleCheck(config["directionWeight"],
            "directionWeight", 0.5);
    layerH = (doubleCheck(
            config["layerHeight"], "layerHeight"));
    firstLayerZ = doubleCheck(config["bedZOffset"], 
            "bedZOffset");
    doPutModelOnPlatform = boolCheck(config["doPutModelOnPlatform"], 
            "doPutModelOnPlatform", true);
    infillDensity = doubleCheck(config["infillDensity"],
            "infillDensity");
    gridSpacingMultiplier = doubleCheck(config["gridSpacingMultiplier"],
            "gridSpacingMultiplier", 0.85);
    nbOfShells = uintCheck(config["numberOfShells"],
            "numberOfShells");
    layerWidthMinimum = doubleCheck(config["layerWidthMinimum"],
            "layerWidthMinimum");
    layerWidthMaximum = doubleCheck(config["layerWidthMaximum"],
            "layerWidthMaximum", 1.0);
    layerWidthRatio = doubleCheck(config["layerWidthRatio"],
            "layerWidthRatio");

    doExternalSpurs = boolCheck(config["doExternalSpurs"],
                                "doExternalSpurs");
    doInternalSpurs = boolCheck(config["doInternalSpurs"],
                                "doInternalSpurs");
    minSpurWidth = doubleCheck(config["minSpurWidth"],
            "minSpurWidth");
    maxSpurWidth = doubleCheck(config["maxSpurWidth"],
            "maxSpurWidth");
    spurOverlap = doubleCheck(config["spurOverlap"],
                              "spurOverlap", 0.01);
    minSpurLength = doubleCheck(config["minSpurLength"],
                                "minSpurLength");

    layerWidthRatio = std::min(std::max(layerWidthRatio * layerH, 
            layerWidthMinimum), layerWidthMaximum)/layerH;
    insetDistanceMultiplier =
            doubleCheck(config["insetDistanceMultiplier"],
            "insetDistanceMultiplier");
    infillShellSpacingMultiplier = 
            doubleCheck(config["infillShellSpacingMultiplier"], 
            "infillShellSpacingMultiplier");
    loadSolidLayerParams(config);
}
void GrueConfig::loadGantryParams(const Configuration& config) {
    rapidMoveFeedRateXY = (doubleCheck(
            config["rapidMoveFeedRateXY"], "rapidMoveFeedRateXY"));
    rapidMoveFeedRateZ = (doubleCheck(
            config["rapidMoveFeedRateZ"], "rapidMoveFeedRateZ"));
    scalingFactor = (doubleCheck(
            config["feedScalingFactor"], "feedScalingFactor", 60.0));

    startingX = (doubleCheck(
            config["startX"], "startX"));
    startingY = (doubleCheck(
            config["startY"], "startY"));
    startingZ = (doubleCheck(
            config["startZ"], "startZ"));
    startingA = 0;
    startingB = 0;
    startingFeed = 0;
    centerX = (doubleCheck(config["centerX"], "centerX", 0));
    centerY = (doubleCheck(config["centerX"], "centerX", 0));
}
void GrueConfig::loadGcodeParams(const Configuration& config) {
    defaultExtruder = uintCheck(config["defaultExtruder"],
            "defaultExtruder");
    header = pathCheck(config["startGcode"],
            "startGcode", "");
    footer = pathCheck(config["endGcode"],
            "endGcode", "");
    doOutlines = boolCheck(config["doOutlines"],
            "doOutlines", false);
    doInsets = boolCheck(config["doInsets"],
            "doInsets", true);
    doInfills = boolCheck(config["doInfills"],
            "doInfills", true);
    doFanCommand = boolCheck(config["doFanCommand"],
            "doFanCommand", false);
    if (doFanCommand) {
        fanLayer = uintCheck(config["fanLayer"],
                "fanLayer");
    }
    doAnchor = boolCheck(
            config["doAnchor"], "doAnchor", true);
    doPrintLayerMessages = boolCheck(
            config["printLayerMessages"],
            "printLayerMessages", false);
    doPrintProgress = boolCheck(
            config["doPrintProgress"],
            "doPrintProgress", false);
    minLayerDuration = doubleCheck(
            config["minLayerDuration"], 
            "minLayerDuration", 0);
    useEaxis = (boolCheck(config["useEAxis"],
            "useEAxis", false));
    commentOpen = (stringCheck(config["commentOpen"],
                               "commentOpen", "("));
    commentClose = (stringCheck(config["commentClose"],
                                "commentClose", ")"));
    weightedFanCommand = (intCheck(config["weightedFanCommand"],
                                   "weightedFanCommand", -1));
}
void GrueConfig::loadRaftParams(const Configuration& config) {
    raftLayers = uintCheck(config["raftLayers"],
                "raftLayers");
    raftBaseThickness = doubleCheck(
            config["raftBaseThickness"], "raftBaseThickness");
    raftInterfaceThickness = doubleCheck(
            config["raftInterfaceThickness"],
            "raftInterfaceThickness");
    raftOutset = doubleCheck(
            config["raftOutset"], "raftOutset");
    raftModelSpacing = doubleCheck(
            config["raftModelSpacing"], "raftModelSpacing");
    raftDensity = doubleCheck(
            config["raftDensity"], "raftDensity");
    raftAligned = boolCheck(
            config["raftAligned"], "raftAligned", false);
}
void GrueConfig::loadSupportParams(const Configuration& config) {
    supportMargin = doubleCheck(config["supportMargin"],
                "supportMargin");
    supportDensity = doubleCheck(
            config["supportDensity"], "supportDensity");
}
void GrueConfig::loadPathingParams(const Configuration& config) {
}
void GrueConfig::loadProfileParams(const Configuration& config) {
    loadExtruderParams(config);
    loadExtrusionParams(config);
}
void GrueConfig::loadExtruderParams(const Configuration& config) {
    size_t extruderCount = config["extruderProfiles"].size();
    const Json::Value& extruderBase = config["extruderProfiles"];
    for (size_t i = 0; i < extruderCount; i++) {
        const Json::Value &value = extruderBase[static_cast<int>(i)];

        stringstream ss;
        ss << "extruders[" << i << "].";
        string prefix = ss.str();

        Extruder extruder;
        // extruder.extrusionTemperature = 
        // 		doubleCheck(value["extrusionTemperature"], 
        // 		(prefix + "extrusionTemperature").c_str());
        extruder.nozzleDiameter = doubleCheck(value["nozzleDiameter"],
                (prefix + "nozzleDiameter").c_str());

        extruder.firstLayerExtrusionProfile =
                stringCheck(value["firstLayerExtrusionProfile"],
                (prefix + "firstLayerExtrusionProfile").c_str());
        extruder.insetsExtrusionProfile =
                stringCheck(value["insetsExtrusionProfile"],
                (prefix + "insetsExtrusionProfile").c_str());
        extruder.infillsExtrusionProfile =
                stringCheck(value["infillsExtrusionProfile"],
                (prefix + "infillsExtrusionProfile").c_str());
        extruder.outlinesExtrusionProfile =
                stringCheck(value["outlinesExtrusionProfile"],
                (prefix + "outlinesExtrusionProfile").c_str());

        extruder.id = i;
        extruder.code = 'A' + i;

        extruder.feedDiameter = doubleCheck(value["feedDiameter"],
                (prefix + "feedDiameter").c_str());
        extruder.nozzleDiameter = doubleCheck(value["nozzleDiameter"],
                (prefix + "nozzleDiameter").c_str());
        extruder.retractDistance = doubleCheck(value["retractDistance"],
                (prefix + "retractDistance").c_str());
        extruder.retractRate = doubleCheck(value["retractRate"],
                (prefix + "retractRate").c_str());
        extruder.restartExtraDistance =
                doubleCheck(value["restartExtraDistance"],
                (prefix + "restartExtraDistance").c_str());

        extruders.push_back(extruder);
    }
}
void GrueConfig::loadExtrusionParams(const Configuration& config) {
    const Json::Value &extrusionsRoot = config["extrusionProfiles"];

    for (Json::ValueIterator itr = extrusionsRoot.begin();
            itr != extrusionsRoot.end(); itr++) {
        string profileName = itr.key().asString();

        stringstream ss;
        ss << "extrusionProfiles[\"" << profileName << "\"].";
        string prefix = ss.str();
        const Json::Value &value = *itr;
        Extrusion extrusion;
        extrusion.feedrate = doubleCheck(value["feedrate"],
                (prefix + "feedrate").c_str());

        extrusion.temperature = doubleCheck(value["temperature"],
                (prefix + "temperature").c_str(), INVALID_SCALAR);

        extrusionProfiles.insert(pair<std::string,
                Extrusion > (profileName, extrusion));
    }
}
void GrueConfig::loadSolidLayerParams(const Configuration& config) {
    try {
        roofLayerCount =
                uintCheck(config["roofLayerCount"], "roofLayerCount");
    } catch (const ConfigException& ce) {
        Scalar roofThickness = doubleCheck(config["roofThickness"], 
                "roofThickness");
        roofLayerCount = static_cast<unsigned>(ceil(roofThickness / layerH));
    }
    try {
        floorLayerCount =
                uintCheck(config["floorLayerCount"], "floorLayerCount");
    } catch (const ConfigException& ce) {
        Scalar floorThickness = doubleCheck(config["floorThickness"], 
                "floorThickness");
        floorLayerCount = static_cast<unsigned>(ceil(floorThickness / layerH));
    }
}




}


