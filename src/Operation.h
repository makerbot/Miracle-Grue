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

//TODO: Convert to object pointer to a specific call in an object
class Operation {

public:
	typedef DataEnvelope& (Operation::*collectorFunction)(DataEnvelope&);
	//typedef DataEnvelope* (Operation::&collector)(DataEnvelope*);
	typedef collectorFunction callback_t;


public:

	bool isCancelled;
	char* description;
	callback_t yieldToCallback; // for now, still a ptr, not yet a reference

	std::vector<DataEnvelope> envelopes;

    Operation(callback_t  callback) : isCancelled(false), description(0x00),
    		yieldToCallback(callback) {}


    virtual ~Operation() {};
    virtual void setYielder(callback_t  callback) { yieldToCallback = callback;};
    virtual void main() = 0;
    virtual void queue(DataEnvelope &data) = 0;
    virtual void cleanup() = 0;
    virtual std::string interrogate() = 0;
    virtual uint32_t acceptsEnvelopeType() = 0;
    virtual uint32_t  yieldsEnvelopeType() = 0;

};

#endif

