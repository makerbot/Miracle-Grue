/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

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

