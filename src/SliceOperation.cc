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
