/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/

#include <assert.h>
#include "SliceOperation.h"



using namespace std;

SliceOperation::SliceOperation()
{
	cout << "SliceOperation() @"  << this<< endl;

    this->acceptTypes.append(TYPE_MESH_3D);
    this->emitTypes.append(TYPE_SHELL_BINARY);
}

SliceOperation::~SliceOperation()
{
	cout << "~SliceOperation() @"  << this<< endl;
}


void SliceOperation::processEnvelope(const DataEnvelope& envelope)
{

	printf("%s\n", __FUNCTION__ );
	const MeshData &data = *(dynamic_cast<const MeshData* > (&envelope) );
	assert(&data != NULL);
}






