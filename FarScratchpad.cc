/*
    MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/
#include <stdio.h>

#include <string.h>

#include "src/SliceOperation.h"
#include "src/DebugOperation.h"
#include "src/DataEnvelope.h"
#include "src/Operation.h"

#include "src/MandTest/MandStlLoaderOperation.h"
#include "src/MandTest/MandCarveOperation.h"
#include "src/MandTest/MandInsetOperation.h"
#include "src/MandTest/MandInfillOperation.h"
#include "src/MandTest/MandWriteSvgOperation.h"


int testCallbackCount = 0;
bool testSliceOp = true;

using namespace std;
using namespace Json;

int main() {
  /// -- Run hello
  printf("Hello world!\n");

  if(testSliceOp)
  {
	  printf("%s: Building DataEnvelope(s)\n", __FUNCTION__);

	  MandStlLoaderOperation* loaderOp = new MandStlLoaderOperation();
	  Json::Value loaderRequires = MandStlLoaderOperation::getStaticConfigRequirements();

	  MandCarveOperation* carveOp = new MandCarveOperation();
	  Json::Value carveRequires = MandCarveOperation::getStaticConfigRequirements();

	  MandInsetOperation* insetOp = new MandInsetOperation();
	  Json::Value insetRequires = MandInsetOperation::getStaticConfigRequirements();

	  MandInfillOperation* infillOp = new MandInfillOperation();
	  Json::Value infillRequires = MandInfillOperation::getStaticConfigRequirements();

	  MandWriteSvgOperation* svgWriteOp= new MandWriteSvgOperation();
	  Json::Value svgWriteRequires = MandInfillOperation::getStaticConfigRequirements();

	  Configuration* cfg = new Configuration();

	  (*cfg)["MandWriteSvgOperation"]["format"] = ".svg";
	  (*cfg)["MandWriteSvgOperation"]["filename"] = "mandStreamTest";

	  std::vector<Operation*> loadOut, carveOut, insetOut, infillOut, svgWriteOut;
	  loadOut.push_back(carveOp);
	  carveOut.push_back(insetOp);
	  insetOut.push_back(infillOp);
	  infillOut.push_back(svgWriteOp);

	  loaderOp->init(*cfg, loadOut);
	  carveOp->init(*cfg, carveOut);
	  insetOp->init(*cfg, insetOut);
	  infillOp->init(*cfg, infillOut);
	  svgWriteOp->init(*cfg, svgWriteOut);

	  loaderOp->start();

	  DataEnvelope* kickstartEnv = new DataEnvelope(/*AtomType*/TYPE_C_ASCII);
	  string sourceFile = ("input.stl");
	  const char* srcFilename = sourceFile .c_str();
	  kickstartEnv->setRawData((void*)srcFilename, strnlen(srcFilename,64) ,false); //allow it to be destroyed as the function exits
	  kickstartEnv->setInitial();

	  //start the chain running
	  loaderOp->accept(*kickstartEnv);

	  loaderOp->finish();

	  loaderOp->deinit();

//
//

  }

}


