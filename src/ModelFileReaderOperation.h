/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/
#ifndef MODEL_FILE_READER_H_
#define MODEL_FILE_READER_H_

#include "Operation.h"
#include "PathData.h"

#include <iostream>
#include <fstream>

#include <string>
#include <assert.h>

#include "MeshData.h" // instances of this class generate 3D mesh data


class ModelFileReaderOperation : public Operation
{
	std::ifstream *pStream;

public:
	ModelFileReaderOperation();
	~ModelFileReaderOperation();


	void processEnvelope(const DataEnvelope& envelope);

	void start();

	// closes the file
	void finish();


	// file handle accessor
	std::istream& stream() const;

};




#endif /* MODEL_FILE_READER_H_ */

