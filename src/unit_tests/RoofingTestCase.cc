#include <cppunit/config/SourcePrefix.h>
#include "UnitTestUtils.h"

#include "RoofingTestCase.h"

#include "UnitTestUtils.h"
#include "mgl/miracle.h"


CPPUNIT_TEST_SUITE_REGISTRATION( RoofingTestCase );

using namespace std;
using namespace mgl;

MyComputer computer;

string outputDir ("outputs/test_cases/roofingTestCase/");

string inputDir("./test_cases/roofingTestCase/stls/");


void RoofingTestCase::setUp()
{
	MyComputer computer;
	mkDebugPath(outputDir.c_str());

}

void RoofingTestCase::testSimple()
{
	cout<<endl;

	cout << "HO!" << endl;
}
