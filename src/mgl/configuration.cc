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
        doPrintLayerMessages(INVALID_BOOL), doPrintProgress(INVALID_BOOL), 
        layerH(INVALID_SCALAR), firstLayerZ(INVALID_SCALAR), 
        infillDensity(INVALID_SCALAR), nbOfShells(INVALID_UINT), 
        layerWidthRatio(INVALID_SCALAR), 
        insetDistanceMultiplier(INVALID_SCALAR), roofLayerCount(INVALID_UINT), 
        floorLayerCount(INVALID_UINT), doRaft(INVALID_BOOL), 
        raftLayers(INVALID_UINT), raftBaseThickness(INVALID_SCALAR), 
        raftInterfaceThickness(INVALID_SCALAR), raftOutset(INVALID_SCALAR), 
        raftModelSpacing(INVALID_SCALAR), raftDensity(INVALID_SCALAR), 
        doSupport(INVALID_BOOL), supportMargin(INVALID_SCALAR), 
        supportDensity(INVALID_SCALAR), doGraphOptimization(INVALID_BOOL), 
        coarseness(INVALID_SCALAR), directionWeight(INVALID_SCALAR), 
        rapidMoveFeedRateXY(INVALID_SCALAR), rapidMoveFeedRateZ(INVALID_SCALAR), 
        useEaxis(INVALID_BOOL), scalingFactor(INVALID_BOOL), 
        startingX(INVALID_SCALAR), startingY(INVALID_SCALAR), 
        startingZ(INVALID_SCALAR), startingA(INVALID_SCALAR), 
        startingB(INVALID_SCALAR), startingFeed(INVALID_SCALAR){}
void GrueConfig::loadFromFile(const Configuration& config) {
    loadSlicingParams(config);
    doRaft = boolCheck(config["doRaft"], "doRaft");
    if(doRaft)
        loadRaftParams(config);
    doSupport = boolCheck(config["doSupport"], "doSupport");
    if(doSupport)
        loadSupportParams(config);
    doGraphOptimization = boolCheck(
            config["doGraphOptimization"], "doGraphOptimization");
    if(doGraphOptimization)
        loadPathingParams(config);
    loadGantryParams(config);
    loadGcodeParams(config);
    loadProfileParams(config);
}
void GrueConfig::loadSlicingParams(const Configuration& config) {
    coarseness = (doubleCheck(
            config["coarseness"], "coarseness"));
    directionWeight = doubleCheck(config["directionWeight"],
            "directionWeight");
    layerH = (doubleCheck(
            config["layerHeight"], "layerHeight"));
    firstLayerZ = doubleCheck(config["bedZOffset"], 
            "bedZOffset");
    infillDensity = doubleCheck(config["infillDensity"],
            "infillDensity");
    gridSpacingMultiplier = doubleCheck(config["gridSpacingMultiplier"],
            "gridSpacingMultiplier", 0.92);
    nbOfShells = uintCheck(config["numberOfShells"],
            "numberOfShells");
    layerWidthRatio = doubleCheck(config["layerWidthRatio"],
            "layerWidthRatio");
    insetDistanceMultiplier =
            doubleCheck(config["insetDistanceMultiplier"],
            "insetDistanceMultiplier");
    roofLayerCount =
            uintCheck(config["roofLayerCount"], "roofLayerCount");
    floorLayerCount =
            uintCheck(config["floorLayerCount"], "floorLayerCount");
    
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
    doPrintLayerMessages = boolCheck(
            config["printLayerMessages"],
            "printLayerMessages", false);
    doPrintProgress = boolCheck(
            config["doPrintProgress"],
            "doPrintProgress", false);
    useEaxis = (boolCheck(config["useEAxis"],
            "useEAxis", false));
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
}
void GrueConfig::loadSupportParams(const Configuration& config) {
    supportMargin = doubleCheck(config["supportMargin"],
                "supportMargin");
    supportDensity = doubleCheck(
            config["supportDensity"], "supportDensity");
}
void GrueConfig::loadPathingParams(const Configuration& config) {
    iterativeEffort = uintCheck(config["iterativeEffort"], 
            "iterativeEffort");
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




}


