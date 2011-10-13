#include "SliceOperation.h"


void SliceOperation::main()
{ printf("%s\n", __FUNCTION__ );}

void SliceOperation::queue(DataEnvelope &pAtom)
{
	printf("%s\n", __FUNCTION__ );
	if( (this->yieldToCallback) == 0x00)
		printf("NOTE: passing atom Fail!\n");
	else{
		printf("NOTE: passing atom blindly!\n");
		(this->*yieldToCallback)(pAtom);
	}
	return;
}

void SliceOperation::setYielder(callback_t  callback){
	this->yieldToCallback = callback;
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
