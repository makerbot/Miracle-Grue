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
#include <iostream>
#include <fstream>



using namespace std;

class GCoderOperation: public Operation
{
	ofstream *outstream;
    // output data collector
    vector<string> gStrings;
    bool initalized;
    Configuration* pConfig;

public:
	GCoderOperation(): initalized(false), outstream(0x00)
	{
		cout << __FUNCTION__ << endl;
		cout << "(Miracle Grue)" << endl;
	};
	virtual ~GCoderOperation(){}

	void init(Configuration& config);
	void start();
	DataEnvelope* processEnvelope(const DataEnvelope& envelope);
	void cleanup();

	std::string interrogate();
	AtomType collectsEnvelopeType();
	AtomType  emitsEnvelopeType();

	const Configuration &configuration()const {return *pConfig;}
	ostream& stream() {return *(this->outstream); }

private:
    void init_machine(std::ostream &ss) const;
    void init_platform(std::ostream &ss) const;
    void init_extruders(std::ostream &ss) const;
    void wait_for_warm_up(std::ostream &ss) const;
    void goto_home_position(std::ostream &ss) const;
    void finish_gcode(std::ostream &ss) const;

	void write(const char *gstring, ostream &ss) const;
	void closeFile();

};

#endif /* GCODEROPERATION_H_ */

