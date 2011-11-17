/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/
#include <assert.h>
#include <sstream>

#include "ModelFileReaderOperation.h"

using namespace std;

ModelFileReaderOperation::ModelFileReaderOperation()
	:pStream(NULL)
{
	cout << "ModelFileReaderOperation() @"  << this<< endl;
	//requiredConfigRoot["filename"] = "asString";
}


ModelFileReaderOperation::~ModelFileReaderOperation()
{
	cout << "~ModelFileReaderOperation() @"  << this<< endl;
}

void ModelFileReaderOperation::processEnvelope(const DataEnvelope& envelope)
{
	// this method must exist because it is declared pure virutalin Operation
	// however the ModelFileReader does not receive any data so...
//	assert(0); // this method should not be called

}

std::istream& ModelFileReaderOperation::stream() const
{
	assert(pStream);
	return *(pStream);
}


void ModelFileReaderOperation::start()
{
    
	const Configuration &config = configuration();
//	pStream = new std::ifstream("filename.stl");
//	if( config.contains("ModelFileReaderOperation") )
//	{
//		string file = config["ModelFileReaderOperation"]["filename"].asString();
//		std::cout << "ModelFileReaderOperation reading file: \"" << file << "\""<< std::endl;
//	}
//    finish();
}

void ModelFileReaderOperation::finish()
{
	// assert(pStream);
//	pStream->close();
//	pStream = NULL;
}

void ModelFileReaderOperation::init(Configuration& config,const std::vector<Operation*> &outputs)
{
	bool baseSuccess = Operation::initCommon(config, outputs);
	if(baseSuccess){
	// - Start custom to ExampleOperation code

	// - End custom to ExampleOperation code
	}

}



