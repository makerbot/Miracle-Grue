#include <stdio.h>

#include "src/SliceOperation.h"
#include "src/DebugOperation.h"
#include "src/DataEnvelope.h"

int testCallbackCount = 0;

/*DataEnvelope* testCallback(DataEnvelope* DataEnvelope)
{
	printf("%s: round %d\n",__FUNCTION__, testCallbackCount );
	if(DataEnvelope == 0x00){
		printf("%s: No DataEnvelope\n",__FUNCTION__ );
	}
	else {
		printf("%s: Has DataEnvelope, Type ID \n",__FUNCTION__, DataEnvelope->typeID );
	}
	testCallbackCount++;
}*/

bool testSliceOp = true;

int main() {
  // -- Run hello
  printf("Hello world!\n");

  if(testSliceOp)

  {
	  printf("%s: Building DataEnvelope\n", __FUNCTION__);
	  DataEnvelope de = DataEnvelope();

	  DebugOperation dbgOp = DebugOperation(0x00);

	  printf("%s: Testing Callback\n", __FUNCTION__);
	  // call the callback directly to test it
	  //testCallback(test);

	  printf("%s: Creating and running slice, no callback\n", __FUNCTION__);
	  //create a slice operation, not using the callback
	  SliceOperation s1(0x00);
	  s1.queue(de);
	  de.setLast();
	  s1.queue(de);

//	  //create a slice operation, using the callback
//	  printf("%s: Creating and running slice, w. callback\n", __FUNCTION__);
//	  SliceOperation s2((callback_t)testCallback);
//	  s2.queue(0x00);
//
//	  printf("%s: Testing a slice queuing data, w. callback\n", __FUNCTION__);
//	  // Test throwing a file to a slice operation
//	  SliceOperation s3((callback_t)testCallback);
//	  //Open Data File of STL type
//	  //DataEnvelope* stlData = new DataEnvelope(TYPE_DUMMY_DATA, );
//	  DataEnvelope* stlData = new DataEnvelope(TYPE_DUMMY_DATA, 0x00,0,(char*)"");
//	  if (s3.acceptsEnvelopeType() == stlData->typeID)
//	  {
//		  printf("%s: Queuing one Envelope of stl to s3\n", __FUNCTION__);
//		  s3.queue(stlData); //
//	  }
//	  else {
//		  printf("%s: Envelope Type Mismatch at s3\n", __FUNCTION__);
//	  }
  }

}


/*
 * GCoderOperation =
 * PrintController= new PrintController()
 * GCO = new GCoderOperation(PrintController.collect())
 * PO = new PatherOperation(GCO.collect())
   SICe.queue()
 *
 *
 */

