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

#include <float.h>

void MglCoreTestCase::setUp()
{
	std::cout<< " Starting:" <<__FUNCTION__ << endl;
	std::cout<< " Exiting:" <<__FUNCTION__ << endl;
}

void throwsException(){
	Exception e("foo");
	throw e;
}

void MglCoreTestCase::ExceptionTest() {
	CPPUNIT_ASSERT_THROW(throwsException(), Exception);
}


void  MglCoreTestCase::ScalarBasics(){
}

void  MglCoreTestCase::ScalarMaths(){
	/** basic math (assuming it works)*/

	/** complex math **/

	/** helper functions **/
	Scalar s1 = 0, s2 = 0;
	CPPUNIT_ASSERT( tequals(s1, s1, 0.2) == true);
	CPPUNIT_ASSERT( tequals(s1, s1, 1) == true);
	s1 = 10, s2 = 10.1;
	CPPUNIT_ASSERT( tequals(s1, s2, 0.05) == false);
	CPPUNIT_ASSERT( tequals(s1, s2, 1) == true);
	s1 = 0, s2 = SCALAR_EPSILON;
	CPPUNIT_ASSERT( tequals(s1, s2, 0) == false);
	s1 = 0, s2 = 0;
	Scalar t1 = SCALAR_EPSILON;
	CPPUNIT_ASSERT( tequals(s1, s2, t1) == true);

	// Note the behavior of tequals vs. SCALAR_EPSILON
	s1 = 0, s2 = SCALAR_EPSILON;
	CPPUNIT_ASSERT( tequals(s1, s2, 0) == false);
}

void  MglCoreTestCase::Vector2Basics()
{
	Vector2 foo;
	CPPUNIT_ASSERT( foo.x == 0);
	CPPUNIT_ASSERT( foo.y == 0);

	Vector2 bar(10,100);
	CPPUNIT_ASSERT( bar[0] == 10);
	CPPUNIT_ASSERT( bar[1] == 100);
	CPPUNIT_ASSERT_THROW(bar[2], Exception);

	bar[0] = 20; bar[1] = 200;
	CPPUNIT_ASSERT( bar[0] == 20);
	CPPUNIT_ASSERT( bar[1] == 200);
	CPPUNIT_ASSERT_THROW(bar[2] = 2000, Exception);

}


void  MglCoreTestCase::Vector2Maths()
{

}

