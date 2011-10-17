/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/
#include "SliceOperation.h"


DataEnvelope* SliceOperation::processEnvelope(const DataEnvelope& envelope)
{
	printf("%s\n", __FUNCTION__ );
	printf("%s: The processEnvelope! It Does Nothing!!!\n", __FUNCTION__ );
	return 0x00;
}


void SliceOperation::init(Configuration& config)
{
}

void SliceOperation::cleanup()
{
}


string SliceOperation::interrogate() {}

AtomType SliceOperation::collectsEnvelopeType() {
	return TYPE_INVALID;
}

AtomType SliceOperation::emitsEnvelopeType()
{
	return TYPE_INVALID;
}
