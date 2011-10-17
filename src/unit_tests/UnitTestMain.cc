#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>


int main( int argc, char* argv[] ){
  // Create the event manager and test controller
  CPPUNIT_NS::TestResult controller;

  // Add a listener that colllects test result
  CPPUNIT_NS::TestResultCollector result;
  controller.addListener( &result );

  // Add a listener that print dots as test run.
  CPPUNIT_NS::BriefTestProgressListener progress;
  controller.addListener( &progress );

  // Add the top suite to the test runner
  CPPUNIT_NS::TestRunner runner;
  runner.addTest( CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest() );
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write();

  return result.wasSuccessful() ? 0 : 1;
}


/*


// *
 // * gcoder_test_main.cc
 // *
 // *  Created on: Oct 14, 2011
 // *      Author: farmckon
 // *

#include "src/GCoderOperation.h"
#include "src/DataEnvelope.h"

Configuration *pConfig  = new Configuration();

Configuration &readFakeConfigFile()
{
	// assert(pConfig == NULL);

	pConfig->machineName = "Special";
	pConfig->firmware = "v2.9";

	Extruder e;
	e.defaultSpeed =1.98;

	pConfig->extruders.push_back(e);
	pConfig->extruders.push_back(e);

	pConfig->extruders[0].extrusionTemperature = 225;
	pConfig->extruders[0].coordinateSystemOffsetX = -16.5;

	pConfig->extruders[1].extrusionTemperature = 210;
	pConfig->extruders[0].coordinateSystemOffsetX = 16.5;

	return *pConfig;
}

int main()
{

	Configuration &cfg = readFakeConfigFile();

	GCoderOperation tooler = GCoderOperation();

	tooler.init(cfg);
	tooler.start();

	DataEnvelope d = DataEnvelope();
	d.typeID =  TYPE_ASCII_PATHER;
	d.setLast();
	tooler.collect(d);

	delete pConfig;

//	op.init(gcodeOutStream);
	return -1;
}


*/
