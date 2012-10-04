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

string stringCheck(const Json::Value &value, const char *name,
				   const string &defaultVal) {
	if (value.isNull())
		return defaultVal;
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

// this is a work in progress...

void loadExtrusionProfileData(const Configuration& conf, GCoderConfig &gcoderCfg) {
    const Json::Value &extrusionsRoot = conf.root["extrusionProfiles"];

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
                (prefix + "temperature").c_str(), -99999);

        gcoderCfg.extrusionProfiles.insert(pair<std::string,
                Extrusion > (profileName, extrusion));
    }
}

void loadExtruderConfigFromFile(const Configuration& conf,
        ExtruderConfig &extruderCfg) {
    extruderCfg.defaultExtruder =
            uintCheck(conf.root["defaultExtruder"],
            "defaultExtruder");
}

void loadGCoderConfigFromFile(const Configuration& conf,
        GCoderConfig &gcoderCfg) {
    gcoderCfg.gantryCfg.set_coarseness(doubleCheck(
            conf.root["coarseness"], "coarseness"));
    gcoderCfg.gantryCfg.set_rapid_move_feed_rate_xy(doubleCheck(
            conf.root["rapidMoveFeedRateXY"], "rapidMoveFeedRateXY"));
    gcoderCfg.gantryCfg.set_rapid_move_feed_rate_z(doubleCheck(
            conf.root["rapidMoveFeedRateZ"], "rapidMoveFeedRateZ"));
    gcoderCfg.gantryCfg.set_layer_h(doubleCheck(
            conf.root["layerHeight"], "layerHeight"));
    gcoderCfg.gantryCfg.set_scaling_factor(doubleCheck(
            conf.root["feedScalingFactor"], "feedScalingFactor", 60.0));

    gcoderCfg.gantryCfg.set_start_x(doubleCheck(
            conf.root["startX"], "startX"));
    gcoderCfg.gantryCfg.set_start_y(doubleCheck(
            conf.root["startY"], "startY"));
    gcoderCfg.gantryCfg.set_start_z(doubleCheck(
            conf.root["startZ"], "startZ"));

    loadExtrusionProfileData(conf, gcoderCfg);

    if (conf.root["extruderProfiles"].size() == 0) {
        stringstream ss;
        ss << "No extruder defined in the configuration file";
        ConfigException mixup(ss.str().c_str());
        throw mixup;
        return;
    }

    int x = conf.root["extruderProfiles"].size();
    unsigned int extruderCount = (unsigned int) x;
    for (unsigned int i = 0; i < extruderCount; i++) {
        const Json::Value &value = conf.root["extruderProfiles"][i];

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

        gcoderCfg.extruders.push_back(extruder);
    }

    gcoderCfg.header = pathCheck(conf.root["startGcode"],
            "startGcode", "");
    gcoderCfg.footer = pathCheck(conf.root["endGcode"],
            "endGcode", "");
    gcoderCfg.doOutlines = boolCheck(conf.root["doOutlines"],
            "doOutlines", false);
    gcoderCfg.doInsets = boolCheck(conf.root["insets"],
            "doInsets", true);
    gcoderCfg.doInfills = boolCheck(conf.root["doInfills"],
            "doInfills", true);
    gcoderCfg.doSupport = boolCheck(conf.root["doSupport"],
            "doSupport", false);
    gcoderCfg.doFanCommand = boolCheck(conf.root["doFanCommand"],
            "doFanCommand", false);
    if(gcoderCfg.doFanCommand) {
        gcoderCfg.fanLayer = uintCheck(conf.root["fanLayer"], 
                "fanLayer");
    }
    gcoderCfg.doPrintLayerMessages = boolCheck(
            conf.root["printLayerMessages"],
            "printLayerMessages", false);
    gcoderCfg.doPrintProgress = boolCheck(
            conf.root["doPrintProgress"],
            "doPrintProgress", false);
    gcoderCfg.gantryCfg.set_use_e_axis(boolCheck(conf.root["useEAxis"],
            "useEAxis", false));
    gcoderCfg.defaultExtruder = uintCheck(conf.root["defaultExtruder"],
            "defaultExtruder");

	gcoderCfg.commentOpen = stringCheck(conf.root["commentOpen"],
										"commentOpen", "(");
	gcoderCfg.commentClose = stringCheck(conf.root["commentClose"],
										"commentClose", ")");

	gcoderCfg.gantryCfg.commentOpen = stringCheck(conf.root["commentOpen"],
										"commentOpen", "(");
	gcoderCfg.gantryCfg.commentClose = stringCheck(conf.root["commentClose"],
										"commentClose", ")");
}

void loadSlicerConfigFromFile(const Configuration &config,
        SlicerConfig &slicerCfg) {
    //Relevant to slicer
    slicerCfg.layerH = doubleCheck(config["layerHeight"], "layerHeight");
    slicerCfg.firstLayerZ = doubleCheck(config["bedZOffset"], "bedZOffset");
}

void loadRegionerConfigFromFile(const Configuration& config,
        RegionerConfig& regionerCfg) {
    //Relevant to regioner
    regionerCfg.infillDensity = doubleCheck(config["infillDensity"],
            "infillDensity");
    regionerCfg.nbOfShells = uintCheck(config["numberOfShells"],
            "numberOfShells");
    regionerCfg.layerWidthRatio = doubleCheck(config["layerWidthRatio"],
            "layerWidthRatio");
    regionerCfg.insetDistanceMultiplier =
            doubleCheck(config["insetDistanceMultiplier"],
            "insetDistanceMultiplier");
    regionerCfg.roofLayerCount =
            doubleCheck(config["roofLayerCount"], "roofLayerCount");
    regionerCfg.floorLayerCount =
            doubleCheck(config["floorLayerCount"], "floorLayerCount");

    //Rafting Configuration
    regionerCfg.doRaft = boolCheck(config["doRaft"], "doRaft");
    if (regionerCfg.doRaft) {
        regionerCfg.raftLayers = uintCheck(config["raftLayers"],
                "raftLayers");

        regionerCfg.raftBaseThickness = doubleCheck(
                config["raftBaseThickness"], "raftBaseThickness");

        regionerCfg.raftInterfaceThickness = doubleCheck(
                config["raftInterfaceThickness"],
                "raftInterfaceThickness");

        regionerCfg.raftOutset = doubleCheck(
                config["raftOutset"], "raftOutset");

        regionerCfg.raftModelSpacing = doubleCheck(
                config["raftModelSpacing"], "raftModelSpacing");

        regionerCfg.raftModelSpacing = doubleCheck(
                config["supportDensity"], "supportDensity");
    }

    regionerCfg.doSupport = boolCheck(config["doSupport"], "doSupport");

    if (regionerCfg.doSupport) {
        regionerCfg.supportMargin = doubleCheck(config["supportMargin"],
                "supportMargin");

        regionerCfg.raftModelSpacing = doubleCheck(
                config["raftModelSpacing"], "raftModelSpacing");
    }

    if (regionerCfg.doRaft || regionerCfg.doSupport) {
        regionerCfg.supportDensity = doubleCheck(
                config["supportDensity"], "supportDensity");
    }
}

void loadPatherConfigFromFile(const Configuration& config,
        PatherConfig& patherCfg) {
    patherCfg.doGraphOptimization = boolCheck(
            config["doGraphOptimization"], "doGraphOptimization",
            patherCfg.doGraphOptimization);
    patherCfg.coarseness = doubleCheck(
            config["coarseness"],
            "coarseness",
            patherCfg.coarseness);
    patherCfg.directionWeight = doubleCheck(
            config["directionWeight"],
            "directionWeight",
            patherCfg.directionWeight);
}

void loadMeshConfigFromFile(const Configuration &config,
							MeshConfig &meshCfg) {
	meshCfg.centerX = doubleCheck(config["centerX"], "centerX", 0);
	meshCfg.centerY = doubleCheck(config["centerY"], "centerY", 0);
	meshCfg.lowerToBed = boolCheck(config["lowerToBed"], "lowerToBed", true);
}


}
