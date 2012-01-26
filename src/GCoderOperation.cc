/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/
#define __need_timeval

#include <assert.h>
#include <ctime>
#include <cmath>
#include <iomanip>

#include "GCoderOperation.h"

#include "mgl/segment.h"
#include "mgl/scadtubefile.h"
#include "mgl/abstractable.h"
#include "json-cpp/include/json/value.h"

using namespace std;
using namespace mgl;
using namespace Json;




/// other global static values.
static Value* GCoderOperationConfigRequirements;

/************** Start of Functions custom to this <NAME_OF>Operation ***********************/

// ToolHead is a state machine that knows
// where it is, and how fast its going




/************** End of Functions custom to this <NAME_OF>Operation ***********************/


/// This staic global pointer points to a unique instance of a
/// Value object, which contains the minimum configuration values
/// required to build a working operation of this type.
/// Must be named <OperationName>ConfigRequirements so it does not collide with




/**
 * This is an accessor function to retreve a singleton of <OperationName>ConfigRequirements
 * if that dictionary does not exit, this function will generate it.
 * The returned dictionary specifies the minimum configuration settings needed to make a valid
 * initaliation of an Operation of this type
 *
 * @return global static Value pointer to configuration dictionary.
 */
Value* GCoderOperation::getStaticConfigRequirements()
{
	// if we don't have one of these global static's, we have never initalized,
	// so initalize now.
	if (GCoderOperationConfigRequirements == 0x00)
	{
		// - Start custom to GCoderOperation code
		// for this Example operation, we need a prefix and a language specified
		// to initalize
		Value* cfg = new Value;
//		( *cfg )["prefix"]= "asString";
//		( *cfg )["lang"] = "asString";
		// - End custom to GCoderOperation code
		GCoderOperationConfigRequirements = cfg;

		std::cout << __FUNCTION__ << std::endl;
		std::cout << "TODO: Create config Requirements" << std::endl;

		// This object is expected to live until the program dies. No deconstruction !
	}
	return GCoderOperationConfigRequirements;
}


/**
 * Standard Constructor.  Note that an object can be built and exist, but
 * not yet be configured or initalized. See details in implementation.
 * Anything things that do not need configuration
 * SHOULD be initalized in the constructor.
 */

GCoderOperation::GCoderOperation()
{
	// - Start custom to GCoderOperation code
	// Because this logging stream is always this file (and is not configuration dependant,
	//we build it in the constructor, and destroy it  in the destructor.

	this->acceptTypes.push_back(TYPE_PATH_BINARY);
	//this->emitTypes.push_back(); Left in as a note, but this example emits no data type

	// - End custom to GCoderOperation code

}


/**
 * Standard Destructor.  This should close streams (if any are open) and
 * deinitalize the Operation (if it is still initalized). See details in implementation.
 */
GCoderOperation::~GCoderOperation()
{
}



/**
 * This function takes a configuration object, and verifies that it can be used to
 * configure the object.
 * @param config
 * @return
 */
bool GCoderOperation::isValidConfig(Configuration& config) const
{

	cout << __FUNCTION__ << endl;
	return true;
}


/**
 * This function initalizes and configures this Operation to take data. Once this returns,
 * the operation may be sent data envelopes (via 'accept') at any time.  All streams, memory structures,
 * and related infrastructure for a specific processing setup must be created here.
 * @param config a configuration for setting up this operation
 * @param outputs a list of other Operations to send out outgoing packets to
 */
void GCoderOperation::init(Configuration& conf,const std::vector<Operation*> &outputs)
{
	bool baseSuccess = Operation::initCommon(conf, outputs);
	if(baseSuccess){
	// - Start custom to GCoderOperation code
		 gcoder.loadData(conf);
	// - End custom to GCoderOperation code
	}

}



/**
 * This function tears down everything setup in initalization. It will also check to see
 * if a stream is running, and if it is, it forces a final data envelope to be queued before
 * continuing with deiitalization. This will force a data flush in edge or fail cases.
 */
void GCoderOperation::deinit()
{
	assert(this->initalized == true);
	assert(this->pConfig != NULL);

	// IFF we have an ongoing stream, as we are being deinitalized, something went wrong.
	// force a final DataEnvelpe to our operation to flush the end of the stream.
	if(streamRunning)
	{
		// - Following line custom to GCoderOperation code
		cout << "Stream Running at deinit time. Automatically sending final envelope " << endl;

//		DataEnvelope d;
//		d.setFinal();
//		this->accept(d);
//		this->streamRunning = false;
//		d.release(); //release the constuctor default ref count of 1

	}

	// - Start custom to GCoderOperation code
	// - End custom to GCoderOperation code

	this->initalized = false;
	pConfig = NULL;
}

/**
 * This is the heart of envelope processing.
 * @param envelope
 */
void GCoderOperation::processEnvelope(const DataEnvelope& envelope)
{
	assert(0);
}

void GCoderOperation::accept(const PathData& envelope)
{
	/// we should be configured before ever doing this
	assert(this->initalized == true);

	/// If this stream is not running, our first packet starts it running
	if( this->streamRunning == false) {
		this->streamRunning = true;
	}

	// - Start custom to GCoderOperation code

	//cout << "TODO: test cast and/or flag type in GCoderOperation::processEnvelope" << endl;

	stringstream ss;
// 	const PathData &pathData = *(dynamic_cast<const PathData* > (&envelope) );
	const SliceData &pathData = *(dynamic_cast<const SliceData* > (&envelope) );

	gcoder.writeSlice(ss, pathData);
	wrapAndEmit(ss);

	//cout << "TODO: test cast and/or flag type in GCoderOperation::processEnvelope" << endl;

	// - End custom to GCoderOperation code

	return;
}



void GCoderOperation::start(){

	// the first thing that needs to happen in start is the propigation of the
	// start to all other modueles.
	Operation::startCommon();

	// - Start custom to GCoderOperation code
	cout << "GCoderOperation::start() !!" << endl;
	stringstream ss;

	gcoder.writeStartOfFile(ss);


	wrapAndEmit(ss);
	// - End custom to GCoderOperation code

}

void GCoderOperation::finish(){

	// - Start custom to GCoderOperation code
	cout << "GCoderOperation::finish()"<< endl;
	stringstream ss;
	gcoder.writeGcodeEndOfFile(ss);

	wrapAndEmit(ss);

	// - End custom to GCoderOperation code

	// the last things to do in finish is propigate the finish
	// to all other modules.
	Operation::finishCommon();

}






/************** Start of Functions custom to GCoderOperation ***********************/

void GCoderOperation::wrapAndEmit(const stringstream &ss)
{
	GCodeEnvelope* data = new GCodeEnvelope(ss.str().c_str());
	emit(data);
	data->release();
}








