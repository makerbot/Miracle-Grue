/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/
#include <assert.h>
#include <sstream>

#include "MandInfillOperation.h"
#include "RegionEnvelope.h"
#include "ShellEnvelope.h"

#include "../json-cpp/include/json/value.h"

#include "../BGL/BGLCompoundRegion.h"

using namespace std;
using namespace Json;
using namespace BGL;
/// This staic global pointer points to a unique instance of a
/// Value object, which contains the minimum configuration values
/// required to build a working operation of this type.
/// Must be named <OperationName>ConfigRequirements so it does not collide with
/// other global static values.
static Value* MandInfillOperationConfigRequirements;


/**
 * This is an accessor function to retreve a singleton of <OperationName>ConfigRequirements
 * if that dictionary does not exit, this function will generate it.
 * The returned dictionary specifies the minimum configuration settings needed to make a valid
 * initaliation of an Operation of this type
 *
 * @return global static Value pointer to configuration dictionary.
 */
Value* MandInfillOperation::getStaticConfigRequirements()
{
	// if we don't have one of these global static's, we have never initalized,
	// so initalize now.
	if (MandInfillOperationConfigRequirements == 0x00)
	{
		// - Start custom to MandInfillOperation code
//		Value* cfg = new Value();
//		(*cfg)["extrusionWidth"]  = "asDouble";
//	    (*cfg)["perimeterShells"] = "asInt";
//	    MandInfillOperationConfigRequirements = cfg;
		// This object is expected to live until the program dies. No deconstruction !
	}
	return MandInfillOperationConfigRequirements;
}


/**
 * Standard Constructor.  Note that an object can be built and exist, but
 * not yet be configured or initalized. See details in implementation.
 * Anything things that do not need configuration
 * SHOULD be initalized in the constructor.
 */

MandInfillOperation::MandInfillOperation():Operation()
{
	// - Start custom to MandInfillOperation code
	this->acceptTypes.push_back(/*AtomType*/TYPE_BGL_REGION);
	this->emitTypes.push_back(/*AtomType*/TYPE_BGL_FILLED);// Left in as a note, but this example emits no data type

	// - End custom to MandInfillOperation code

}


/**
 * Standard Destructor.  This should close streams (if any are open) and
 * deinitalize the Operation (if it is still initalized). See details in implementation.
 */
MandInfillOperation::~MandInfillOperation()
{
	// IFF we are currently initalized, we need to check for an open stream, as well as deinit
	if(initalized) {

		// - Following line custom to MandInfillOperation code
		cout << " Operation initailzed at destruction time. Automatically running deinit" <<endl;

//		//NOTE:  deinit will check that the data stream is closed, and if needed it will
//		// gaurentee a last DataEnvelope is sent (so we can deinitalize with confidence)
//		this->deinit();
	}

	// - Start custom to MandInfillOperation code

	// - End custom to MandInfillOperation code


}



/**
 * This function takes a configuration object, and verifies that it can be used to
 * configure the object.
 * @param config
 * @return
 */
bool MandInfillOperation::isValidConfig(Configuration& config) const
{

	cout << __FUNCTION__ << endl;

	if(config["MandInfillOperation"].type() !=  /*ValueType.*/objectValue)
	{
		// - Start custom to MandInfillOperation code

		// - End custom to MandInfillOperation code
		return true;
	}
	cout << "ERROR: configuration is not valid, In BETA accepting config anyway" << endl;
	return true;
}


/**
 * This function initalizes and configures this Operation to take data. Once this returns,
 * the operation may be sent data envelopes (via 'accept') at any time.  All streams, memory structures,
 * and related infrastructure for a specific processing setup must be created here.
 * @param config a configuration for setting up this operation
 * @param outputs a list of other Operations to send out outgoing packets to
 */

void MandInfillOperation::init(Configuration& config,const std::vector<Operation*> &outputs)
{
	bool baseSuccess = Operation::initCommon(config, outputs);
	if(baseSuccess){
		// - Start custom to MandInfillOperation code
		// - End custom to MandInfillOperation code
	}

}


/**
 * This function tears down everything setup in initalization. It will also check to see
 * if a stream is running, and if it is, it forces a final data envelope to be queued before
 * continuing with deiitalization. This will force a data flush in edge or fail cases.
 */
void MandInfillOperation::deinit()
{
	assert(this->initalized == true);
	assert(this->pConfig != NULL);

	// IFF we have an ongoing stream, as we are being deinitalized, something went wrong.
	// force a final DataEnvelpe to our operation to flush the end of the stream.
	if(streamRunning)
	{
		// - Following line custom to MandInfillOperation code
		cout << "Stream Running at deinit time. Automatically sending final envelope " <<endl;

	}

	// - Start custom to MandInfillOperation code

	// - End custom to MandInfillOperation code

	this->initalized = false;
	pConfig = NULL;
}

/**
 * This is the heart of envelope processing.
 * @param envelope
 */
void MandInfillOperation::processEnvelope(const DataEnvelope& envelope)
{
	//cout << __FUNCTION__ << endl;
	//cout <<  "INFILL THIS!!!!" << endl;
	/// we should be configured before ever doing this
	assert(this->initalized == true);

	Configuration& config = *pConfig;


	/// If this stream is not running, our first packet starts it running
	if( this->streamRunning == false) {
		this->streamRunning = true;
	}

	//check for a null value case
	if( ! config.contains("MandInfillOperation") ) {
		cout << "failure of failures" << endl;
		throw "no config options we want"; //TODO: make this more sane
		return;
	}
	double infillDensity = config["MandInfillOperation"]["infillDensity"].asDouble();
    double extrusionWidth = config["MandInfillOperation"]["extrusionWidth"].asDouble();

    //simply update our existing envelope
    RegionEnvelope& env = (RegionEnvelope&)(dynamic_cast<const RegionEnvelope&>(envelope));

    env.infillMask.infillPathsForRegionWithDensity( infillDensity,
    		extrusionWidth, env.infill);

    env.typeID = /*AtomType*/TYPE_BGL_FILLED;
    //cout << __FUNCTION__ << "emitting" << endl;
    emit(dynamic_cast<DataEnvelope*>(&env) );
}

void MandInfillOperation::start(){

	// - Start custom to MandInfillOperation code

	// - End custom to MandInfillOperation code


	// the first thing that needs to happen in start is the propigation of the
	// start to all other modueles.
	Operation::startCommon();

}

void MandInfillOperation::finish(){

	// - Start custom to MandInfillOperation code

	// - End custom to MandInfillOperation code


	// the last things to do in finish is propigate the finish
	// to all other modules.
	Operation::finishCommon();

}

