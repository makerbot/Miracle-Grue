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
	DebugOperation(callback_t callback): Operation(callback) {};
	void main();
    void queue(DataEnvelope &data);
    void cleanup();
    std::string interrogate();
    uint32_t acceptsEnvelopeType();
    uint32_t  yieldsEnvelopeType();


};

#endif /* DEBUGOPERATION_H_ */
