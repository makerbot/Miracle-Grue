/*
 * GCoderOperation.h
 *
 *  Created on: Oct 14, 2011
 *      Author: farmckon
 */
#include "Operation.h"
#include <iostream>
#include <fstream>

#ifndef GCODEROPERATION_H_
#define GCODEROPERATION_H_

using namespace std;

class GCoderOperation: public Operation
{
	ofstream *outstream;

    // output data collector
    vector<string> gStrings;

    bool initalized;

public:
	GCoderOperation(): Operation(), initalized(false), outstream(0x00)
	{
		cout << __FUNCTION__ << endl;
		cout << "(Miracle Grue)" << endl;
	};

	void init(Configuration& config);

	void collect(const DataEnvelope& envelope);

	void cleanup();

	std::string interrogate();
	AtomType collectsEnvelopeType();
	AtomType  emitsEnvelopeType();


private:
    void init_machine(ostream &ss) const;
    void init_platform(ostream &ss) const;
    void init_extruders(ostream &ss) const;
	void write(const char *gstring, ostream &ss) const;
	void closeFile(ofstream *fs) const;

};

#endif /* GCODEROPERATION_H_ */
