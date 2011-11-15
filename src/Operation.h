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
protected:
    Configuration* pConfig;


    // uint32_t  envelopesProcessed; // count of envelopes processed since reset
    std::vector<Operation*> outputs; // pointers to consumers
    std::vector<Operation*> inputs;  // pointers to input operations
    std::vector<DataEnvelope*> dataEnvelopes;

    std::vector<AtomType> emitTypes;
    std::vector<AtomType> acceptTypes;

    bool initalized;
    bool streamRunning;

    //Json::Value configRequirements;

public:
    /// General base constructor for an Operation
    Operation();

    /// General base destructor for an Operation
    virtual ~Operation();

	bool acceptsType( AtomType type );

    /// Init Function: Called to set the configuration
    virtual void init(Configuration& config,const std::vector<Operation*> &outputs) = 0;

    /// Accepts incoming data, to collect in this Operation until the operation is ready to
    /// process the data, and send it's own data.  It generally checks validity, increments the use count.
    /// and simply returns.
    // TRICKY: in this base implementation, it processes the packet inline, but it will not always do so
    virtual bool accept(DataEnvelope& envelope);

	//sends data to the next operation for it's use
    virtual void emit(DataEnvelope* envelope);

    virtual void start() = 0;

    virtual void finish() = 0;


    /// This function should tear down settings or objects built by collect
    virtual void deinit() = 0;



    /**
     * Base class check to setup outputs list, and set configuration values
     * @param config
     * @param outputs
     * @return
     */
	bool initCommon(Configuration& config,const std::vector<Operation*> &outputs);

    void startCommon();

    void finishCommon();

	void deinitCommon();

	virtual bool isValidConfig(Configuration &cfg) const = 0;

    /**
     * This is the core processing function, most users only need to override
     * this function to create their own models
     * @param envelope
     */
	virtual void processEnvelope(const DataEnvelope& envelope) = 0;


    /**
     * Returns the type of envelope this module can collect without error
     * @return
     */
    std::vector<AtomType>& collectsEnvelopeType() ;

    ///
    /**
     * Returns the type of envelope this module can collect without error
     * @return
     */
    virtual std::vector<AtomType>& emitsEnvelopeType();

    /**
     * Simple test function if an envelope of data is the last
     *
     */	bool isLastEnvelope(const DataEnvelope& envelope);



protected:

     /**
      * configuration object accessor
      * @return
      */
	Configuration &configuration() const ;
};

#endif

