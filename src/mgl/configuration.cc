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
		ConfigMess mixup(ss.str().c_str());
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
		ConfigMess mixup(ss.str().c_str());
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
		ConfigMess mixup(ss.str().c_str());
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
		ConfigMess mixup(ss.str().c_str());
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
	std::ifstream file(filename, std::ifstream::binary);
	Json::Reader reader;
	reader.parse(file , root);
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

void mgl::loadGCoderData(const Configuration& conf, GCoder &gcoder)
{

	gcoder.programName = stringCheck(conf.root["programName"],"programName");
	gcoder.versionStr  = stringCheck(conf.root["versionStr"],"versionStr");
	gcoder.machineName = stringCheck(conf.root["machineName"],"machineName");
	gcoder.firmware    = stringCheck(conf.root["firmware"], "firmware");

	gcoder.gantry.xyMaxHoming = boolCheck(conf.root["gantry"]["xyMaxHoming"], "gantry.xyMaxHoming");
	gcoder.gantry.zMaxHoming  = boolCheck(conf.root["gantry"]["zMaxHoming" ], "gantry.zMaxHoming");
	gcoder.gantry.scalingFactor = doubleCheck(conf.root["gantry"]["scalingFactor"], "gantry.scalingFactor");
	gcoder.gantry.rapidMoveFeedRate = doubleCheck(conf.root["gantry"]["rapidMoveFeedRate"], "gantry.rapidMoveFeedRate");
	gcoder.platform.temperature = doubleCheck(conf.root["platform"]["temperature"], "platform.temperature");
	gcoder.platform.automated   = boolCheck(conf.root["platform"]["automated"], "platform.automated");
	gcoder.platform.waitingPositionX = doubleCheck(conf.root["platform"]["waitingPositionX"], "platform.waitingPositionX");
	gcoder.platform.waitingPositionY = doubleCheck(conf.root["platform"]["waitingPositionY"], "platform.waitingPositionY");
	gcoder.platform.waitingPositionZ = doubleCheck(conf.root["platform"]["waitingPositionZ"], "platform.waitingPositionZ");

	gcoder.outline.enabled  = boolCheck(conf.root["outline"]["enabled"], "outline.enabled");
	gcoder.outline.distance = doubleCheck(conf.root["outline"]["distance"], "outline.distance");

	if(conf.root["extruders"].size() ==0)
	{
		stringstream ss;
		ss << "No extruder defined in the configuration file";
		ConfigMess mixup(ss.str().c_str());
		throw mixup;
	}

	unsigned int extruderCount = conf.root["extruders"].size();
	for(int i=0; i < extruderCount; i++)
	{
		stringstream ss;
		ss << "extruders[" << i << "].";
		string prefix = ss.str();

		Extruder extruder;
		extruder.coordinateSystemOffsetX = doubleCheck(conf.root["extruders"][i]["coordinateSystemOffsetX"], (prefix+"coordinateSystemOffsetX").c_str() );
		extruder.extrusionTemperature = doubleCheck(conf.root["extruders"][i]["extrusionTemperature"], (prefix+"extrusionTemperature").c_str() );
		extruder.nozzleZ = doubleCheck(conf.root["extruders"][i]["nozzleZ"], (prefix+"nozzleZ").c_str() );
		extruder.zFeedRate = doubleCheck(conf.root["extruders"][i]["zFeedRate"], (prefix+"zFeedRate").c_str() );

		extruder.extrusionProfile.feedrate = doubleCheck(conf.root["extruders"][i]["extrusionProfile"]["feedrate"], (prefix+"extrusionProfile.feedrate").c_str() );
		extruder.extrusionProfile.flow = doubleCheck(conf.root["extruders"][i]["extrusionProfile"]["flow"], (prefix+"extrusionProfile.flow").c_str() );
		extruder.extrusionProfile.leadIn= doubleCheck(conf.root["extruders"][i]["extrusionProfile"]["leadIn"], (prefix+"extrusionProfile.leadIn").c_str() );
		extruder.extrusionProfile.leadOut= doubleCheck(conf.root["extruders"][i]["extrusionProfile"]["leadOut"], (prefix+"extrusionProfile.leadOut").c_str() );
		extruder.extrusionProfile.snortFlow= doubleCheck(conf.root["extruders"][i]["extrusionProfile"]["snortFlow"], (prefix+"extrusionProfile.snortFlow").c_str() );
		extruder.extrusionProfile.snortFeedrate= doubleCheck(conf.root["extruders"][i]["extrusionProfile"]["snortFeedrate"], (prefix+"extrusionProfile.snortFeedrate").c_str() );
		extruder.extrusionProfile.squirtFlow= doubleCheck(conf.root["extruders"][i]["extrusionProfile"]["squirtFlow"], (prefix+"extrusionProfile.squirtFlow").c_str() );
		extruder.extrusionProfile.squirtFeedrate= doubleCheck(conf.root["extruders"][i]["extrusionProfile"]["squirtFeedrate"], (prefix+"extrusionProfile.squirtFeedrate").c_str() );
		gcoder.extruders.push_back(extruder);
	}

	gcoder.gcoding.outline = boolCheck(conf.root["gcoder"]["outline"], "gcoder.outline");
	gcoder.gcoding.insets = boolCheck(conf.root["gcoder"]["insets"], "gcoder.insets");
	gcoder.gcoding.infills = boolCheck(conf.root["gcoder"]["infills"], "gcoder.infills");
	gcoder.gcoding.infillFirst = boolCheck(conf.root["gcoder"]["infillFirst"], "gcoder.infillFirst");
	gcoder.gcoding.firstLayerExtrudeMultiplier = doubleCheck(conf.root["gcoder"]["firstLayerExtrudeMultiplier"], "gcoder.firstLayerExtrudeMultiplier");
	gcoder.gcoding.dualtrick = boolCheck(conf.root["gcoder"]["dualtrick"], "gcoder.dualtrick");
}

void mgl::loadSlicerDaTa( const Configuration &config, Slicer &slicer)
{
	slicer.layerH = doubleCheck(config["slicer"]["layerH"], "slicer.layerH");
	slicer.firstLayerZ = doubleCheck(config["slicer"]["firstLayerZ"], "slicer.firstLayerZ");
	slicer.tubeSpacing 	= doubleCheck(config["slicer"]["tubeSpacing"], "slicer.tubeSpacing");
	slicer.angle 		= doubleCheck(config["slicer"]["angle"], "slicer.angle");
	slicer.nbOfShells 	= uintCheck(config["slicer"]["nbOfShells"], "slicer.nbOfShells");
	slicer.layerW 		= doubleCheck(config["slicer"]["layerW"], "slicer.layerW");
	slicer.infillShrinkingMultiplier 		= doubleCheck(config["slicer"]["infillShrinkingMultiplier"], "slicer.infillShrinkingMultiplier");
	slicer.insetDistanceMultiplier  = doubleCheck(config["slicer"]["insetDistanceMultiplier"], "slicer.insetDistanceMultiplier");
	slicer.insetCuttOffMultiplier  	= doubleCheck(config["slicer"]["insetCuttOffMultiplier"],  "slicer.insetCuttOffMultiplier");
}
