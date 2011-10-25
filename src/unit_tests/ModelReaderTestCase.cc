#include <fstream>

#include <cppunit/config/SourcePrefix.h>
#include "ModelReaderTestCase.h"

#include "../Configuration.h"
#include "../ModelFileReaderOperation.h"


CPPUNIT_TEST_SUITE_REGISTRATION( ModelReaderTestCase );


using namespace std;


void ModelReaderTestCase::setUp()
{

}

void ModelReaderTestCase::example()
{

	cout << endl;
	MeshData data;
	ModelFileReaderOperation operation;

//  CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, 1.1, 0.05 );
//  CPPUNIT_ASSERT( 1 == 0 );
	CPPUNIT_ASSERT( 1 == 1 );

	long* l1 = new long(12);
	long* l2 = new long(12);

	CPPUNIT_ASSERT_EQUAL( 12, 12 );
	CPPUNIT_ASSERT_EQUAL( 12L, 12L );
	CPPUNIT_ASSERT_EQUAL( *l1, *l2 );

	delete l1;
	delete l2;

	CPPUNIT_ASSERT( 12L == 12L );
//  CPPUNIT_ASSERT_EQUAL( 12, 13 );
//  CPPUNIT_ASSERT_DOUBLES_EQUAL( 12.0, 11.99, 0.5 );

}




