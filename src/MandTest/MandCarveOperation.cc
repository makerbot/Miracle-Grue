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

#include "MandCarveOperation.h"
#include "ShellEnvelope.h"
#include "StlEnvelope.h"

#include "../json-cpp/include/json/value.h"

#include "../BGL/BGLMesh3d.h"
#include "../BGL/BGLCompoundRegion.h"

using namespace std;
using namespace Json;
using namespace BGL;


/// This staic global pointer points to a unique instance of a
/// Value object, which contains the minimum configuration values
/// required to build a working operation of this type.
/// Must be named <OperationName>ConfigRequirements so it does not collide with
/// other global static values.
static Value* MandCarveOperationConfigRequirements;


/**
 * This is an accessor function to retreve a singleton of <OperationName>ConfigRequirements
 * if that dictionary does not exit, this function will generate it.
 * The returned dictionary specifies the minimum configuration settings needed to make a valid
 * initaliation of an Operation of this type
 *
 * @return global static Value pointer to configuration dictionary.
 */
Value* MandCarveOperation::getStaticConfigRequirements()
{
	// if we don't have one of these global static's, we have never initalized,
	// so initalize now.
	if (MandCarveOperationConfigRequirements == 0x00)
	{
		// - Start custom to MandCarveOperation code
		// for this Example operation, we need a prefix and a language specified
		// to initalize
		Value* cfg = new Value;

		// - End custom to MandCarveOperation code
		MandCarveOperationConfigRequirements = cfg;
		// This object is expected to live until the program dies. No deconstruction !
	}
	return MandCarveOperationConfigRequirements;
}


/**
 * Standard Constructor.  Note that an object can be built and exist, but
 * not yet be configured or initalized. See details in implementation.
 * Anything things that do not need configuration
 * SHOULD be initalized in the constructor.
 */

MandCarveOperation::MandCarveOperation():Operation()
{
	// - Start custom to MandCarveOperation code
	this->acceptTypes.push_back(/*AtomType*/TYPE_BGL_MESH);
	this->emitTypes.push_back(/*AtomType*/TYPE_BGL_SHELL);
	// - End custom to MandCarveOperation code

}


/**
 * Standard Destructor.  This should close streams (if any are open) and
 * deinitalize the Operation (if it is still initalized). See details in implementation.
 */
MandCarveOperation::~MandCarveOperation()
{
	// IFF we are currently initalized, we need to check for an open stream, as well as deinit
	if(initalized) {

		// - Following line custom to MandCarveOperation code
		cout << " Operation initailzed at destruction time. Automatically running deinit" <<endl;

		//NOTE:  deinit will check that the data stream is closed, and if needed it will
		// gaurentee a last DataEnvelope is sent (so we can deinitalize with confidence)
		this->deinit();
	}

	// - Start custom to MandCarveOperation code

	// - End custom to MandCarveOperation code


}



/**
 * This function takes a configuration object, and verifies that it can be used to
 * configure the object.
 * @param config
 * @return
 */
bool MandCarveOperation::isValidConfig(Configuration& config) const
{

	cout << __FUNCTION__ << endl;

	if(config["MandCarveOperation"].type() !=  /*ValueType.*/objectValue)
	{
		// - Start custom to MandCarveOperation code
		// code here should check for specific config values that we require,
		// and make sure they are each valid
		// - End custom to MandCarveOperation code
		return true;
	}
	cout << "ERROR: configuration is not valid, In BETA accepting config anyway" << endl;
	return true;
}



void MandCarveOperation::finish(){
	Operation::finishCommon();
}

void MandCarveOperation::start()
{
	Operation::startCommon();

}



/**
 * This function initalizes and configures this Operation to take data. Once this returns,
 * the operation may be sent data envelopes (via 'accept') at any time.  All streams, memory structures,
 * and related infrastructure for a specific processing setup must be created here.
 * @param config a configuration for setting up this operation
 * @param outputs a list of other Operations to send out outgoing packets to
 */
void MandCarveOperation::init(Configuration& config,const std::vector<Operation*> &outputs)
{
	//For Alpha version of MG engine, pConfig must be null,
	//i.e. We can't re-configure an object once it's been configured
	assert(this->pConfig == NULL);
	assert(this->initalized == false);

	if(isValidConfig(config)){
		cout << __FUNCTION__ << "count of ops" << outputs.size() << endl;
		this->outputs.insert(this->outputs.end(), outputs.begin(), outputs.end());
		this->initalized = true;
		// - Start custom to MandCarveOperation code
		// - End custom to MandCarveOperation code
	}
	else {
		cout << "configuration does not contain valid data" << endl;
		assert(0);
	}
}


/**
 * This function tears down everything setup in initalization. It will also check to see
 * if a stream is running, and if it is, it forces a final data envelope to be queued before
 * continuing with deiitalization. This will force a data flush in edge or fail cases.
 */
void MandCarveOperation::deinit()
{
	assert(this->initalized == true);
	assert(this->pConfig != NULL);

	// IFF we have an ongoing stream, as we are being deinitalized, something went wrong.
	// force a final DataEnvelpe to our operation to flush the end of the stream.
	if(streamRunning)
	{
		// - Following line custom to MandCarveOperation code
		cout << "Stream Running at deinit time. Automatically sending final envelope " <<endl;

		DataEnvelope d(TYPE_INVALID);
		d.setFinal();
		this->accept(d);
		this->streamRunning = false;
		d.release(); //release the constuctor default ref count of 1
	}

	// - Start custom to MandCarveOperation code

	// - End custom to MandCarveOperation code

	this->initalized = false;
	pConfig = NULL;
}


ShellEnvelope* ShellEnvelopeFromMesh(BGL::Mesh3d& mesh , float  zLayer ){
	CompoundRegion outReg;
	mesh.regionForSliceAtZ(zLayer, outReg);
    double svgWidth  = 40 + mesh.maxX - mesh.minX;
    double svgHeight = 40 + mesh.maxY - mesh.minY;
    double svgXOff   = 20 - mesh.minX;
    double svgYOff   = 20 - mesh.minY;

	ShellEnvelope* env = new ShellEnvelope(outReg, zLayer,
			svgWidth, svgHeight, svgXOff, svgYOff);

	//cout << __FUNCTION__ << " exit " << endl;
	return env;
}


/**
 * This is the heart of envelope processing.
 * @param envelope
 */
void MandCarveOperation::processEnvelope(const DataEnvelope& envelope)
{
//	cout << __FUNCTION__ << " enter " << endl;

	/// we should be configured before ever doing this
	assert(this->initalized == true);

	/// If this stream is not running, our first packet starts it running
	if( this->streamRunning == false) {
		this->streamRunning = true;
	}

//	cout << __FUNCTION__ << endl;

	if(envelope.getAtomType() == TYPE_BGL_MESH)
	{
		const StlEnvelope *stlEnv = (dynamic_cast<const StlEnvelope* > (&envelope) );

		BGL::Mesh3d mesh = stlEnv->getMesh();

	    float topZ = mesh.maxZ;
	    float layerThickness = 0.4; // move to a global config
	    float z = layerThickness/2.0f;

	    while (z < topZ) {
		// - Start custom to MandCarveOperation code
	    // Carve model to find layer outlines
	    	ShellEnvelope* mEnvelope = ShellEnvelopeFromMesh(mesh, z );
			this->emit( dynamic_cast<const DataEnvelope*>(mEnvelope) );
	    	//cout << "blarg, at z layer: " << z << endl;
			z += layerThickness;
	    }
	}
	else{
		cout << "Envelope is not of type " + envelope.getAtomType() <<". We cannot accept it"<<
		cout << " into function " << __FUNCTION__ << endl;
	}
	// - End custom to MandCarveOperation code

	envelope.release();
	return;
}

//void MandCarveOperation::finish()
//{
//	Operation::finishCommon();
//}

/************** Start of Functions custom to MandCarveOperation ***********************/


/************** End of Functions custom to MandCarveOperation ***********************/


