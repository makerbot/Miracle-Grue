#include <fstream>

#include <cppunit/config/SourcePrefix.h>
#include "ConfigTestCase.h"

#include "../Configuration.h"
#include "../ModelFileReaderOperation.h"
#include "../SliceOperation.h"
#include "../RegionerOperation.h"
#include "../PatherOperation.h"
#include "../GCoderOperation.h"
#include "../FileWriterOperation.h"

#include "../json-cpp/include/json/writer.h"

CPPUNIT_TEST_SUITE_REGISTRATION( ConfigTestCase );

using namespace std;

#define SINGLE_EXTRUDER_FILE_NAME "v29_single_xtruder_warmup.gcode"

// for now, use cout, until we add Boost support
// included from DataEnvelope #define BOOST_LOG_TRIVIAL(trace) cout

/// boost log values (future use) are trace/debug/info/warning/error/fatal

void configureTOM(Configuration& config, bool automaticBuildPlatform, double platformTemp )
{
	BOOST_LOG_TRIVIAL(trace)  << "Starting:" <<__FUNCTION__ << endl;

	config["machineName"] = "TOM";
	config["firmware"] = "v2.9";

	config["platform"]["temperature"] = platformTemp;
	config["platform"]["automated"] = automaticBuildPlatform;
	config["platform"]["waitingPositionX"] = 52.0;
	config["platform"]["waitingPositionY"] = -57.0;
	config["platform"]["waitingPositionZ"] = 10.0;
	BOOST_LOG_TRIVIAL(trace)<< "Exiting:" <<__FUNCTION__ << endl;


}

// fills a configuration object with the data
// for a single extruder
void configureExtruder(Configuration& config, double temperature, double speed, double offsetX)
{

	Json::Value extruder;

	BOOST_LOG_TRIVIAL(trace)<< "Starting:" <<__FUNCTION__ << endl;

	extruder["defaultExtrusionSpeed"]= speed;
	extruder["extrusionTemperature"] = temperature;
	extruder["coordinateSystemOffsetX"] = offsetX;

	config["extruders"].append(extruder);
	BOOST_LOG_TRIVIAL(trace)<< "Exiting:" <<__FUNCTION__ << endl;
}

void configureSingleExtruder(Configuration &config)
{
	BOOST_LOG_TRIVIAL(trace)<< "Starting:" <<__FUNCTION__ << endl;
	configureTOM(config, true, 110);
	configureExtruder(config, 220, 6, 0);
	BOOST_LOG_TRIVIAL(trace)<< "Exiting:" <<__FUNCTION__ << endl;
}

// Write test values to a string, check that they match as
// expected
void ConfigTestCase::configWrite()
{

	BOOST_LOG_TRIVIAL(trace)<< "Starting:" <<__FUNCTION__ << endl;
	Configuration config;


	configureSingleExtruder(config);
	CPPUNIT_ASSERT_EQUAL((size_t)1, (size_t)config["extruders"].size());

	Json::Value extruder;
	config["extruders"].append(extruder);

	config["extruders"][0]["defaultExtrusionSpeed"] = 6;
	config["extruders"][0]["extrusionTemperature"]= 200;
	config["extruders"][0]["coordinateSystemOffsetX"] = 0;


	string s = config.asJson();
	BOOST_LOG_TRIVIAL(trace) << "json" << s.c_str() << endl;
	BOOST_LOG_TRIVIAL(trace)<< "Exiting:" <<__FUNCTION__ << endl;
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

