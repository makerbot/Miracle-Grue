#include <fstream>

#include <cppunit/config/SourcePrefix.h>
#include "GCoderTestCase.h"

#include "../Configuration.h"
#include "../GCoderOperation.h"
#include "../FileWriterOperation.h"

#include "../PathData.h"
#include "../GCodeEnvelope.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( GCoderTestCase );

#define SINGLE_EXTRUDER_FILE_NAME "test_cases/GCoderTestCase/output/single_xtruder_warmup.gcode"
#define DUAL_EXTRUDER_FILE_NAME "test_cases/GCoderTestCase/output/dual_xtruder_warmup.gcode"
#define SINGLE_EXTRUDER_WITH_PATH "test_cases/GCoderTestCase/output/single_xtruder_with_path.gcode"

// for now, use cout, until we add Boost support
//#define BOOST_LOG_TRIVIAL(trace) cout
//#define BOOST_LOG_TRIVIAL(debug) cout
//#define BOOST_LOG_TRIVIAL(info) cout
//#define BOOST_LOG_TRIVIAL(warning) cout
//#define BOOST_LOG_TRIVIAL(error) cout
//#define BOOST_LOG_TRIVIAL(fatal) cout

void configurePlatform(Configuration& config, bool automaticBuildPlatform, double platformTemp )
{
	BOOST_LOG_TRIVIAL(trace)  << "Starting:" <<__FUNCTION__ << endl;

	config["scalingFactor"] = 1.0;
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

	extruder["defaultExtrusionSpeed"] = speed;
	extruder["extrusionTemperature"] = temperature;
	extruder["coordinateSystemOffsetX"] = offsetX;
	extruder["slowFeedRate"] = 1000;
	extruder["slowExtrusionSpeed"] = 1.0;
	extruder["fastFeedRate"] = 3000;
	extruder["fastExtrusionSpeed"] = 2.682;
	extruder["nozzleZ"] = 0.2;
	config["extruders"].append(extruder);
	BOOST_LOG_TRIVIAL(trace)<< "Exiting:" <<__FUNCTION__ << endl;
}

void configureSingleExtruder(Configuration &config)
{
	BOOST_LOG_TRIVIAL(trace)<< "Starting:" <<__FUNCTION__ << endl;
	configurePlatform(config, true, 110);
	configureExtruder(config, 220, 6, 0);
	BOOST_LOG_TRIVIAL(trace)<< "Exiting:" <<__FUNCTION__ << endl;
}

// fills a configuration object with data for 2 extruders
void configureDualExtruder(Configuration& config)
{
	BOOST_LOG_TRIVIAL(trace)<< "Starting:" <<__FUNCTION__ << endl;
	configurePlatform(config, true, 110)	;
	configureExtruder(config, 220, 6, 0);
	configureExtruder(config, 220, 6, 0);
	BOOST_LOG_TRIVIAL(trace)<< "Exiting:" <<__FUNCTION__ << endl;
}




void GCoderTestCase::setUp()
{
	BOOST_LOG_TRIVIAL(trace)<< " Starting:" <<__FUNCTION__ << endl;
	BOOST_LOG_TRIVIAL(trace)<< " Exiting:" <<__FUNCTION__ << endl;
}


void run_tool_chain(Configuration &config, DataEnvelope* envelope = NULL)
{

	BOOST_LOG_TRIVIAL(trace)<< "get Config static requirements:" <<__FUNCTION__ << endl;
	/// 1) (Optional)  call getStaticConfigRequirements, to make sure you can build a
	/// good configuration.
	Json::Value* gCoderRequires= GCoderOperation::getStaticConfigRequirements();
	Json::Value* fileWriterRequires= FileWriterOperation::getStaticConfigRequirements();
	assert( (void*)gCoderRequires != NULL);
	assert( (void*)fileWriterRequires != NULL);

	/// 2)  Build an instance of the object. This builds member functions, allocates much space etc
	BOOST_LOG_TRIVIAL(trace)<< "Build Operation Instances:" <<__FUNCTION__ << endl;
	GCoderOperation &tooler = *new GCoderOperation();
	FileWriterOperation &fileWriter = *new FileWriterOperation();

	///3) Create Output Vector(s) from each operation (not always required)
	BOOST_LOG_TRIVIAL(trace)<< "Build Output Vectors:" <<__FUNCTION__ << endl;
	vector<Operation*> empty;
	vector<Operation*> toolerOutputs;
	toolerOutputs.push_back(&fileWriter);

	///4) Build a Configure object. use the staticConfigRequirements to help you unless you know
	/// exactly what to build
	BOOST_LOG_TRIVIAL(trace)<< "Build Output Vectors:" <<__FUNCTION__ << endl;
	Configuration cfg;
	//	cfg.root["FileWriterOperation"]["prefix"] = Value("tester");
	//	cfg.root["FileWriterOperation"]["lang"] = Value("eng");


	///5) initalize the Object with your configuration, and your output list
	BOOST_LOG_TRIVIAL(trace)<< "Initalizing Operations:" <<__FUNCTION__ << endl;
	tooler.init(config,  toolerOutputs);
	fileWriter.init(config, empty);

	/// 6) Send a start signal to the first operation in the Operation Graph
	tooler.start();

	///7) Send inital one or more data envelopes to the object.
	if(envelope != NULL)
	{
		BOOST_LOG_TRIVIAL(trace)<< "Accept Envelope @" << envelope <<" "<<__FUNCTION__ << endl;
		tooler.accept(*envelope);
	}
	else {
		// BOOST_LOG_TRIVIAL(trace)<< "No Envelope accepted this test:" <<__FUNCTION__ << endl;
	}

	/// 8) Send a finish signal to the first operation in the Operation Graph
	/// that call to finish will propagate down the graph automatically
	tooler.finish();

	//9) De-init (for safty)
	tooler.deinit();
	fileWriter.deinit();

	delete &tooler;
	delete &fileWriter;

	BOOST_LOG_TRIVIAL(trace)<< "Exiting:" <<__FUNCTION__ << endl;
}



// a function that adds 4 points to a polygon within the list paths for
// a new extruder.
void initSimplePath(PathData &d)
{
	BOOST_LOG_TRIVIAL(trace)<< "Starting:" <<__FUNCTION__ << endl;
	d.paths.push_back(Paths());
	d.paths[0].push_back(Polygon());
	Polygon &poly = d.paths[0][0];

	double lower_x = -10;
	double lower_y = -10;
	double dx = 50;
	double dy = 30;


	Point2D p0(lower_x, lower_y);
	Point2D p1(p0.x, p0.y + dy);
	Point2D p2(p1.x + dx, p1.y);
	Point2D p3(p2.x, p2.y - dy);

	poly.push_back(p0);
	poly.push_back(p1);
	poly.push_back(p2);
	poly.push_back(p3);

	BOOST_LOG_TRIVIAL(trace)<< "Exiting:" <<__FUNCTION__ << endl;
}

//
// This test creates a gcode file for single extruder machine
// The file contains code to home the tool and heat the extruder/platform
//
void GCoderTestCase::testSingleExtruder()
{
	BOOST_LOG_TRIVIAL(trace)<< "Starting:" <<__FUNCTION__ << endl;
	Configuration config;
	config["FileWriterOperation"]["filename"] = SINGLE_EXTRUDER_FILE_NAME;
	config["FileWriterOperation"]["format"]= ".gcode";

	configureSingleExtruder(config);
//	CPPUNIT_ASSERT_EQUAL((size_t)1, config.extruders.size());


	Json::StyledWriter w;
	string confstr = w.write(config.root);
	cout << confstr << endl;

	run_tool_chain(config);
	// verify that gcode file has been generated
	CPPUNIT_ASSERT( ifstream(SINGLE_EXTRUDER_FILE_NAME) );
	BOOST_LOG_TRIVIAL(trace)<< "Exiting:" <<__FUNCTION__ << endl;
}

//
// This test creates a gcode file for a dual extruder machine
//
void GCoderTestCase::testDualExtruders()
{
	BOOST_LOG_TRIVIAL(trace)<< "Starting:" <<__FUNCTION__ << endl;
	// cerate an empty configuration object
	Configuration config;
	// set the output fie
	config["FileWriterOperation"]["filename"]= DUAL_EXTRUDER_FILE_NAME;
	config["FileWriterOperation"]["format"]= ".gcode";

	// add extruder information
	configureDualExtruder(config);
//	CPPUNIT_ASSERT_EQUAL((size_t)2,config.extruders.size());
	// create a simple Gcode operation (no paths), initialize it and run it
	run_tool_chain(config);

	CPPUNIT_ASSERT( ifstream(DUAL_EXTRUDER_FILE_NAME) );

	BOOST_LOG_TRIVIAL(trace)<< "Exiting:" <<__FUNCTION__ << endl;
}


//
// 	This tests generates gcode for a simple rectangular path.
//
void GCoderTestCase::testSimplePath()
{
	BOOST_LOG_TRIVIAL(trace)<< "Starting:" <<__FUNCTION__ << endl;
	// create empty configuration and set the file name
	Configuration config;
	config["FileWriterOperation"]["filename"] = SINGLE_EXTRUDER_WITH_PATH;
	config["FileWriterOperation"]["format"]= ".gcode";

	// load 1 extruder
	configureSingleExtruder(config);
	//	CPPUNIT_ASSERT_EQUAL((size_t)1, config.extruders.size());

	// create a path message as if received by a pather operation
	PathData *path = new PathData(0.2, 0.4);
	// add a simple rectangular path for the single extruder
	initSimplePath(*path);

	// instaniate a gcoder and send it the path as an envelope.
	run_tool_chain(config, path);

	path->release();
	// cleanup the data


	// verify that gcode file has been generated
	CPPUNIT_ASSERT( ifstream(SINGLE_EXTRUDER_WITH_PATH) );
	BOOST_LOG_TRIVIAL(trace)<< "Exiting:" <<__FUNCTION__ << endl;
}


void GCoderTestCase::testConfig()
{
	BOOST_LOG_TRIVIAL(trace)<< "Starting:" <<__FUNCTION__ << endl;

	Configuration conf;

	std::string p = conf.root["programName"].asString();
	cout << endl << endl << endl << "PROGRAM NAME: " << p << endl;
	CPPUNIT_ASSERT(p == "Miracle-Grue");

	configureSingleExtruder(conf);
	Json::StyledWriter w;
	string confstr = w.write(conf.root);
	cout << confstr << endl;


	CPPUNIT_ASSERT(conf.root["extruders"].isArray());
	CPPUNIT_ASSERT(conf.root["extruders"].isValidIndex(0));

	cout << "ExtruderCount " << conf.root["extruders"].size() << endl;

	GCoderConfig single;
	single.loadData(conf);

	cout << endl << endl << endl << "READ!" << endl;

	CPPUNIT_ASSERT(single.extruders.size() ==1);

	BOOST_LOG_TRIVIAL(trace)<< "Exiting:" <<__FUNCTION__ << endl;
}

void gcodeStreamFormat(ostream &ss)
{
    try
    {
    	ss.imbue(std::locale("en_US.UTF-8"));
    }
    catch(...)
    {
    	ss.imbue(std::locale("C"));
    }
    ss.setf(ios_base::floatfield, ios::floatfield);            // floatfield not set
	ss.precision(4);
    
}


void GCoderTestCase::testFloatFormat()
{
	stringstream ss;
	gcodeStreamFormat(ss);
    
	ss << endl;
	ss << "loc: " << ss.getloc().name() << endl;

	CPPUNIT_ASSERT_EQUAL(ss.getloc().name(), string("en_US.UTF-8")); // std::string("C") );

	//locale myloc(  locale(),    // C++ default locale
    //       new WithComma);// Own numeric facet
	ss << endl;
	// ss << "LOCALE name: " << myloc.name() << endl;
	ss << "num: " << 3.1415927 << endl;
	cout << ss.str() << endl;
	BOOST_LOG_TRIVIAL(trace)<< "Exiting:" <<__FUNCTION__ << endl;
    
}
