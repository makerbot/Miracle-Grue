/**
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
#include "src/Configuration.h"
using namespace std;

int testCallbackCount = 0;



bool testSliceOp = true;

int main() {
  // -- Run hello
  printf("Hello world!\n");

  if(testSliceOp)
  {
	  cout << "building base operation test" << endl;
	  SliceOperation sO = SliceOperation();
	  DebugOperation dbgO = DebugOperation();
	  Configuration cfg;

	  cout << "configuring operations into a chain" << endl;
	  sO.setNext(&dbgO);
	  sO.init(cfg);
	  dbgO.init(cfg);

	  cout << "Building a single 'last' DataEnvelope" << endl;
	  DataEnvelope deL = DataEnvelope();
	  deL.setLast();

	  cout << "collecting 'dummy' envelope" << endl;
	  sO.collect(deL);

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

