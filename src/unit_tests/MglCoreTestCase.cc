#include <fstream>

#include <cstdlib>

#include <cppunit/config/SourcePrefix.h>

#include "UnitTestUtils.h"
#include "MglCoreTestCase.h"

#include "mgl/abstractable.h"
#include "mgl/meshy.h"
#include "mgl/configuration.h"
#include "mgl/gcoder.h"

using namespace std;
using namespace mgl;


string testCaseInputsDir("test_cases/mglCoreTestCase/inputs/");


CPPUNIT_TEST_SUITE_REGISTRATION( MglCoreTestCase );
#include <float.h>
#include <bits/basic_string.h>

string outputDir("outputs/test_cases/mglCoreTestCase/");

void MglCoreTestCase::setUp()
{
	std::cout<< " Starting:" <<__FUNCTION__ << endl;
	MyComputer computer;
	char pathsep = computer.fileSystem.getPathSeparatorCharacter();
	outputDir = string("outputs") + pathsep + string("test_cases ") + 
			pathsep + string("mglCoreTestCase") + pathsep;
	computer.fileSystem.guarenteeDirectoryExistsRecursive(outputDir.c_str());
}

void throwsException(){
	Exception e("foo");
	throw e;
}

void MglCoreTestCase::testExceptions() {
	CPPUNIT_ASSERT_THROW(throwsException(), Exception);
}


void  MglCoreTestCase::testScalarBasics(){
}

void  MglCoreTestCase::testScalarMaths(){
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

	// Note the behavior of tequals vs. SCALAR_EPSILON
	s1 = 0, s2 = SCALAR_EPSILON;
	t1 = SCALAR_EPSILON;
	CPPUNIT_ASSERT( tequals(s1, s2, t1) == false);

}

void  MglCoreTestCase::testPoint2TypeBasics()
{
	Point2Type foo;
	cout << "Does default constructor initialize to 0?" << endl;
	CPPUNIT_ASSERT( foo.x == 0);
	CPPUNIT_ASSERT( foo.y == 0);
	cout << "Yes!" << endl;
	
	Point2Type bar(10,100);
	cout << "Does the constructor set the values correctly?" << endl;
	CPPUNIT_ASSERT( bar[0] == 10);
	CPPUNIT_ASSERT( bar[1] == 100);
	cout << "Yes!" << endl;
	cout << "Does the bracket operator fail correctly?" << endl;
	CPPUNIT_ASSERT_THROW(bar[2], Exception);
	cout << "Yes!" << endl;

	bar[0] = 20; bar[1] = 200;
	cout << "Does the bracket operator allow assignment?" << endl;
	CPPUNIT_ASSERT( bar[0] == 20);
	CPPUNIT_ASSERT( bar[1] == 200);
	cout << "Yes!" << endl;
	cout << "Does the bracket operator assignment fail correctly?" << endl;
	CPPUNIT_ASSERT_THROW(bar[2] = 2000, Exception);
	cout << "Yes!" << endl;

}


void  MglCoreTestCase::Point2TypeMaths()
{
	Point2Type v0(0,0),v1(0,1),v2(1,0), v3(0,1);
	Point2Type v4(0,0.04);

	Scalar r0 = AreaSign(v0,v1,v2);
	CPPUNIT_ASSERT(r0 < 0 );
	cout << "foo " << r0 << endl;

	r0 = AreaSign(v2,v1,v0);
	CPPUNIT_ASSERT(r0 > 0 );
	cout << "foo " << r0 << endl;

	r0 = AreaSign(v2,v1,v4);
	CPPUNIT_ASSERT(r0 > 0 );
	cout << "foo " << r0 << endl;

	//	r1 = AreaSign(v0,v1,v3);
//	cout << "foo " << r1 << endl;

//	Scalar r2 = AreaSign(v0,v2,v3);
//	cout << "foo " << r2 << endl;
}


void MglCoreTestCase::testLineSegment2dBasics()
{
	Segment2Type seg1;
	CPPUNIT_ASSERT( seg1.a.x == 0);
	CPPUNIT_ASSERT( seg1.a.y == 0);
	CPPUNIT_ASSERT( seg1.b.x == 0);
	CPPUNIT_ASSERT( seg1.b.y == 0);

	Point2Type v1(10,20), v2(100,200);
	Segment2Type seg2(v1, v2);
	CPPUNIT_ASSERT( seg2.a.x == 10);
	CPPUNIT_ASSERT( seg2.a.y == 20);
	CPPUNIT_ASSERT( seg2.b.x == 100);
	CPPUNIT_ASSERT( seg2.b.y == 200);

	Segment2Type seg3 = seg2;
	CPPUNIT_ASSERT( (void*)&seg2 != (void*)&seg3);
	CPPUNIT_ASSERT( (void*)&seg2.a != (void*)&seg3.a);
	CPPUNIT_ASSERT( (void*)&seg2.a.x != (void*)&seg3.a.x);

}

void MglCoreTestCase::testVector3Basics()
{
	Point3Type foo;
	CPPUNIT_ASSERT( foo.x == 0);
	CPPUNIT_ASSERT( foo.y == 0);
	CPPUNIT_ASSERT( foo.z == 0);
	Point3Type bar = foo;
	CPPUNIT_ASSERT( (void*)&bar != (void*)&foo);
	CPPUNIT_ASSERT( (void*)&bar.x != (void*)&foo.x);

	Point3Type baz(10, 40, 50 );
	CPPUNIT_ASSERT( baz[0] == 10);
	CPPUNIT_ASSERT( baz[1] == 40);
	CPPUNIT_ASSERT( baz[2] == 50);
	CPPUNIT_ASSERT_THROW(baz[3] = 20, Exception);

	baz[0] = 100; baz[1] = 400, baz[2] = 500;
	CPPUNIT_ASSERT( baz.x == 100);
	CPPUNIT_ASSERT( baz.y == 400);
	CPPUNIT_ASSERT( baz.z == 500);

	//verify Vector3.tequals
	Point3Type v0(0,0,0);
	Point3Type v1(0,0,SCALAR_EPSILON*2);
	CPPUNIT_ASSERT( v0.tequals(v1,SCALAR_EPSILON ) == false );
	Point3Type v2(0,0,0);
	Point3Type v3(0,0,SCALAR_EPSILON/2);
	CPPUNIT_ASSERT( v2.tequals(v3,SCALAR_EPSILON ) == true );

}

void MglCoreTestCase::testTriangle3Maths() {

	//Vector3 v0, v1, v2;
	Point3Type v0(0,0,0);
	Point3Type v1(0,0,0);
	Point3Type v2(0,0,0);
	Triangle3Type foo(v0,v1,v2);

	Point3Type x1= foo[0];
	Point3Type x2= foo[1];
	Point3Type x3= foo[2];
	CPPUNIT_ASSERT( x1.tequals(foo[0],SCALAR_EPSILON ) );
	CPPUNIT_ASSERT( x2.tequals(foo[1],SCALAR_EPSILON ) );
	CPPUNIT_ASSERT( x3.tequals(foo[2],SCALAR_EPSILON ) );

	Point3Type value = foo.normal();
	CPPUNIT_ASSERT( v0.tequals(value,SCALAR_EPSILON ) );

	Point3Type v3(0,0,0);
	Point3Type v4(0,0,0);
	Point3Type v5(0,0,0);
	Triangle3Type foo2(v0,v1,v2);
}



void MglCoreTestCase::testTequalsPolygon()
{
	Point2Type v0(0,0),v1(0,1),v2(1,0), v3(0,1);
	Point2Type v4(0,0.04);

	mgl::Polygon p1,p2,p3;
	p1.push_back(v0);	p1.push_back(v1);	p1.push_back(v2);
	p2.push_back(v0);	p2.push_back(v1);	p2.push_back(v2);
	p3.push_back(v0);
	// sizes mismatch
	bool same = tequalsPolygonCompare(p1,p3, SCALAR_EPSILON);
	CPPUNIT_ASSERT(same == false);
	// same exact poly
	same = tequalsPolygonCompare(p1,p1, SCALAR_EPSILON);
	CPPUNIT_ASSERT(same == true);
	same = tequalsPolygonCompare(p1,p2, SCALAR_EPSILON);
	CPPUNIT_ASSERT(same == true);

	Point2Type v5(0,0),v6(0,1),v7(1,0), v8(SCALAR_EPSILON*2,1);
	mgl::Polygon p4;
	p4.push_back(v5);	p4.push_back(v6);	p2.push_back(v7);
	same = tequalsPolygonCompare(p1,p4, SCALAR_EPSILON);
	CPPUNIT_ASSERT(same == false);
}

/// Test tequals for polygons function
void MglCoreTestCase::testTequalsPolygons()
{
	Point2Type v0(0,0),v1(0,1),v2(1,0), v3(0,1);
	Point2Type v4(0,0.04);

	mgl::Polygon p1,p2,p3;
	p1.push_back(v0);	p1.push_back(v1);	p1.push_back(v2);
	p2.push_back(v1);	p2.push_back(v1);	p2.push_back(v2);

	Polygons polys1,polys2, polys3,polys4;
	polys1.push_back(p1);
	polys1.push_back(p2);

	polys2.push_back(p1);
	polys2.push_back(p2);

	polys3.push_back(p1);

	polys4.push_back(p2);
	polys4.push_back(p1);

	bool same = false;
	//exactly the same poly
	same = tequalsPolygonsCompare(polys1,polys1, SCALAR_EPSILON);
	CPPUNIT_ASSERT(same == true);

	//different size polys
	same = tequalsPolygonsCompare(polys1,polys3, SCALAR_EPSILON);
	CPPUNIT_ASSERT(same == false);

	// polys with same data
	same = tequalsPolygonsCompare(polys1,polys2, SCALAR_EPSILON);
	CPPUNIT_ASSERT(same == true);

	//polys with different data
	same = tequalsPolygonsCompare(polys1,polys4, SCALAR_EPSILON);
	CPPUNIT_ASSERT(same == false );
}



void MglCoreTestCase::testMeshyLoads()
{

	string binaryStl = testCaseInputsDir +"linkCup.stl";
	cout << "Test: " << __FUNCTION__ << endl;
	Meshy mesh;
	size_t loadSize = mesh.readStlFile(binaryStl.c_str());
	cout << binaryStl << " : face count=" << loadSize << endl;
	CPPUNIT_ASSERT_EQUAL((size_t) 32816, loadSize);

	string asciiStl = testCaseInputsDir +"3D_Knot.stl";
	cout << "Test: " << __FUNCTION__ << endl;
	Meshy mesh2;
	loadSize = mesh2.readStlFile(asciiStl.c_str());
	cout << asciiStl <<" : face count=" << loadSize << endl;
	CPPUNIT_ASSERT_EQUAL((size_t)2892, loadSize);

}


void MglCoreTestCase::testSliceDataConstructorDestructor()
{

}
