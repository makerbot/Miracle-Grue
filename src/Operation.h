/* TODO: License/Metadata Insert
// some concepts by XXX of Mandoline
 *
 */

#include "DataEnvelope.h"
#include "Configuration.h"
#include <iostream>
#include <vector>
#include <string>

#ifndef MG_OPERATION_H
#define MG_OPERATION_H

using namespace std;

/**
 * This class is the base class for all operations in the MiracleGrue engine
 * It is an element in a 'chain of responsibility' model of data processing.
 */
class Operation
{

public:
    Operation() : nextOperation(0x00){};
    virtual ~Operation() {};

    virtual void init(Configuration& config) = 0;
    virtual void cleanup() = 0;
	virtual void collect(const DataEnvelope& envelope) = 0;
    virtual string interrogate() = 0;
    virtual AtomType collectsEnvelopeType() = 0;
    virtual AtomType emitsEnvelopeType() = 0;


protected:

    Operation* nextOperation;


	void setNext(Operation* nextOp) { nextOperation = nextOp;};
	void emitData(const DataEnvelope& envelope)
	{
		if(nextOperation != 0x00) {
			nextOperation->collect(envelope);
		}
		else {
			cout << "WARNING: no next operation" << endl;
		}
	};

};

#endif

