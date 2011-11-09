/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/

#ifndef MG_OPERATION_H
#define MG_OPERATION_H

#include "DataEnvelope.h"
#include "Configuration.h"
#include <iostream>
#include <vector>
#include <string>

#include "json-cpp/include/json/value.h"


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
    Configuration* pConfig;
protected:

    // uint32_t  envelopesProcessed; // count of envelopes processed since reset
    std::vector<Operation*> outputs; // pointers to consumers
    std::vector<Operation*> inputs;  // pointers to input operations
    std::vector<DataEnvelope*> dataEnvelopes;

    std::vector<AtomType> emitTypes;
    std::vector<AtomType> acceptTypes;

    //Json::Value interfaceDict;

public:
    /// General base constructor for an Operation
    Operation()
     :pConfig(NULL)
    {

    }

    /// General base destructor for an Operation
    virtual ~Operation()
    {

    }

    // this is an accessor so that operation can
    // access their input's previous data
    const std::vector<DataEnvelope*>& get_data()
    {
    	return dataEnvelopes;
    }

    /// Init Function: Called to set the configuration
    virtual void init(Configuration& config, const std::vector<Operation*> &inputs, const std::vector<Operation*> &outputs)
    {
    	// copy the contents of the input output vectors
    	this->outputs = outputs;
    	this->inputs = inputs;
    	this->pConfig = &config;
    }

    // Accepts incoming data, to collect in this Operation until the operation is ready to
    // process the data, and send it's own data.  It generally checks validity, increments the use count.
    // and simply returns.
    // TRICKY: in this base implementation, it processes the packet inline, but it will not always do so
    virtual bool accept(DataEnvelope& envelope)
    {
    	envelope.addRef(); //matching 'release' for this object is in function 'emit'
    	// validate(envelope);
    	// bool canThread = requestThreadFromPool(this);

    	//if(canThread) //process that envelope later
    	//	return;
    	//else // process envelope now
    	//  processEnvelope(envelope);

    	processEnvelope(envelope);
    	return true;
    }

	//sends data to the next operation for it'suse
    virtual void emit(DataEnvelope* envelope)
    {
    	dataEnvelopes.push_back(envelope);
    	for( std::vector<Operation*>::iterator i = outputs.begin(); i != outputs.end(); i++)
    	{
    		Operation& op = *(*i);
    		bool accepted = op.accept(*envelope);
    		if(accepted)
    			envelope->release(); //matching 'addRef' for this object is in fuction 'accept'
    		 else
    			 std::cout << __FUNCTION__ << "packet not accepted by next operation. Won't decrement use for safety" << std::endl;

    	}

    }
    
    virtual void start()
    {
    	for( std::vector<Operation*>::iterator i = inputs.begin(); i != inputs.end(); i++)
    	{
    		Operation& op = *(*i);
    		op.start();
    	}
    }
    
    virtual void finish()
    {
    	for( std::vector<Operation*>::iterator i = outputs.begin(); i != outputs.end(); i++)
    	{
    		Operation& op = *(*i);
    		op.finish();
    	}
    }


    // This is the core processing function, most users only need to override
    // this function to create their own models
    virtual void processEnvelope(const DataEnvelope& envelope) = 0;

    // This function should tear down settings or objects built by collect
    virtual void cleanup() {};


    /**
     * This is a static function that returns a Json Dict containing sections MUST, SHOULD, MAY,
     * each section is itself a Json dict containing values required(MUST), allowed(SHOULD), suggested (MAY)
     * in order for this Operation to use correctly. These dicts should be used in constructing the
     * parameters for a Operation Chain.
     *
     * The terms MUST/SHOULD/MAY are used as per RFC 2119 specification
     */
    //virtual Json::Value queryInterface() const = 0;
	virtual Json::Value queryInterface() const = 0;


    /// Returns the type of envelope this module can collect without error
    std::vector<AtomType>& collectsEnvelopeType() {
			return acceptTypes;
	}

    /// Returns the type of envelope this module can collect without error
    virtual std::vector<AtomType>& emitsEnvelopeType(){
			return emitTypes;
	}    

    /// Simple test function if an envelope of data is the last
	bool isLastEnvelope(const DataEnvelope& envelope)
	{ return envelope.isLastEnvelope();}




protected:
	// configuration object accessor
	Configuration &configuration() const
	{
		return *pConfig;
	}
};

#endif

