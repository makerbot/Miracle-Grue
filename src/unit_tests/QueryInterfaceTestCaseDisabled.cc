#include <fstream>

#include <cppunit/config/SourcePrefix.h>
#include "QueryInterfaceTestCase.h"

#include "mgl/configuration.h"
//#include "../ModelFileReaderOperation.h"
//#include "../SliceOperation.h"
//#include "../RegionerOperation.h"
//#include "../PatherOperation.h"
//#include "../GCoderOperation.h"
//#include "../FileWriterOperation.h"

#include <json/writer.h>
#include <json/value.h>

CPPUNIT_TEST_SUITE_REGISTRATION( QueryInterfaceTestCase );

using namespace std;

#define SINGLE_EXTRUDER_FILE_NAME "v29_single_xtruder_warmup.gcode"

// for now, use cout, until we add Boost support
// included from DataEnvelope #define BOOST_LOG_TRIVIAL(trace) cout

/// boost log values (future use) are trace/debug/info/warning/error/fatal

//void configureTOM(Configuration& config, bool automaticBuildPlatform, double platformTemp )
//{
//	BOOST_LOG_TRIVIAL(trace)  << "Starting:" <<__FUNCTION__ << endl;
//
//	config["machineName"] = "TOM";
//	config["firmware"] = "v2.9";
//
//	config["platform"]["temperature"] = platformTemp;
//	config["platform"]["automated"] = automaticBuildPlatform;
//	config["platform"]["waitingPositionX"] = 52.0;
//	config["platform"]["waitingPositionY"] = -57.0;
//	config["platform"]["waitingPositionZ"] = 10.0;
//	BOOST_LOG_TRIVIAL(trace)<< "Exiting:" <<__FUNCTION__ << endl;
//
//
//}

// fills a configuration object with the data
// for a single extruder
//void configureExtruder(Configuration& config, double temperature, double speed, double offsetX)
//{
//
//	Json::Value extruder;
//
//	BOOST_LOG_TRIVIAL(trace)<< "Starting:" <<__FUNCTION__ << endl;
//
//	extruder["defaultExtrusionSpeed"]= speed;
//	extruder["extrusionTemperature"] = temperature;
//	extruder["coordinateSystemOffsetX"] = offsetX;
//
//	config["extruders"].append(extruder);
//	BOOST_LOG_TRIVIAL(trace)<< "Exiting:" <<__FUNCTION__ << endl;
//}

//void configureSingleExtruder(Configuration &config)
//{
//	BOOST_LOG_TRIVIAL(trace)<< "Starting:" <<__FUNCTION__ << endl;
//	configureTOM(config, true, 110);
//	configureExtruder(config, 220, 6, 0);
//	BOOST_LOG_TRIVIAL(trace)<< "Exiting:" <<__FUNCTION__ << endl;
//}

// Write test values to a string, check that they match as
// expected
void QueryInterfaceTestCase::simpleInterrogationTest()
{

	//BOOST_LOG_TRIVIAL(trace)<< "Starting:" <<__FUNCTION__ << endl;

	// Get our dictionary of Config Requirements.
	Json::Value* fileWriteRequires = FileWriterOperation::getStaticConfigRequirements();
	cout << "fileWriteRequires" << (*fileWriteRequires)["filename"]<< endl;

	// Verify we have the expecte size
	CPPUNIT_ASSERT_EQUAL((size_t)2, (size_t)fileWriteRequires->size());

	CPPUNIT_ASSERT_EQUAL((string)"asString",
				(string)(*fileWriteRequires)["filename"].asString());
	CPPUNIT_ASSERT_EQUAL((string)"asString",
				(string)(*fileWriteRequires)["format"].asString());
}

void QueryInterfaceTestCase::testConfgurationVsInformation()
{
	// Get our dictionary of Config Requirements.
	Json::Value* fileWriteRequires = FileWriterOperation::getStaticConfigRequirements();
	cout << "fileWriteRequires" << (*fileWriteRequires)["filename"]<< endl;

	FileWriterOperation fwo;
	Configuration cfg;

	cfg["FileWriteOperation"]["filename"] = "outtest";
	cfg["FileWriteOperation"]["format"] = "gcode";
	cout << "has ? " <<  cfg["FileWriteOperation"] << endl;

	std::vector<Operation*> outputs;

//	cout << "****" << endl <<" Re-init case Failure is Expected "<< endl << "****" << endl ;
//	fwo.init(cfg, outputs);

	Configuration cfg2;
	cfg2["GcodeOperation"]["expectToFail"] = true;

	fwo.init(cfg2, outputs);

	FileWriterOperation fwo2;
	fwo2.init(cfg2, outputs);

}


void configurePathTest(Configuration& config)
{
	config["FileWriterOperation"]["filename"]= SINGLE_EXTRUDER_FILE_NAME;
	config["machineName"] = "TOM";
	config["firmware"] ="v2.9";

	Json::Value extruder;
	config["extruders"].append(extruder);
	config["extruders"][0]["defaultExtrusionSpeed"] = 6;
	config["extruders"][0]["extrusionTemperature"] = 200;
	config["extruders"][0]["coordinateSystemOffsetX"] = 0;

	config["platform"]["temperature"] = 30;
}

