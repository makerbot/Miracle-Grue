/*
 * DebugOperation.cc
 *
 *  Created on: Oct 13, 2011
 *      Author: farmckon
 */
#include "DebugOperation.h"

void DebugOperation::init(Configuration& config) {}

void DebugOperation::collect(const DataEnvelope &data){
	printf("%s",__FUNCTION__);
	printf("Beans!\n");
	if(data.lastFlag == true)
	{
		printf(" last operation!\n");
	}
	// always call emit data if we can even if just with dummy data!
	emitData(data);
	return;
}

void DebugOperation::cleanup() {
	printf("%s",__FUNCTION__);
	return;
}

std::string DebugOperation::interrogate() {
	printf("%s",__FUNCTION__);
	return "foo";
}

AtomType collectsEnvelopeType();
AtomType emitsEnvelopeType();

AtomType  DebugOperation::collectsEnvelopeType(){
	printf("%s",__FUNCTION__);
	return TYPE_INVALID;
}

AtomType DebugOperation::emitsEnvelopeType(){
	printf("%s",__FUNCTION__);
	return TYPE_INVALID;
}

