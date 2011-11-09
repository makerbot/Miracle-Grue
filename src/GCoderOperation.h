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


#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>

#include "Operation.h"

#include "PathData.h"    // input data
#include "GCodeData.h"	 // output data


class GCoderOperation: public Operation
{

public:
	GCoderOperation();

	virtual ~GCoderOperation(){}

	void start();
	void finish();

	void processEnvelope(const DataEnvelope& envelope);
	void cleanup();


private:

	void emit(const char* msg);

    // write important config information in gcode file
    void writeGCodeConfig(std::ostream &ss) const;
	void writeMachineInitialization(std::ostream &ss) const;
    void writePlatformInitialization(std::ostream &ss) const;
    void writeExtrudersInitialization(std::ostream &ss) const;
    void writeHomingSequence(std::ostream &ss) const;
    void writeWarmupSequence(std::ostream &ss) const;
    void writeAnchor(std::ostream &ss) const;

    void writePaths(std::ostream &ss, const PathData& pathData) const;

    void writeSwitchExtruder(std::ostream& ss, int extruderId) const;
    void writeWipeExtruder(std::ostream& ss, int extruderId) const;

    void writeGcodeEndOfFile(std::ostream &ss) const;
};

#endif /* GCODEROPERATION_H_ */

