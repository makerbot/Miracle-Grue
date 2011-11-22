/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/
#include <assert.h>

#include "RegionerOperation.h"

using namespace std;

RegionerOperation::RegionerOperation()
{
	std::cout << __FUNCTION__ << std::endl;
	std::cout << "(Miracle Grue)" << std::endl;
	std::cout << "RegionerOperation() @"  << this<< std::endl;

	//No values to add to requiredConfigRoot;

};

RegionerOperation::~RegionerOperation()
{
	std::cout << "~RegionerOperation() @"  << this<< std::endl;
}

void RegionerOperation::processEnvelope(const DataEnvelope& envelope)
{
	const SliceData &data = *(dynamic_cast<const SliceData* > (&envelope) );
	assert(&data != NULL);

	RegionData *output = new RegionData(42, 3.1415927);
	emit(output);
}

void RegionerOperation::init(Configuration& config,const std::vector<Operation*> &outputs)
{
	bool baseSuccess = Operation::initCommon(config, outputs);
	if(baseSuccess){
	// - Start custom to ExampleOperation code

	// - End custom to ExampleOperation code
	}

}

