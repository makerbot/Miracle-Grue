/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/

#include "Operation.h"
#include "assert.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

/// General base constructor for an Operation
Operation::Operation()
 :pConfig(NULL), initalized(false), streamRunning(false)
{

}

/// General base destructor for an Operation
Operation::~Operation()
{

}


bool Operation::acceptsType( AtomType type )
{
	//cout << "Accept Type Test!" << endl;
	vector<AtomType>::iterator it;
	for ( it = acceptTypes.begin() ; it < acceptTypes.end(); it++ )  {
		AtomType i = * it;
		if ( i == type){
				//cout << "takes type: " << i << endl;
				return true;
		}
	}
	cout << "this type of DataEnvelope not accepted" << endl;
	return false;
};

/// Accepts incoming data, to collect in this Operation until the operation is ready to
/// process the data, and send it's own data.  It generally checks validity, increments the use count.
/// and simply returns.
// TRICKY: in this base implementation, it processes the packet inline, but it will not always do so
bool Operation::accept(DataEnvelope& envelope)
{
	//check incoming envelope data type
	if( false ==  acceptsType( envelope.getAtomType() ) ) {
		cout << " Accept Type Failure !" << endl;
		return false;
	}

	envelope.addRef(); //matching 'release' for this object is in function 'emit'
	processEnvelope(envelope);

	return true;
	// FUTURE:
	// validate(envelope);
	// bool canThread = requestThreadFromPool(this);
	//if(canThread) //process that envelope later
	//	this.envQueue.push_back(envelope);
	//else // process envelope now
	//  processEnvelope(envelope);
}


///sends data to the next operation for it'suse
void Operation::emit(DataEnvelope* envelope)
{
	//cout << __FUNCTION__  << endl;
	dataEnvelopes.push_back(envelope);
	for( vector<Operation*>::iterator i = outputs.begin(); i != outputs.end(); i++)
	{
		Operation& op = *(*i);
		//cout << "emitting to @" << *i << endl;
		bool accepted = op.accept(*envelope);
		if(accepted){
			//cout << "op accepted to @" << envelope << endl;
			envelope->release(); //matching 'addRef' for this object is in fuction 'accept'
		}
		 else
			 cout << __FUNCTION__ << "packet not accepted by next operation. Won't decrement use for safety" << endl;

	}

}
    
/**
 * Base class check to setup outputs list, and set configuration values
 * @param config
 * @param outputs
 * @return
 */
bool Operation::initCommon(Configuration& config,const vector<Operation*> &outputs)
{
	//For Alpha version of MG engine, pConfig must be null, and initalizes must be false.
	//i.e. We can't call init twice on a single object.
	assert(this->pConfig == NULL);
	assert(this->initalized == false);

	if(isValidConfig(config)){
		this->outputs.insert(this->outputs.end(), outputs.begin(), outputs.end());
		this->initalized = true;
		this->pConfig = &config;
		return true;
	}
	else {
		cout << "configuration does not contain valid data" << endl;
		assert(0);
	}
	return false;
}


void Operation::startCommon()
{
	if( this->streamRunning == false) {
		this->streamRunning = true;
	}

	for( vector<Operation*>::iterator i = inputs.begin(); i != inputs.end(); i++)
	{
		Operation& op = *(*i);
		op.start();
	}
}

    
void Operation::finishCommon()
{
	if( this->streamRunning == true) {
		this->streamRunning = false;
	}

	for( vector<Operation*>::iterator i = outputs.begin(); i != outputs.end(); i++)
	{
		Operation& op = *(*i);
		op.finish();
	}
}

void Operation::deinitCommon() { }




/// Returns the type of envelope this module can collect without error
vector<AtomType>& Operation::collectsEnvelopeType()
{
		return acceptTypes;
}

/// Returns the type of envelope this module can collect without error
vector<AtomType>& Operation::emitsEnvelopeType()
{
		return emitTypes;
}

/// Simple test function if an envelope of data is the last
bool Operation::isLastEnvelope(const DataEnvelope& envelope)
{
	return envelope.isLastEnvelope();
}




Configuration& Operation::configuration() const
{
	return *pConfig;
}

