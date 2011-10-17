/*
 * DebugOperation.cc
 *
 *  Created on: Oct 13, 2011
 *      Author: farmckon
 */
#include "DebugOperation.h"

void DebugOperation::init(Configuration& config) {}


DataEnvelope* DebugOperation::processEnvelope(const DataEnvelope& envelope)
{
	printf("%s",__FUNCTION__);
	printf("Beans!\n");
	if(envelope.lastFlag == true)
	{
		printf(" last operation!\n");
	}
	return 0x00;
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

