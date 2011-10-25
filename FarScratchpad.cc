/*
    MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/
#include <stdio.h>

#include "src/SliceOperation.h"
#include "src/DebugOperation.h"
#include "src/DataEnvelope.h"
#include "src/Operation.h"

int testCallbackCount = 0;
bool testSliceOp = true;

int main() {
	cout << "Hello World!" << endl;
	
	ModelFileReaderOperation readOp = new ModelFileReaderOperation():
	SlicerOperation sliceOp = new SlicerOperation();
	
	readOp.start(); //read the file, and start sending envelopes of data to slicer
	
	}
	
int main_old() {
  // -- Run hello
  printf("Hello world!\n");

  if(testSliceOp)
  {
	printf("%s: Building DataEnvelope(s)\n", __FUNCTION__);
	DataEnvelope de = DataEnvelope();
	DataEnvelope deL = DataEnvelope();

	PathLoadOperation plOp = new PathLoadOperation();
	GCoderOperation gcOp  = new GCodeOperation();

	// create output vector for configurattion
	std::vector<Operation*> plOuts;
	plOuts.append(gcOp);
	std::vector<Operation*> gcOuts;

	Configuration c = new Configuration();
	gcOp = init(c,  /* &inputs,*/ &plOuts);
	gcOp = init(c,  /* &inputs,*/ &gcOuts);
 	
	//This will start the operation chain running
 	PlOp.satrt("test.stl"); 
	

//	  printf("%s: Creating and Slice -> Debug workflow\n", __FUNCTION__);
//	  DebugOperation* dbgOp = new DebugOperation();
//	  SliceOperation* s1 = new SliceOperation();
//	  s1->collect(de);
//
////	  printf("%s: Creating and Slice -> Debug workflow\n", __FUNCTION__);
//	  SliceOperation *s2 = new SliceOperation();
//	  s2->setNext(dbgOp);
////	  s2->collect(de);
////	  deL.setLast();
////	  s2->collect(deL);
//
//	  printf("%s: Testing a slice queuing data, w. callback\n", __FUNCTION__);
//	  DataEnvelope dummyData= DataEnvelope(TYPE_DUMMY_DATA, 0x00,0,(char*)"");
//	  DataEnvelope dummyData2 = DataEnvelope(TYPE_DUMMY_DATA, 0x00,0,(char*)"");
//	  dummyData2.setLast();
//
//	  if (s2->collectsEnvelopeType() == dummyData.typeID)
//	  {
//		  printf("%s: Queuing one Envelope of dummy to s2\n", __FUNCTION__);
//		  s2->collect(dummyData);
//		  s2->collect(dummyData2);
//	  }
////	  else {
////		  printf("%s: Envelope Type Mismatch at s3\n", __FUNCTION__);
////	  }
  }

}


