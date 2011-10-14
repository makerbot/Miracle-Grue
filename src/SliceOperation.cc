#include "SliceOperation.h"


void SliceOperation::main()
{ printf("%s\n", __FUNCTION__ );}

void SliceOperation::collect(DataEnvelope& dataE)
{
	printf("%s\n", __FUNCTION__ );
	if (this->nextOperation == 0x00 )
		printf("WARNING: no next operation!\n");
	else{
		printf("NOTE: passing atom blindly!\n");
		this->nextOperation->collect(dataE);
//		(this->*yieldToCallback)(dataE);
	}
	return;
}


void SliceOperation::cleanup()
{	printf("%s\n", __FUNCTION__ );}


std::string SliceOperation::interrogate()
{	printf("%s\n", __FUNCTION__ );return (char*)"INTERROGATE!";}


uint32_t SliceOperation::acceptsEnvelopeType()
{
	printf("%s\n", __FUNCTION__ );
	return TYPE_DUMMY_DATA;
}


uint32_t  SliceOperation::yieldsEnvelopeType()
{
	printf("%s\n", __FUNCTION__ );
	return TYPE_DUMMY_DATA;
}
