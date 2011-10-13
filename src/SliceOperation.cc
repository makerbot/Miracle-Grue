#include "SliceOperation.h"


void SliceOperation::main()
{ printf("%s\n", __FUNCTION__ );}

void SliceOperation::queue(DataEnvelope* pAtom)
{
	printf("%s\n", __FUNCTION__ );
	if ( this->yeildToCallback != 0x00){
		printf("has yield to callback\n");
		this->yeildToCallback(0x00);
	}
		printf("does not have yield to callback\n");
	return;
}

void SliceOperation::cleanup()
{ printf("__FUNCTION__");}

char* SliceOperation::interrogate()
{printf("__FUNCTION__");return (char*)"__FUNCTION__";}
