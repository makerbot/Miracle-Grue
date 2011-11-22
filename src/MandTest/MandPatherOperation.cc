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

#include "MandPatherOperation.h"

#include "../json-cpp/include/json/value.h"

using namespace std;
using namespace Json;

/// This staic global pointer points to a unique instance of a
/// Value object, which contains the minimum configuration values
/// required to build a working operation of this type.
/// Must be named <OperationName>ConfigRequirements so it does not collide with
/// other global static values.
static Value* MandPatherOperationConfigRequirements;


/**
 * This is an accessor function to retreve a singleton of <OperationName>ConfigRequirements
 * if that dictionary does not exit, this function will generate it.
 * The returned dictionary specifies the minimum configuration settings needed to make a valid
 * initaliation of an Operation of this type
 *
 * @return global static Value pointer to configuration dictionary.
 */
Value* MandPatherOperation::getStaticConfigRequirements()
{
	// if we don't have one of these global static's, we have never initalized,
	// so initalize now.
	if (MandPatherOperationConfigRequirements == 0x00)
	{
		// - Start custom to MandPatherOperation code
		// for this Example operation, we need a prefix and a language specified
		// to initalize
		Value* cfg = new Value;
		( *cfg )["prefix"]= "asString";
		( *cfg )["lang"] = "asString";
		// - End custom to MandPatherOperation code
		MandPatherOperationConfigRequirements = cfg;
		// This object is expected to live until the program dies. No deconstruction !
	}
	return MandPatherOperationConfigRequirements;
}


/**
 * Standard Constructor.  Note that an object can be built and exist, but
 * not yet be configured or initalized. See details in implementation.
 * Anything things that do not need configuration
 * SHOULD be initalized in the constructor.
 */

MandPatherOperation::MandPatherOperation()
{
	// - Start custom to MandPatherOperation code
	// Because this logging stream is always this file (and is not configuration dependant,
	//we build it in the constructor, and destroy it  in the destructor.

	this->acceptTypes.push_back(/*AtomType*/TYPE_BGL_FILLED);
	this->acceptTypes.push_back(/*AtomType*/TYPE_BGL_PATHED);
	//this->emitTypes.push_back(); Left in as a note, but this example emits no data type

	// - End custom to MandPatherOperation code

}


/**
 * Standard Destructor.  This should close streams (if any are open) and
 * deinitalize the Operation (if it is still initalized). See details in implementation.
 */
MandPatherOperation::~MandPatherOperation()
{
	// IFF we are currently initalized, we need to check for an open stream, as well as deinit
	if(initalized) {

		// - Following line custom to MandPatherOperation code
		cout << " Operation initailzed at destruction time. Automatically running deinit" <<endl;

//		//NOTE:  deinit will check that the data stream is closed, and if needed it will
//		// gaurentee a last DataEnvelope is sent (so we can deinitalize with confidence)
//		this->deinit();
	}

	// - Start custom to MandPatherOperation code

	// - End custom to MandPatherOperation code


}



/**
 * This function takes a configuration object, and verifies that it can be used to
 * configure the object.
 * @param config
 * @return
 */
bool MandPatherOperation::isValidConfig(Configuration& config) const
{

	cout << __FUNCTION__ << endl;

	if(config["MandPatherOperation"].type() !=  /*ValueType.*/objectValue)
	{
		// - Start custom to MandPatherOperation code
		/// TODO:
		/// req = getStaticConfigRequirements():
		/// if (req >= config)
			///return true
		// - End custom to MandPatherOperation code
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
void MandPatherOperation::init(Configuration& config,const std::vector<Operation*> &outputs)
{
	bool baseSuccess = Operation::initCommon(config, outputs);
	if(baseSuccess){
	// - Start custom to MandPatherOperation code

	// - End custom to MandPatherOperation code
	}

}


/**
 * This function tears down everything setup in initalization. It will also check to see
 * if a stream is running, and if it is, it forces a final data envelope to be queued before
 * continuing with deiitalization. This will force a data flush in edge or fail cases.
 */
void MandPatherOperation::deinit()
{
	assert(this->initalized == true);
	assert(this->pConfig != NULL);

	// IFF we have an ongoing stream, as we are being deinitalized, something went wrong.
	// force a final DataEnvelpe to our operation to flush the end of the stream.
	if(streamRunning)
	{
		// - Following line custom to MandPatherOperation code
		cout << "Stream Running at deinit time. Automatically sending final envelope " <<endl;

/*
		DataEnvelope d;
		d.setFinal();
		this->accept(d);
		this->streamRunning = false;
		d.release(); //release the constuctor default ref count of 1
*/

	}

	// - Start custom to MandPatherOperation code

	// - End custom to MandPatherOperation code

	this->initalized = false;
	pConfig = NULL;
}

/**
 * This is the heart of envelope processing.
 * @param envelope
 */
void MandPatherOperation::processEnvelope(const DataEnvelope& envelope)
{
	/// we should be configured before ever doing this
	assert(this->initalized == true);

	/// If this stream is not running, our first packet starts it running
	if( this->streamRunning == false) {
		this->streamRunning = true;
	}

	// - Start custom to MandPatherOperation code

	//NOTE: you probably want to do
	if ( envelope.getAtomType() == /*AtomType*/TYPE_BGL_FILLED) {
		RegionEnvelope* rEnv = (RegionEnvelope*)dynamic_cast<const RegionEnvelope*>(&envelope);
		if(rEnv!= NULL) {
			PathData* mEnvelope = createPathFromFilledRegion(rEnv);
			this->emit( dynamic_cast<const DataEnvelope*>(mEnvelope) ); // pass conversion down the tree
			mEnvelope->release(this); //release the initialreference
		}
		else {
	    	cout << "ERROR: can't conver packet @" << (void*)&envelope << " of type " << (uint32_t)envelope.getAtomType() << endl;
		}

	} else {
    	cout << "ERROR: can't accept packet @" << (void*)&envelope << " of type " << (uint32_t)envelope.getAtomType() << endl;
    }

	// - End custom to MandPatherOperation code

	return;
}

void MandPatherOperation::start() {

	// the first thing that needs to happen in start is the propigation of the
	// start to all other modueles.
	Operation::startCommon();

	// - Start custom to MandPatherOperation code

	// - End custom to MandPatherOperation code
}

void MandPatherOperation::finish(){

	// - Start custom to MandPatherOperation code

	// - End custom to MandPatherOperation code


	// the last things to do in finish is propigate the finish
	// to all other modules.
	Operation::finishCommon();

}


/************** Start of Functions custom to MandPatherOperation ***********************/

PathData* MandPatherOperation::createPathFromFilledRegion(RegionEnvelope* envelope){
	PathData* pd = new PathData(0,0);
	return pd;

}


/************** End of Functions custom to MandPatherOperation ***********************/


