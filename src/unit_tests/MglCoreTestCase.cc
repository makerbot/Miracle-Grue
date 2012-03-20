#include <fstream>

#include <cstdlib>

#include <cppunit/config/SourcePrefix.h>
#include "MglCoreTestCase.h"


#include "mgl/abstractable.h"
#include "mgl/meshy.h"
#include "mgl/configuration.h"
#include "mgl/gcoder.h"

using namespace std;
using namespace mgl;



CPPUNIT_TEST_SUITE_REGISTRATION( MglCoreTestCase );


void MglCoreTestCase::setUp()
{
	std::cout<< " Starting:" <<__FUNCTION__ << endl;
	std::cout<< " Exiting:" <<__FUNCTION__ << endl;
}
