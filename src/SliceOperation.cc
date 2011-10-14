#include "SliceOperation.h"



void SliceOperation::collect(const DataEnvelope& dataE)
{
	printf("%s\n", __FUNCTION__ );
	if (this->nextOperation == 0x00 )
		printf("WARNING: no next operation!\n");
	else{
		printf("NOTE: passing atom blindly!\n");
		// always call emit data, even if just with dummy data!
		emitData(dataE);
	}

	return;
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
