/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/

#define OMPFF // openMP mulitithreading extensions This Fu packs a ompff!


#include <iostream>
#include <string>

#include <stdlib.h>
#include "mgl/abstractable.h"
#include "mgl/meshy.h"
#include "Configuration.h"
#include "GCoderOperation.h"
#include "FileWriterOperation.h"

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





int main(int argc, char *argv[], char *envp[])
{
	// design by contract ;-)
	int checks = preConditionsOrShowUsage(argc, argv);
	if(checks != 0)
	{
		return checks;
	}

	string modelFile;
	double firstLayerZ = 0.11;
	double layerH = 0.35;
	double layerW = 0.583333;
	double tubeSpacing = 0.5;
	double angle = M_PI *0.5;
	string configFileName = "miracle.config";


    Configuration config;
    config.readFromFile(configFileName.c_str());

    parseArgs(config, argc, argv, modelFile, configFileName);
    cout << config.asJson() << endl;


	MyComputer computer;
	cout << endl;
	cout << endl;

	// std::string modelFile = models[i];
	cout << "firstLayerZ (f) = " << firstLayerZ << endl;
	cout << "layerH (h) = " << layerH << endl;
	cout << "layerW (w) = " << layerW << endl;
	cout << "tubeSpacing (t) = " << tubeSpacing  << endl;
	cout << "angle (a) = " << angle << endl;
	cout << "configuration file (c) = " <<  configFileName << endl;
	cout << endl;

	cout << "behold!" << endl;
	cout << "Materialization of \"" << modelFile << "\" has begun at " << computer.clock.now() << endl;


	std::string stlFiles = computer.fileSystem.removeExtension(computer.fileSystem.ExtractFilename(modelFile));
	stlFiles += "_";

	std::string scadFile = "."; // outDir;
	scadFile += computer.fileSystem.getPathSeparatorCharacter();
	scadFile += computer.fileSystem.ChangeExtension(computer.fileSystem.ExtractFilename(modelFile), ".scad" );

	std::string gcodeFile = ".";
	gcodeFile += computer.fileSystem.getPathSeparatorCharacter();
	gcodeFile += computer.fileSystem.ChangeExtension(computer.fileSystem.ExtractFilename(modelFile), ".gcode" );

	cout << endl << endl;
	cout << modelFile << " to \"" << gcodeFile << "\" and \"" << scadFile << "\"" << endl;

	Meshy mesh(config["slicer"]["firstLayerZ"].asDouble(), config["slicer"]["layerH"].asDouble()); // 0.35
	loadMeshyFromStl(mesh, modelFile.c_str());

	std::vector< TubesInSlice >  allTubes;

	cout << "Slicing" << endl;

	sliceAndPath(mesh,
			layerW,
			tubeSpacing,
			angle,
			scadFile.c_str(),
			allTubes); //paths);

    writeGcodeFile(config, gcodeFile.c_str(), allTubes);

	cout << endl << "Done!" << endl;

}
