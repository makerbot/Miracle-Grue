/* TODO: License/Metadata Insert
// some concepts by XXX of Mandoline
 *
 */

#include "DataEnvelope.h"
#include <stdio.h>
#include <vector>
#include <string>

#ifndef MG_OPERATION_H
#define MG_OPERATION_H

class OperationAcceptor
{
public:
	virtual void collect(DataEnvelope& envelope) = 0;
};

//class OperationEmitter
//{
//	//TODO: make a reference, and create a blank 'ground/termnial' operator
//	OperationAcceptor* nextOperation;
//	void setNext(OperationAcceptor* nextOp) { nextOperation = nextOp;};
//
//};

//TODO: Convert to object pointer to a specific call in an object
class Operation /*: public OperationAcceptor*/
{

public:

	bool isCancelled;
	char* description;

	std::vector<DataEnvelope> envelopes;

	//TODO: make a reference, and create a blank 'ground/termnial' operator
	Operation* nextOperation;
	virtual void setNext(Operation* nextOp) { nextOperation = nextOp;};
	virtual void collect(DataEnvelope& envelope) = 0;

    Operation() : /*OperationAcceptor(),*/ isCancelled(false), description(0x00) {};


    virtual ~Operation() {};
    virtual void main() = 0;
	//virtual void collect(DataEnvelope& envelope);
    virtual void cleanup() = 0;
    virtual std::string interrogate() = 0;
    virtual uint32_t acceptsEnvelopeType() = 0;
    virtual uint32_t  yieldsEnvelopeType() = 0;

};

#endif

