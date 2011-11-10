#include <fstream>
#include <iterator>
#include <string.h>

#include <cppunit/config/SourcePrefix.h>
#include "FileWriterTestCase.h"

#include "../FileWriterOperation.h" //#includes "../Configuration.h"

//#include "../SliceOperation.h"
//#include "../RegionerOperation.h"
//#include "../PatherOperation.h"
//#include "../GCoderOperation.h"
//#include "../FileWriterOperation.h"

#include "../json-cpp/include/json/writer.h"

using namespace std;

using namespace Json;

static string testCaseSourceDir("./test_cases/fileWriterTestCase/input/");
static string tastCaseOutputDir("./test_cases/fileWriterTestCase/output/");

CPPUNIT_TEST_SUITE_REGISTRATION( FileWriterTestCase );



void FileWriterTestCase::create_FileWriter()
{
	//BOOST_LOG_TRIVIAL(trace) << endl << "Starting: " <<__FUNCTION__ << endl;
	Json::Value* configRequirements = FileWriterOperation::getStaticConfigRequirements();
	assert( (void*)configRequirements != NULL);
	FileWriterOperation fwo;

	Json::Value* val2 = fwo.getStaticConfigRequirements();
	assert( (void*)val2 != NULL);

	//BOOST_LOG_TRIVIAL(trace)<< "Ending: " <<__FUNCTION__ << endl << endl;

}



void FileWriterTestCase::createConfigRunDestroy_FileWriter()
{
	/// Minimum use example

	/// 1) (Optional)  call getStaticConfigRequirements, to make sure you can build a
	/// good configuration.
	Json::Value* configRequirements = FileWriterOperation::getStaticConfigRequirements();
	assert( (void*)configRequirements != NULL);

	/// 2)  Build an instance of the object. This builds member functions, allocates much space etc
	FileWriterOperation fwo;

	///3) Create Output Operation(s) to hook up to (not always required)
	/// no output operation for this object!
	std::vector<Operation*> zeroOutputOperations;

	///4) Build a Configure object. use the staticConfigRequirements to help you unless you know
	/// exactly what to build
	Configuration cfg;

	cfg.root["FileWriterOperation"]["filename"] = Value("outputTest.raw");
	cfg.root["FileWriterOperation"]["format"] = Value(".uint32_t");

	///5) initalize the Object with your configuration, and your output list
	fwo.init(cfg, zeroOutputOperations);

	///6) Send inital one or more data envelopes to the object. The zeroth envelope
	///must be flagged as 'stream start' and the last envelope flagged as 'stream end'
	///(If there is only one envelope, those flags can both be set in that envelope
	///BOOST_LOG_TRIVIAL(trace)<< "Ending: " <<__FUNCTION__ << endl;
	uint32_t* data = new uint32_t;
	data[0] = (uint32_t)32;
	DataEnvelope testEnv;
	testEnv.setFinal();
	testEnv.setInitial();
	testEnv.setRawData(data, 1,true);
	fwo.accept(testEnv);

	///7) (Optional) de-init the object. If this is not done, it happens
	///automatically when the object is destroyed (or, in the future, when
	///'init' is called a 2nd time
	fwo.deinit();

	//TODO: test file is closed.
}

void put_into_vector( ifstream& ifs, vector<string>& v )
{
	istream_iterator<string> front(ifs);
	istream_iterator<string> back;
	string tempString;
	while( getline(ifs, tempString))
	//for (istream_iterator<string> i = front; i != back; ++i)
	    v.push_back(tempString);

}


void FileWriterTestCase::streamFileTo_FileWriter()
{
	//0 get data to stream as a vector
	vector<string> gcodeLines;

	string textStreamInputFile = testCaseSourceDir + "textStreamInput.txt";

	ifstream ifs( textStreamInputFile.c_str() );
	put_into_vector( ifs, gcodeLines );


	/// 1) (Optional)  call getStaticConfigRequirements, to make sure you can build a
	/// good configuration.
	Json::Value* configRequirements = FileWriterOperation::getStaticConfigRequirements();
	assert( (void*)configRequirements != NULL);

	/// 2)  Build an instance of the object. This builds member functions, allocates much space etc
	FileWriterOperation fwo;

	///3) Create Output Operation(s) to hook up to (not always required)
	/// no output operation for this object!
	std::vector<Operation*> zeroOutputOperations;

	///4) Build a Configure object. use the staticConfigRequirements to help you unless you know
	/// exactly what to build
	Configuration cfg;
	string testOutputFile = tastCaseOutputDir + "textStreamOutput.txt";

	cfg.root["FileWriterOperation"]["filename"] = Value(testOutputFile);
	cfg.root["FileWriterOperation"]["format"] = Value(".txt");

	cfg.root["FileWriterOperation"]["in"]["AtomType"] = Value("c_ascii");
	//cfg.root["FileWriterOperation"]["in"]["AtomType"] = Value("c_ascii");
	//cfg.root["FileWriterOperation"]["in"]["AtomType"] = Value("gcode");

	cfg.root["FileWriterOperation"]["out"] = Value(); //null value, no output

	///5) initalize the Object with your configuration, and your output list
	fwo.init(cfg, zeroOutputOperations);

	///6) Send inital one or more data envelopes to the object. The zeroth envelope
	///must be flagged as 'stream start' and the last envelope flagged as 'stream end'
	///(If there is only one envelope, those flags can both be set in that envelope
	///BOOST_LOG_TRIVIAL(trace)<< "Ending: " <<__FUNCTION__ << endl;
	vector<string>::iterator it;
	for ( it=gcodeLines.begin() ; it < gcodeLines.end(); it++ ) {
		//cout << *it << endl;
		DataEnvelope* testEnv = new DataEnvelope(/*AtomType*/TYPE_C_ASCII);
		testEnv->setFinal();
		testEnv->setInitial();
		string s = *it;
		const char* data = s.c_str();
		testEnv->setRawData((void*)data, strnlen(data,2048),false); //allow it to be destroyed as the function exits
		fwo.accept(*testEnv);

		// In this case, we are manually managing DataEnvelope existance
		testEnv->release(this);
		delete testEnv;
  }

	///7) (Optional) de-init the object. If this is not done, it happens
	///automatically when the object is destroyed (or, in the future, when
	///'init' is called a 2nd time
	fwo.deinit();

	///TRICKY: configRequirements is a global static. To make memtests not leak, we force
	//delete this. Inmost cases, this is not needed
	delete configRequirements;
}



