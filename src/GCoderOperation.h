#ifndef GCODEROPERATION_H_
#define GCODEROPERATION_H_

/*
 * GCoderOperation.h
 *
 *  Created on: Oct 14, 2011
 *      Author: farmckon
 */
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

