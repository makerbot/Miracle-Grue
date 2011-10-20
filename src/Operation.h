/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/

#include "DataEnvelope.h"
#include "Configuration.h"
#include <iostream>
#include <vector>
#include <string>

#ifndef MG_OPERATION_H
#define MG_OPERATION_H


/**
 * On Operation is a basic unit of data processing used by the processing chain,
 * each operation accepts one or many DataEnvelopers, and transforms/configures/
 * processes them to generate the next data type in the workflow.
 *
 * Most operation will involve:
 * - Object Construction
 * - Calling 'init' with a configuration
 * - Calling 'collect' one or more times with envelopes of data
 *   (note, the core processing function 'processEnvelope' is called by collect
 * - Calling cleanup to remove/clean/purge data
 * - Object Destruction
 */
class Operation
{
protected:


    Operation* nextOperation; ///pointer to the next operation in the chain
    uint32_t  envelopesProcessed; /// count of envelopes processed since reset

public:
    /// General base constructor for an Operation
    Operation() : nextOperation(0x00), envelopesProcessed(0){};

    /// General base constructor for an Operation
    virtual ~Operation() {};

    /// Init Function: Called to set the configuration
    virtual void init(Configuration& config) = 0;

    // Collect function accepts incoming data.  It usually
    // simply checks validity, and manages pushing data to the next
    // operation, but it can be expanded or overridden.
    virtual void collect(const DataEnvelope& envelope)
    {
    	DataEnvelope* newDataEnvelope = this->processEnvelope(envelope);
    	envelopesProcessed++;
    	if(nextOperation == 0x00)
    		std::cout << "no next operation registered" << std::endl;
    	else if (newDataEnvelope == 0x00)
    		std::cout << "no new dataEnvelope generated" << std::endl;
    	else
    		nextOperation->collect((DataEnvelope&)*newDataEnvelope);
    }

    // This is the core processing function, most users only need to override
    // this function to create their own models
    virtual DataEnvelope* processEnvelope(const DataEnvelope& envelope) = 0;

    // This function should tear down settings or objects built by collect
    virtual void cleanup() = 0;

    // This is a function returns data about how this module is used
    // TODO: this should/can return more detailed and useful data
    virtual std::string interrogate() = 0;

    // Returns the type of envelope this module can collect without error
    virtual AtomType collectsEnvelopeType() = 0;

    // Returns the type of envelope this module can collect without error
    virtual AtomType emitsEnvelopeType() = 0;

    // this function is used to set the next operation in the chain
	void setNext(Operation* nextOp) {
    	if(nextOperation == 0x00)
    		nextOperation = nextOp;
    	else if (nextOp == 0x00)
    		std::cout << "unregistering an operation" << std::endl;
    	else
    		std::cout << "resetting registered operation" << std::endl;
		nextOperation = nextOp;};

	// Simple test function if an envelope of data is the first
	bool isFirstEnvelope(const DataEnvelope& envelope)
	{ return envelopesProcessed == 0 ? true: false;}

	// Simple test function if an envelope of data is the last
	bool isLastEnvelope(const DataEnvelope& envelope)
	{ return envelope.lastFlag;}

protected:

};

#endif

