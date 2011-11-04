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

#include "FileWriterOperation.h"

using namespace std;

FileWriterOperation::FileWriterOperation()
	:pStream(NULL)
{

}

FileWriterOperation::~FileWriterOperation()
{

}

std::ostream& FileWriterOperation::stream() const
{
	assert(pStream);
	return *(pStream);
}


void FileWriterOperation::start()
{
    cout << "FileWriter::start()" << endl;
	const Configuration &config = configuration();

	if(config.contains("FileWriteOperation"))
	{
		std::string filename = config["FileWriteOperation"]["filename"].asString();
		pStream = new std::ofstream(filename.c_str());
		std::cout << "Writing to file: \"" << filename << "\""<< std::endl;
	}
    Operation::start();
}

void FileWriterOperation::finish()
{
    Operation::finish();
    cout << "FileWriterOperation::finish()" <<endl;
	assert(this->pStream != NULL);
	pStream->close();
	pStream = NULL;
    cout << "File closed" << endl;
    
}

void FileWriterOperation::processEnvelope(const DataEnvelope& envelope)
{
	GCodeData *d = NULL;
	const Configuration &config = configuration();

	cout << endl;
	cout << "----------------" << endl;
	cout << "FileWriterOperation::processEnvelope" << endl;
	cout << "stream @" << pStream << endl;

	if (config["FileWriterOperation"]["format"].asString() == ".gcode")
	{
		cout << "processing DataEnvelope as gcode" << pStream << endl;
		const GCodeData &data = *(dynamic_cast<const GCodeData* > (&envelope) );
		assert(&data != NULL);
		cout << ">> " << data.gString << endl;
		stream() << data.gString;
	}
	else {
		cout << "processing DataEnvelope as raw" << pStream << endl;
	}
	cout << "----------------" << endl << endl;

}



