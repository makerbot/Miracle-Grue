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


using namespace mgl;
using namespace std;

double mgl::doubleCheck(const Json::Value &value, const char *name)
{
	if(value.isNull())
	{
		stringstream ss;
		ss << "Missing required floating point field \""<< name << "\" in configuration file";
		ConfigException mixup(ss.str().c_str());
		throw mixup;
	}
	return value.asDouble();
}

unsigned int mgl::uintCheck(const Json::Value &value, const char *name)
{
	if(value.isNull())
	{
		stringstream ss;
		ss << "Missing required unsigned integer field \""<< name << "\" in configuration file";
		ConfigException mixup(ss.str().c_str());
		throw mixup;
	}
	return value.asUInt();
}

string mgl::stringCheck(const Json::Value &value, const char *name)
{
	if(value.isNull())
	{
		stringstream ss;
		ss << "Missing required string field \""<< name << "\" in configuration file";
		ConfigException mixup(ss.str().c_str());
		throw mixup;
	}
	return value.asString();
}

bool mgl::boolCheck(const Json::Value &value, const char *name)
{
	if(value.isNull())
	{
		stringstream ss;
		ss << "Missing required string field \""<< name << "\" in configuration file";
		ConfigException mixup(ss.str().c_str());
		throw mixup;
	}
	return value.asBool();
}
Configuration::Configuration()
	:filename("")
{
}

void Configuration::readFromFile(const char* filename)
{
	this->filename = filename;
    std::ifstream file(filename);
    if (file) {
        Json::Reader reader;
        reader.parse(file , root);
    }
    else
    {
        string msg = "Config file: \"";
        msg += filename;
        msg += "\" can't be found";
        ConfigException mixup(msg.c_str());
        throw mixup;
    }

}

Configuration::~Configuration()
{
	// not sure we need to clean up here
	// this->root.clear();
}

std::string Configuration::asJson(Json::StyledWriter writer ) const
{
	return writer.write(root);
}

// this is a work in progress...
void loadExtrusionProfileData(const Configuration& conf, GCoderConfig &gcoderCfg)
{
	const Json::Value &extrusionsRoot = conf.root["extrusionProfiles"];
	for( Json::ValueIterator itr = extrusionsRoot.begin() ; itr != extrusionsRoot.end() ; itr++ )
	{
		string profileName = itr.key().asString();
		stringstream ss;
		ss << "extrusionProfiles[\"" << profileName << "\"].";
		string prefix = ss.str();
		const Json::Value &value = *itr;
		Extrusion extrusion;
		extrusion.feedrate 	= doubleCheck(value["feedrate"], (prefix + "feedrate").c_str());
		extrusion.flow 		= doubleCheck(value["flow"], (prefix + "flow").c_str());
		extrusion.leadIn 	= doubleCheck(value["leadIn"], (prefix + "leadIn").c_str());
		extrusion.leadOut	= doubleCheck(value["leadOut"], (prefix + "leadOut").c_str());
		extrusion.snortFeedrate = doubleCheck(value["snortFeedrate"], (prefix + "snortFeedrate").c_str());
		extrusion.snortFlow 	= doubleCheck(value["snortFlow"], (prefix + "snortFlow").c_str());
		extrusion.squirtFeedrate= doubleCheck(value["squirtFeedrate"], (prefix + "squirtFeedrate").c_str());
		extrusion.squirtFlow 	= doubleCheck(value["squirtFlow"], (prefix + "squirtFlow").c_str());

		gcoderCfg.extrusionProfiles.insert( pair<std::string, Extrusion>(profileName, extrusion));
	}
}


void mgl::loadGCoderConfigFromFile(const Configuration& conf, GCoderConfig &gcoderCfg)
{

	gcoderCfg.programName = stringCheck(conf.root["programName"],"programName");
	gcoderCfg.versionStr  = stringCheck(conf.root["versionStr"],"versionStr");
	gcoderCfg.machineName = stringCheck(conf.root["machineName"],"machineName");
	gcoderCfg.firmware    = stringCheck(conf.root["firmware"], "firmware");

	gcoderCfg.gantry.xyMaxHoming = boolCheck(conf.root["gantry"]["xyMaxHoming"], "gantry.xyMaxHoming");
	gcoderCfg.gantry.zMaxHoming  = boolCheck(conf.root["gantry"]["zMaxHoming" ], "gantry.zMaxHoming");
	gcoderCfg.gantry.scalingFactor = doubleCheck(conf.root["gantry"]["scalingFactor"], "gantry.scalingFactor");
	gcoderCfg.gantry.rapidMoveFeedRateXY = doubleCheck(conf.root["gantry"]["rapidMoveFeedRateXY"], "gantry.rapidMoveFeedRateXY");
	gcoderCfg.gantry.rapidMoveFeedRateZ = doubleCheck(conf.root["gantry"]["rapidMoveFeedRateZ"], "gantry.rapidMoveFeedRateZ");
	gcoderCfg.gantry.homingFeedRateZ = doubleCheck(conf.root["gantry"]["homingFeedRateZ"], "gantry.homingFeedRateZ");

	gcoderCfg.platform.temperature = doubleCheck(conf.root["platform"]["temperature"], "platform.temperature");
	gcoderCfg.platform.automated   = boolCheck(conf.root["platform"]["automated"], "platform.automated");
	gcoderCfg.platform.waitingPositionX = doubleCheck(conf.root["platform"]["waitingPositionX"], "platform.waitingPositionX");
	gcoderCfg.platform.waitingPositionY = doubleCheck(conf.root["platform"]["waitingPositionY"], "platform.waitingPositionY");
	gcoderCfg.platform.waitingPositionZ = doubleCheck(conf.root["platform"]["waitingPositionZ"], "platform.waitingPositionZ");

	gcoderCfg.outline.enabled  = boolCheck(conf.root["outline"]["enabled"], "outline.enabled");
	gcoderCfg.outline.distance = doubleCheck(conf.root["outline"]["distance"], "outline.distance");

	loadExtrusionProfileData(conf, gcoderCfg);

	if(conf.root["extruders"].size() ==0)
	{
		stringstream ss;
		ss << "No extruder defined in the configuration file";
		ConfigException mixup(ss.str().c_str());
		throw mixup;
        return;
	}

	int x = conf.root["extruders"].size();
	unsigned int extruderCount = (unsigned int)x;
	for(unsigned int i=0; i < extruderCount; i++)
	{
		const Json::Value &value = conf.root["extruders"][i];

		stringstream ss;
		ss << "extruders[" << i << "].";
		string prefix = ss.str();

		Extruder extruder;
		extruder.coordinateSystemOffsetX = doubleCheck(value["coordinateSystemOffsetX"], (prefix+"coordinateSystemOffsetX").c_str() );
		extruder.extrusionTemperature = doubleCheck(value["extrusionTemperature"], (prefix+"extrusionTemperature").c_str() );
		extruder.nozzleZ = doubleCheck(value["nozzleZ"], (prefix+"nozzleZ").c_str() );
		extruder.zFeedRate = doubleCheck(value["zFeedRate"], (prefix+"zFeedRate").c_str() );

		extruder.firstLayerExtrusionProfile = stringCheck(value["firstLayerExtrusionProfile"], (prefix+"firstLayerExtrusionProfile").c_str() );
		extruder.insetsExtrusionProfile = stringCheck(value["insetsExtrusionProfile"], (prefix+"insetsExtrusionProfile").c_str() );
		extruder.infillsExtrusionProfile = stringCheck(value["infillsExtrusionProfile"], (prefix+"infillsExtrusionProfile").c_str() );

		gcoderCfg.extruders.push_back(extruder);
	}

	gcoderCfg.gcoding.outline = boolCheck(conf.root["gcoder"]["outline"], "gcoder.outline");
	gcoderCfg.gcoding.insets  = boolCheck(conf.root["gcoder"]["insets"], "gcoder.insets");
	gcoderCfg.gcoding.infills = boolCheck(conf.root["gcoder"]["infills"], "gcoder.infills");
	gcoderCfg.gcoding.infillFirst = boolCheck(conf.root["gcoder"]["infillFirst"], "gcoder.infillFirst");
	gcoderCfg.gcoding.dualtrick   = boolCheck(conf.root["gcoder"]["dualtrick"], "gcoder.dualtrick");
}

void mgl::loadSlicerConfigFromFile( const Configuration &config, SlicerConfig &slicer)
{
	slicer.layerH = doubleCheck(config["slicer"]["layerH"], "slicer.layerH");
	slicer.firstLayerZ  = doubleCheck(config["slicer"]["firstLayerZ"], "slicer.firstLayerZ");
	slicer.tubeSpacing 	= doubleCheck(config["slicer"]["tubeSpacing"], "slicer.tubeSpacing");
	slicer.angle 		= doubleCheck(config["slicer"]["angle"], "slicer.angle");
	slicer.nbOfShells 	= uintCheck(config["slicer"]["nbOfShells"], "slicer.nbOfShells");
	slicer.layerW 		= doubleCheck(config["slicer"]["layerW"], "slicer.layerW");
	slicer.infillShrinkingMultiplier = doubleCheck(config["slicer"]["infillShrinkingMultiplier"], "slicer.infillShrinkingMultiplier");
	slicer.insetDistanceMultiplier   = doubleCheck(config["slicer"]["insetDistanceMultiplier"], "slicer.insetDistanceMultiplier");
	slicer.insetCuttOffMultiplier  	 = doubleCheck(config["slicer"]["insetCuttOffMultiplier"],  "slicer.insetCuttOffMultiplier");

	slicer.writeDebugScadFiles = boolCheck(config["slicer"]["writeDebugScadFiles"], "slicer.writeDebugScadFiles");

}
