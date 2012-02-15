/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/

//#define OMPFF // openMP mulitithreading extensions This Fu packs a ompff!


#include <iostream>
#include <string>

#include <stdlib.h>
#include "mgl/abstractable.h"
#include "mgl/configuration.h"
#include "mgl/gcoder.h"
#include "mgl/slicy.h"
//#include "FileWriterOperation.h"

using namespace std;
using namespace mgl;


double numberFromCharEqualsStr(const std::string& str)
{
	string nb = str.substr(2, str.length()-2);
	double val = atof(nb.c_str());
	return val;
}

void parseArgs(Configuration &config,
				int argc,
				char *argv[],
				string &modelFile,
				string &configFileName)
{
	modelFile = argv[argc-1];
    for(int i = 1;i < argc - 1;i++){
        string str = argv[i];
        cout << i << " " << str << endl;
        if(str.find("f=") != string::npos)
        	config["slicer"]["firstLayerZ"]  = numberFromCharEqualsStr(str);

        if(str.find("h=") != string::npos)
        	config["slicer"]["layerH"] = numberFromCharEqualsStr(str);

        if(str.find("w=") != string::npos)
        	config["slicer"]["layerW"] = numberFromCharEqualsStr(str);

        if(str.find("t=") != string::npos)
        	config["slicer"]["tubeSpacing"] = numberFromCharEqualsStr(str);

        if(str.find("c=") != string::npos)
        	config["slicer"]["configFileName"] = str.substr(2, str.length()-2);

        if(str.find("a=") != string::npos)
        	config["slicer"]["angle"] = numberFromCharEqualsStr(str);
    }
}


int preConditionsOrShowUsage(int argc, char *argv[])
{
	if (argc < 2)
	{
		cout << endl;
		cout << "*-------------------------------------*" << endl;
		cout << argv[0]<< endl;
		cout << "It is pitch black. You are likely to be eaten by a grue." << endl;
		cout << "Find the light: try " << argv[0] << " [FILE]" << endl;
		return (-1);
	}
	return 0;
}



double doubleCheck(Json::Value &value, const char *name)
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

unsigned int uintCheck(Json::Value &value, const char *name)
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

int main(int argc, char *argv[], char *envp[])
{

	// design by contract ;-)
	int checks = preConditionsOrShowUsage(argc, argv);
	if(checks != 0)
	{
		return checks;
	}

	string modelFile;
	string configFileName = "miracle.config";


    Configuration config;
    config.readFromFile(configFileName.c_str());

    parseArgs(config, argc, argv, modelFile, configFileName);
    cout << config.asJson() << endl;


	MyComputer computer;
	cout << endl;
	cout << endl;

	cout << "CHECK " << config["slicer"]["firstLayerZ"].isNull();

	// std::string modelFile = models[i];
	cout << "firstLayerZ (f) = " << config["slicer"]["firstLayerZ"].asDouble()  << endl;
	cout << "layerH (h) = " << config["slicer"]["layerH"].asDouble()  << endl;
	cout << "layerW (w) = " << config["slicer"]["layerW"].asDouble()  << endl;
	cout << "tubeSpacing (t) = " << config["slicer"]["tubeSpacing"].asDouble()   << endl;
	cout << "angle (a) = " << config["slicer"]["angle"].asDouble() << endl;
	cout << "configuration file (c) = " <<  configFileName << endl;
	cout << endl;

	cout << "behold!" << endl;
	cout << "Materialization of \"" << modelFile << "\" has begun at " << computer.clock.now() << endl;


	std::string stlFiles = computer.fileSystem.removeExtension(computer.fileSystem.ExtractFilename(modelFile));
	stlFiles += "_";

	std::string scadFile = "."; // outDir
	scadFile += computer.fileSystem.getPathSeparatorCharacter();
	scadFile += computer.fileSystem.ChangeExtension(computer.fileSystem.ExtractFilename(modelFile), ".scad" );

	std::string gcodeFile = ".";
	gcodeFile += computer.fileSystem.getPathSeparatorCharacter();
	gcodeFile += computer.fileSystem.ChangeExtension(computer.fileSystem.ExtractFilename(modelFile), ".gcode" );

	cout << endl << endl;
	cout << modelFile << " to \"" << gcodeFile << "\" and \"" << scadFile << "\"" << endl;

	Scalar layerH = doubleCheck(config["slicer"]["layerH"], "slicer.layerH");
	Scalar firstLayerZ = doubleCheck(config["slicer"]["firstLayerZ"], "slicer.firstLayerZ");
	Meshy mesh(firstLayerZ, layerH); // 0.35
	loadMeshyFromStl(mesh, modelFile.c_str());

	unsigned int sliceCount = mesh.readSliceTable().size();
	unsigned int extruderId = 0;

	Scalar tubeSpacing = doubleCheck(config["slicer"]["tubeSpacing"], "slicer.tubeSpacing");
	Scalar angle = doubleCheck(config["slicer"]["angle"], "slicer.angle");
	unsigned int nbOfShells = uintCheck(config["slicer"]["nbOfShells"], "slicer.nbOfShells");
	Scalar layerW = doubleCheck(config["slicer"]["layerW"], "slicer.layerW");


	Slicy slicy(mesh.readAllTriangles(), mesh.readLimits(), layerW, layerH, sliceCount, scadFile.c_str());

	std::vector< SliceData >  slices;
	slices.reserve( mesh.readSliceTable().size());

	Scalar infillShrinking = config["slicer"]["infillShrinking"].asDouble();
	Scalar insetDistanceFactor  = config["slicer"]["insetDistance"].asDouble();

	cout << "Slicing" << endl;
	for(unsigned int sliceId=0; sliceId < sliceCount; sliceId++)
	{
		const TriangleIndices & trianglesForSlice = mesh.readSliceTable()[sliceId];
		Scalar z = mesh.readLayerMeasure().sliceIndexToHeight(sliceId);
		Scalar sliceAngle = sliceId * angle;
		slices.push_back( SliceData(z,sliceId));
		SliceData &slice = slices[sliceId];

		bool hazNewPaths = slicy.slice( trianglesForSlice,
										z,
										sliceId,
										extruderId,
										tubeSpacing,
										sliceAngle,
										nbOfShells,
										infillShrinking,
										insetDistanceFactor,
										slice);
		if(!hazNewPaths)
		{
	    	cout << "WARNING: Layer " << sliceId << " has no outline!" << endl;
			slices.pop_back();
		}
	}

	GCoder gcoder = GCoder();
	gcoder.loadData(config);
    std::ofstream gout(gcodeFile.c_str());
    gcoder.writeStartOfFile(gout);

    ProgressBar progress(slices.size());
    for(int i = 0; i < slices.size(); i++)
    {
        progress.tick();
        cout.flush();
        const SliceData &slice = slices[i];
        gcoder.writeSlice(gout, slice);
    }

    gcoder.writeGcodeEndOfFile(gout);
    gout.close();

    cout << endl << computer.clock.now() << endl;
    cout << "Done!" << endl;

}
