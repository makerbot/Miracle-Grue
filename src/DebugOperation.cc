/*
 * DebugOperation.cc
 *
 *  Created on: Oct 13, 2011
 *      Author: farmckon
 */
#include "DebugOperation.h"


void DebugOperation::main()
{
	printf("%s",__FUNCTION__);
}
void DebugOperation::collect(DataEnvelope &data){
	printf("%s",__FUNCTION__);
	printf("Beans!\n");
	if(data.lastFlag == true)
	{
		printf(" last operation!\n");
	}
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

uint32_t DebugOperation::acceptsEnvelopeType(){
	printf("%s",__FUNCTION__);
	return 0;
}
uint32_t  DebugOperation::yieldsEnvelopeType(){
	printf("%s",__FUNCTION__);
	return 0;
}
