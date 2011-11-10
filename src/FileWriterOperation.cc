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

#include "json-cpp/include/json/value.h"

using namespace std;
using namespace Json;


/// Must be named <OperationName>ConfigRequirements
static Value* FileWriterOperationConfigRequirements;


Value* FileWriterOperation::getStaticConfigRequirements()
{
	if (FileWriterOperationConfigRequirements == 0x00)
	{
		// NOTE: things like file types accepted are defined elsewhere.
		// this is only required config values to run this, not all metadata
		Value* cfg = new Value;
		( *cfg )["filename"]= "asString";
		( *cfg )["format"] = "asString";
		FileWriterOperationConfigRequirements = cfg;
		// This object is expected to live until the program dies.
		// No deconstruction !
	}
	return FileWriterOperationConfigRequirements;
}


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


bool FileWriterOperation::isValidConfig(Configuration& config) const
{

	cout << __FUNCTION__ << endl;

	if(config["FileWriterOperation"].type() !=  /*ValueType.*/objectValue)
	{
		//TODO: do item by item compare
		// against configRequirements later.
		return true;
	}
	cout << "ERROR: configuration is not valid, In BETA accepting config anyway" << endl;
	return true;

}


void FileWriterOperation::init(Configuration& config,const std::vector<Operation*> &outputs)
{
	//For Alpha version, pConfig must be null,
	//ie we can't re-configure an object once it's been configured
	assert(this->pConfig == NULL);

	if(isValidConfig(config)){
		//std::cout <<  "setting config " << endl;
		//std::cout <<  "Fop->fname " << config["FileWriterOperation"]["filename"].asString() <<endl;
		//std::cout <<  "Fop->fmt " <<  config["FileWriterOperation"]["format"].asString() <<endl;
		this->pConfig = &config;
		string filename = config["FileWriterOperation"]["filename"].asString();
		pStream = new std::ofstream(filename.c_str());
		//std::cout << "Writing to file: \"" << filename << "\""<< std::endl;

		//set our accepts type(s)
		//TODO: convert to walking a list of objects
		if (  0 ==
				config["FileWriterOperation"]["in"]["AtomType"].asString().compare("raw") )
		{
			cout << "accepts raw" <<endl;
			acceptTypes.push_back(/*AtomType*/TYPE_INT32);
		}
		if (  0 ==
				config["FileWriterOperation"]["in"]["AtomType"].asString().compare("c_ascii") )
		{
			cout << "accepts ascii" <<endl;
			acceptTypes.push_back(/*AtomType*/TYPE_C_ASCII);
		}

	}
	else {
		cout << "configuration does not contain FileWriterOperation section" << endl;
		assert(0);
	}
}


void FileWriterOperation::deinit()
{
    //cout << __FUNCTION__ <<endl;
	assert(this->pStream != NULL);
	pStream->close();
	delete pStream;
	pStream = NULL;

	assert(this->pConfig != NULL);
	pConfig = NULL;

    //cout << __FUNCTION__ << " done" << endl;
}


void FileWriterOperation::processEnvelope(const DataEnvelope& envelope)
{

//	cout << endl;
//	cout << "----------------" << endl;
//	cout << "FileWriterOperation::processEnvelope" << endl;
//	cout << "stream @" << pStream << endl;
//	cout << "cfg" << (*pConfig)["FileWriterOperation"] << endl;
//	cout << "type" << envelope.getAtomType() << endl;

	/// if we are outputting gcode, make it so
	if ((*pConfig)["FileWriterOperation"]["format"].asString() == ".gcode")
	{
		GCodeEnvelope *data = NULL;

//		cout << "processing DataEnvelope to output gcode to " << pStream << endl;
		if(envelope.getAtomType() == TYPE_ASCII_GCODE ) {
//			cout << "converting ASCII GCODE to gcode obj" << endl;
			data = (GCodeEnvelope*) (dynamic_cast<const GCodeEnvelope* > (&envelope) );
		} else {
			cout << "no known conversion from AtomType " << envelope.getAtomType() <<
					" to output format .gcode " << endl;
			goto return_clean;
		}
		assert(&data != NULL);
//		cout << "Getting stream" << endl;
		stream() << data->gString;
	}
	/// if we are outputting to an unit32_t text file, make it so
	else if ((*pConfig)["FileWriterOperation"]["format"].asString() == ".uint32_t")
	{
		/// if we are outputting unit_32 values,
		if(envelope.getAtomType() == TYPE_INT32 ) {
			uint32_t* rawPtr = (uint32_t*)envelope.getRawPtr();
			size_t dataSz = envelope.getRawSize();
			for( uint32_t* p = rawPtr; p < (rawPtr + dataSz); p++)
				stream() << (uint32_t) *p << endl ;
		}
		else {
			cout << "no known conversion from AtomType " << envelope.getAtomType() <<
					" to output format .uint32_t" << endl;
		}
	}
	else if ((*pConfig)["FileWriterOperation"]["format"].asString() == ".txt")
	{
		if(envelope.getAtomType() == TYPE_C_ASCII ) {
			char* rawPtr = (char*)envelope.getRawPtr();
			size_t dataSz = envelope.getRawSize();
			for( char* p = rawPtr; p < (rawPtr + dataSz); p++)
				stream() << (char) *p;
			stream() << endl ;
		}
		else {
			cout << "no known conversion from AtomType " << envelope.getAtomType() <<
					" to output format .txt" << endl;
		}
	}
	/// otherwise, err out
	else {
		cout << "output type " << (*pConfig)["FileWriterOperation"]["format"].asString();
		cout << " is currently not handled (DataEnvelope is "<<  envelope.getAtomType() <<") " << endl;
	}
//	cout << "----------------" << endl << endl;
return_clean:
	envelope.release(); //this Op does not call 'emit' so we must release in our envelope process
	return;
}



