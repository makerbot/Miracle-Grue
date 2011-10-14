/*
 * DebugOperation.h
 *
 *  Created on: Oct 13, 2011
 *      Author: farmckon
 */

#include "Operation.h"

#ifndef DEBUGOPERATION_H_
#define DEBUGOPERATION_H_

class DebugOperation : public Operation
{

public:
	DebugOperation() {};

	// -- from Operation Acceptor
	void collect(DataEnvelope& envelope);

	// -- from Operation
    void init(Configuration& config);
    void cleanup();
	void collect(const DataEnvelope& envelope);
    string interrogate();
    AtomType collectsEnvelopeType();
    AtomType emitsEnvelopeType();


};

#endif /* DEBUGOPERATION_H_ */
