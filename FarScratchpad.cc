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

	//create our operations.
	// Loader -> Slicer -> Regioner -> Pather -> GCoder
	PathLoadOperation plOp = new PathLoadOperation();
	SliceOperation slOp  = new SliceOperation();
	RegionerOperation regOp = new RegionerOperation();	
	PatherOperation pathOp = new PatherOperation();
	GCodeOperation gcodeOp = new GCodeOperation();
	
	//pather output list
	std::vector<Operation*> plOuts;
	plOuts.append(slOp);

	//slicer output list
	std::vector<Operation*> slOuts;
	slOuts.append(reOp);

	//regioner output list
	std::vector<Operation*> regOuts;
	regOuts.append(pathOp);

	//pather output list
	std::vector<Operation*> pathOuts;
	pathOuts.append(gcodeOp);


	/// creating our path of processing.
	Configuration c = new Configuration();
	plOp.init(c,  /* &inputs,*/ &plOuts);
	slOp.init(c,  /* &inputs,*/ &slOuts);
	regOp.init(c,  /* &inputs,*/ &regOuts);
	pathOp.init(c,  /* &inputs,*/ &pathOuts);
	gcodeOp.init(c,  /* &inputs,*/ &gcodeOuts);
 	
	//This will start the operation chain running
 	PlOp.satrt("test.stl"); 
 	
	
  }

}


