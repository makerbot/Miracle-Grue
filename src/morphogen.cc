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

using namespace std;
using namespace mgl;


double numberFromCharEqualsStr(const std::string& str)
{
	string nb = str.substr(2, str.length()-2);
	double val = atof(nb.c_str());
	return val;
}

void parseArgs(int argc, char *argv[], string &modelFile, double & firstLayerZ, double & layerH, double & layerW, double & tubeSpacing)
{
	modelFile = argv[argc-1];
    for(int i = 1;i < argc - 1;i++){
        string str = argv[i];
        cout << i << " " << str << endl;
        if(str.find("f=") != string::npos)
            firstLayerZ = numberFromCharEqualsStr(str);

        if(str.find("h=") != string::npos)
            layerH = numberFromCharEqualsStr(str);

        if(str.find("w=") != string::npos)
            layerW = numberFromCharEqualsStr(str);

        if(str.find("t=") != string::npos)
            tubeSpacing = numberFromCharEqualsStr(str);
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
}


int main(int argc, char *argv[], char *envp[])
{
	// design by contract ;-)
	int checks = preConditionsOrShowUsage(argc, argv);
	if(checks != 0) return checks;

	string modelFile;
	cout << "wer "<< argc -2 << endl;
	double firstLayerZ = 0.11;
	double layerH = 0.35;
	double layerW = 0.583333;
	double tubeSpacing = 0.5;

    parseArgs(argc, argv, modelFile,  firstLayerZ, layerH, layerW, tubeSpacing);

	MyComputer hal9000;
	cout << endl;
	cout << endl;
	cout << "behold!" << endl;
	cout << "Materialization of \"" << modelFile << "\" has begun at " << hal9000.clock.now() << endl;

	// std::string modelFile = models[i];
	cout << "firstLayerZ (f) = " << firstLayerZ << endl;
	cout << "layerH (h) = " << layerH << endl;
	cout << "layerW (w) = " << layerW << endl;
	cout << "tubeSpacing (t) = " << tubeSpacing  << endl;
	cout << endl;
	std::string stlFiles = hal9000.fileSystem.removeExtension(hal9000.fileSystem.ExtractFilename(modelFile));
	stlFiles += "_";

	std::string scadFile = "."; // outDir;
	scadFile += hal9000.fileSystem.getPathSeparatorCharacter();
	scadFile += hal9000.fileSystem.ChangeExtension(hal9000.fileSystem.ExtractFilename(modelFile), ".scad" );

	std::string stlPrefix = ".";
	stlPrefix += hal9000.fileSystem.getPathSeparatorCharacter();
	stlPrefix += stlFiles.c_str();
	cout << endl << endl;
	cout << modelFile << " to " << stlPrefix << "*.stl and " << scadFile << endl;

	sliceAndScad(modelFile.c_str(), firstLayerZ, layerH, layerW, tubeSpacing, stlPrefix.c_str(), scadFile.c_str());

	cout << "Sliced until " << hal9000.clock.now() << endl;
	cout << endl;
}
