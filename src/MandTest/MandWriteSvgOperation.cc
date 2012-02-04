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

#include "MandWriteSvgOperation.h"

#include "../json-cpp/include/json/value.h"
#include "../BGL/BGLCompoundRegion.h"
#include "../BGL/BGLSVG.h"
#include "../MandTest/RegionEnvelope.h"


using namespace std;
using namespace Json;
using namespace BGL;

/// Must be named <OperationName>ConfigRequirements
static Value* MandWriteSvgOperationConfigRequirements;


Value* MandWriteSvgOperation::getStaticConfigRequirements()
{
	if (MandWriteSvgOperationConfigRequirements == 0x00)
	{
		// NOTE: things like file types accepted are defined elsewhere.
		// this is only required config values to run this, not all metadata
		Value* cfg = new Value;
		( *cfg )["filename"]= "asString";
		( *cfg )["format"] = "asString";
		MandWriteSvgOperationConfigRequirements = cfg;
		// This object is expected to live until the program dies.
		// No deconstruction !
	}
	return MandWriteSvgOperationConfigRequirements;
}


MandWriteSvgOperation::MandWriteSvgOperation()
	:pStream(NULL)
{
	this->acceptTypes.push_back(/*AtomType*/TYPE_BGL_FILLED);
	//this->emitTypes.push_back();// this emits no data type
}


MandWriteSvgOperation::~MandWriteSvgOperation()
{

}


std::ostream& MandWriteSvgOperation::stream() const
{
	assert(pStream);
	return *(pStream);
}


bool MandWriteSvgOperation::isValidConfig(Configuration& config) const
{

	cout << __FUNCTION__ << endl;

	if(config["MandWriteSvgOperation"].type() !=  /*ValueType.*/objectValue)
	{
		//TODO: do item by item compare
		// against configRequirements later.
		return true;
	}
	cout << "ERROR: configuration is not valid, In BETA accepting config anyway" << endl;
	return true;

}


void MandWriteSvgOperation::start()
{
	Operation::startCommon();

}

void MandWriteSvgOperation::finish()
{
	Operation::finishCommon();
}


void MandWriteSvgOperation::init(Configuration& config,const std::vector<Operation*> &outputs)
{
	//For Alpha version, pConfig must be null,
	//ie we can't re-configure an object once it's been configured
	assert(this->pConfig == NULL);

	bool configOk = Operation::initCommon(config, outputs);
	if( configOk ){

		string filename = config["MandWriteSvgOperation"]["filename"].asString();
		pStream = new std::ofstream(filename.c_str());
		//std::cout << "Writing to file: \"" << filename << "\""<< std::endl;

		//set our accepts type(s)
		//TODO: convert to walking a list of objects
		if (  0 ==
				config["MandWriteSvgOperation"]["in"]["AtomType"].asString().compare("raw") )
		{
			cout << "accepts raw" <<endl;
			acceptTypes.push_back(/*AtomType*/TYPE_INT32);
		}
		if (  0 ==
				config["MandWriteSvgOperation"]["in"]["AtomType"].asString().compare("c_ascii") )
		{
			cout << "accepts ascii" <<endl;
			acceptTypes.push_back(/*AtomType*/TYPE_C_ASCII);
		}

	}
	else {
		cout << "configuration does not contain MandWriteSvgOperation section" << endl;
		assert(0);
	}
}


void MandWriteSvgOperation::deinit()
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

void svgPathWithSizeAndOffset(
		ostream &os,
		RegionEnvelope& envelope,
		float width, float height, float dx, float dy, float mmStrokeWidth)
{
    BGL::SVG svg(width, height);
    svg.header(os);

	//cout << __FUNCTION__ << endl;

    CompoundRegions::iterator rit;
    for (rit = envelope.shells.begin(); rit != envelope.shells.end(); rit++) {
	os << "<path class=\"inset_shell\" fill=\"none\" stroke=\"black\"";
	os << " stroke-width=\"" << (mmStrokeWidth/3.937) << "mm\"";
	os << " d=\"" << rit->svgPathWithOffset(dx,dy) << "\" />" << endl;
    }

    BGL::Paths::const_iterator pit;
    for (pit = envelope.infill.begin(); pit != envelope.infill.end(); pit++) {
    	os << "<path class=\"infill_path\" fill=\"none\" stroke=\"blue\"";
    	os << " stroke-width=\"" << (mmStrokeWidth/3.937) << "mm\"";
    	os << " d=\"" << pit->svgPathWithOffset(dx,dy) << "\" />" << endl;
    }

    os << "<path class=\"model_outline\" fill=\"none\" stroke=\"red\"";
    os << " stroke-width=\"" << (mmStrokeWidth/3.937) << "mm\"";

    os << " d=\"" << envelope.perimeter.svgPathWithOffset(dx,dy) << "\" />" << endl;

    svg.footer(os);
}


void MandWriteSvgOperation::processEnvelope(const DataEnvelope& envelope)
{
//	cout << "----------------" << endl;
//	cout << "MandWriteSvgOperation::processEnvelope" << endl;
//	cout << "stream @" << pStream << endl;
//	cout << "cfg" << (*pConfig)["MandWriteSvgOperation"] << endl;
//	cout << "type" << envelope.getAtomType() << endl;

	/// if we are outputting gcode, make it so
	if ((*pConfig)["MandWriteSvgOperation"]["format"].asString() == ".svg")
	{
//		cout << "MandWriteSvgOperation format SVG" << endl;

		string dumpPrefix = (*pConfig)["MandWriteSvgOperation"]["filename"].asString();
		string filetype  = (*pConfig)["MandWriteSvgOperation"]["format"].asString();
	    RegionEnvelope& env = (RegionEnvelope&)(dynamic_cast<const RegionEnvelope&>(envelope));
	    char dumpFileName[512];
	    double zLayer = env.zLayer;
	    snprintf(dumpFileName, sizeof(dumpFileName), "%.128s-%06.2f%s",
	    		dumpPrefix.c_str(), zLayer, filetype.c_str());
		cout << "MandWriteSvgOperation to file "<< dumpFileName << endl;

	    fstream fout;
	    fout.open(dumpFileName, fstream::out | fstream::trunc);
	    if (!fout.good()) {
			cout << "MandWriteSvgOperation fout no good" << endl;
	        return;
	    }
	    svgPathWithSizeAndOffset(fout,  env,
	    		env.svgWidth, env.svgHeight,
				env.svgXOff, env.svgYOff, env.width  );
	    fout.close();


////		cout << "processing DataEnvelope to output gcode to " << pStream << endl;
//		if(envelope.getAtomType() == TYPE_ASCII_GCODE ) {
////			cout << "converting ASCII GCODE to gcode obj" << endl;
//			data = (GCodeEnvelope*) (dynamic_cast<const GCodeEnvelope* > (&envelope) );
//		} else {
//			cout << "no known conversion from AtomType " << envelope.getAtomType() <<
//					" to output format .gcode " << endl;
//			goto return_clean;
//		}
//		assert(&data != NULL);
////		cout << "Getting stream" << endl;
//		stream() << data->gString;
	}
//	/// if we are outputting to an unit32_t text file, make it so
//	else if ((*pConfig)["MandWriteSvgOperation"]["format"].asString() == ".uint32_t")
//	{
//		/// if we are outputting unit_32 values,
//		if(envelope.getAtomType() == TYPE_INT32 ) {
//			uint32_t* rawPtr = (uint32_t*)envelope.getRawPtr();
//			size_t dataSz = envelope.getRawSize();
//			for( uint32_t* p = rawPtr; p < (rawPtr + dataSz); p++)
//				stream() << (uint32_t) *p << endl ;
//		}
//		else {
//			cout << "no known conversion from AtomType " << envelope.getAtomType() <<
//					" to output format .uint32_t" << endl;
//		}
//	}
//	else if ((*pConfig)["MandWriteSvgOperation"]["format"].asString() == ".txt")
//	{
//		if(envelope.getAtomType() == TYPE_C_ASCII ) {
//			char* rawPtr = (char*)envelope.getRawPtr();
//			size_t dataSz = envelope.getRawSize();
//			for( char* p = rawPtr; p < (rawPtr + dataSz); p++)
//				stream() << (char) *p;
//			stream() << endl ;
//		}
//		else {
//			cout << "no known conversion from AtomType " << envelope.getAtomType() <<
//					" to output format .txt" << endl;
//		}
//	}
//	/// otherwise, err out
//	else {
//		cout << "output type " << (*pConfig)["MandWriteSvgOperation"]["format"].asString();
//		cout << " is currently not handled (DataEnvelope is "<<  envelope.getAtomType() <<") " << endl;
//	}
////	cout << "----------------" << endl << endl;
//return_clean:
//	envelope.release(); //this Op does not call 'emit' so we must release in our envelope process
//	return;
}



