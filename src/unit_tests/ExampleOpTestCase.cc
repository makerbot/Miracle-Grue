#include <fstream>
#include <iterator>
#include <string.h>

#include <cppunit/config/SourcePrefix.h>
#include "ExampleOpTestCase.h"

#include "../ExampleOperation.h" //#includes "../Configuration.h"

//#include "../SliceOperation.h"
//#include "../RegionerOperation.h"
//#include "../PatherOperation.h"
//#include "../GCoderOperation.h"
//#include "../ExampleOperation.h"

#include "../json-cpp/include/json/writer.h"

using namespace std;

using namespace Json;

static string testCaseSourceDir("./test_cases/ExampleOpTestCase/input/");
static string tastCaseOutputDir("./test_cases/ExampleOpTestCase/output/");

CPPUNIT_TEST_SUITE_REGISTRATION( ExampleOpTestCase );



void ExampleOpTestCase::create_ExampleOp()
{
	BOOST_LOG_TRIVIAL(trace) << endl << "Starting: " <<__FUNCTION__ << endl;

	/// Minimum use example

	/// 1) (Optional)  call getStaticConfigRequirements, to make sure you can build a
	/// good configuration.
	Json::Value* configRequirements = ExampleOperation::getStaticConfigRequirements();
	assert( (void*)configRequirements != NULL);

	/// 2)  Build an instance of the object. This builds member functions, allocates much space etc
	ExampleOperation exampleOp;

	///3) Create Output Vector(s) from each operation (not always required)
	/// no output operation for this object!
	std::vector<Operation*> zeroOutputOperations;

	///4) Build a Configure object. use the staticConfigRequirements to help you unless you know
	/// exactly what to build
	Configuration cfg;
	cfg.root["ExampleOperation"]["prefix"] = Value("tester");
	cfg.root["ExampleOperation"]["lang"] = Value("eng");

	/// (if cfg < configRequirements)
		///cout << "an error" << endl;

	///5) initalize the Object with your configuration, and your output list
	exampleOp.init(cfg, zeroOutputOperations);


	/// 6) Send a start signal to the first operation in the Operation Graph,
	/// that call to start will propigate down the graph automatically
	exampleOp.start();

	///7) Send inital one or more data envelopes to the object. The zeroth envelope
	///must be flagged as 'stream start' and the last envelope flagged as 'stream end'
	///(If there is only one envelope, those flags can both be set in that envelope
	///BOOST_LOG_TRIVIAL(trace)<< "Ending: " <<__FUNCTION__ << endl;
	uint32_t* data = new uint32_t;
	data[0] = (uint32_t)32;
	DataEnvelope testEnv(TYPE_INVALID);
	testEnv.setFinal();
	testEnv.setInitial();
	testEnv.setRawData(data, 1,true);
	exampleOp.accept(testEnv);

	testEnv.release(); // we are releasing our envelope.

	/// 8) Send a finish signal to the first operation in the Operation Graph
	/// that call to finish will propagate down the graph automatically
	exampleOp.finish();



	///9) (Optional) de-init the object. If this is not done, it happens
	///automatically when the object is destroyed (or, in the future, when
	///'init' is called a 2nd time
	exampleOp.deinit();

	// 8) (implied) exampleOp is destroyed as this funciton exits
}


void ExampleOpTestCase::createNoDenit_ExampleOp()
{
	BOOST_LOG_TRIVIAL(trace) << endl << "Starting: " <<__FUNCTION__ << endl;

	/// Minimum use example

	/// 1) (Optional)  call getStaticConfigRequirements, to make sure you can build a
	/// good configuration.
	Json::Value* configRequirements = ExampleOperation::getStaticConfigRequirements();
	assert( (void*)configRequirements != NULL);

	/// 2)  Build an instance of the object. This builds member functions, allocates much space etc
	ExampleOperation exampleOp;

	///3) Create Output Operation(s) to hook up to (not always required)
	/// no output operation for this object!
	std::vector<Operation*> zeroOutputOperations;

	///4) Build a Configure object. use the staticConfigRequirements to help you unless you know
	/// exactly what to build
	Configuration cfg;
	cfg.root["ExampleOperation"]["prefix"] = Value("tester");
	cfg.root["ExampleOperation"]["lang"] = Value("eng");

	///5) initalize the Object with your configuration, and your output list
	exampleOp.init(cfg, zeroOutputOperations);

	///6) Send inital one or more data envelopes to the object. The zeroth envelope
	///must be flagged as 'stream start' and the last envelope flagged as 'stream end'
	///(If there is only one envelope, those flags can both be set in that envelope
	///BOOST_LOG_TRIVIAL(trace)<< "Ending: " <<__FUNCTION__ << endl;
	uint32_t* data = new uint32_t;
	data[0] = (uint32_t)32;
	DataEnvelope testEnv(TYPE_INVALID);
	testEnv.setFinal();
	testEnv.setInitial();
	testEnv.setRawData(data, 1,true);
	exampleOp.accept(testEnv);

	// 7/8) Let deinit happen as a side effect of object destruction
	//exampleOp.deinit();

	// 8) (implied) exampleOp is destroyed as this funciton exits
}



void ExampleOpTestCase::createNoStreamEnd_ExampleOp()
{
	BOOST_LOG_TRIVIAL(trace) << endl << "Starting: " <<__FUNCTION__ << endl;

	/// Minimum use example

	/// 1) (Optional)  call getStaticConfigRequirements, to make sure you can build a
	/// good configuration.
	Json::Value* configRequirements = ExampleOperation::getStaticConfigRequirements();
	assert( (void*)configRequirements != NULL);

	/// 2)  Build an instance of the object. This builds member functions, allocates much space etc
	ExampleOperation exampleOp;

	///3) Create Output Operation(s) to hook up to (not always required)
	/// no output operation for this object!
	std::vector<Operation*> zeroOutputOperations;

	///4) Build a Configure object. use the staticConfigRequirements to help you unless you know
	/// exactly what to build
	Configuration cfg;
	cfg.root["ExampleOperation"]["prefix"] = Value("tester");
	cfg.root["ExampleOperation"]["lang"] = Value("eng");

	///5) initalize the Object with your configuration, and your output list
	exampleOp.init(cfg, zeroOutputOperations);

	///6) Send inital one or more data envelopes to the object. The zeroth envelope
	///must be flagged as 'stream start' and the last envelope flagged as 'stream end'
	///(If there is only one envelope, those flags can both be set in that envelope
	///BOOST_LOG_TRIVIAL(trace)<< "Ending: " <<__FUNCTION__ << endl;
	uint32_t* data = new uint32_t;
	data[0] = (uint32_t)32;
	DataEnvelope testEnv(TYPE_INVALID);
	//testEnv.setFinal();
	testEnv.setInitial();
	testEnv.setRawData(data, 1,true);
	exampleOp.accept(testEnv);

	// 7/8) Let deinit happen as a side effect of object destruction
	// also, we never send a final packet, auto de-init will do that
	// as it works
	//exampleOp.deinit();

	// 8) (implied) exampleOp is destroyed as this funciton exits
}


