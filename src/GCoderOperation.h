/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/
#ifndef GCODEROPERATION_H_
#define GCODEROPERATION_H_

#include "Operation.h"
#include "PathData.h"

#include <iostream>
#include <fstream>



class GCoderOperation: public Operation
{
	std::ofstream *pStream;
    // output data collector
    std::vector<std::string> gStrings;

    Configuration* pConfig;

public:
	GCoderOperation();

	virtual ~GCoderOperation(){}

	void init(Configuration& config);
	void start();
	void finish();

	DataEnvelope* processEnvelope(const DataEnvelope& envelope);
	void cleanup();

	std::string interrogate();
	AtomType collectsEnvelopeType();
	AtomType  emitsEnvelopeType();

	const Configuration &configuration()const {return *pConfig;}
	std::ostream& stream();

private:

    // write important config information in gcode file
    void writeGCodeConfig();
	void writeMachineInitialization() ;
    void writePlatformInitialization() ;
    void writeExtrudersInitialization() ;
    void writeWarmupSequence();
    void writeHomingSequence();
    void writeGcodeEndOfFile();

    void writePaths(const PathData& pathData);

	//void write(const char *gstring, ostream &ss) const;
	void closeFile();

};

#endif /* GCODEROPERATION_H_ */

