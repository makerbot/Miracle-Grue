#include <fstream>
#include <cstdlib>

#include <cppunit/config/SourcePrefix.h>

#include "UnitTestUtils.h"

#include "JsonConverterTestCase.h"


#include "mgl/abstractable.h"
#include "mgl/meshy.h"
#include "mgl/configuration.h"
#include "mgl/gcoder.h"

#include <stdexcept>

using namespace std;
using namespace mgl;
using namespace Json;
using namespace libthing;

string testCaseInputsDir("test_cases/jsonConverterTestCase/");
string testCaseOutputsDir("outputs/test_cases/jsonConverterTestCase/inputs/");


CPPUNIT_TEST_SUITE_REGISTRATION( JsonConverterTestCase );
#include <float.h>

string outputDir("outputs/test_cases/JsonConverterTestCase/");


void JsonConverterTestCase::setUp()
{
	std::cout<< " Starting:" <<__FUNCTION__ << endl;
	MyComputer computer;
	char pathsep = computer.fileSystem.getPathSeparatorCharacter();
	testCaseInputsDir = string("test_cases") + pathsep + 
			string("jsconConverterTestCase") + pathsep;
	testCaseOutputsDir = string("outputs") + pathsep + 
			string("test_cases") + pathsep + 
			string("jsonConverterTestCase") + pathsep + 
			string("inputs") + pathsep;
	computer.fileSystem.guarenteeDirectoryExistsRecursive(
			testCaseInputsDir.c_str());
	computer.fileSystem.guarenteeDirectoryExistsRecursive(
			testCaseOutputsDir.c_str());
	mkDebugPath(outputDir.c_str());
}


void JsonConverterTestCase::testScalarToJson()
{
	JsonConverter converter;
	Scalar scalar = 10;
	Value value;
	Value expected(10.0);
	bool ok = converter.loadJsonFromScalar(value, scalar);
	CPPUNIT_ASSERT(ok == true);
	CPPUNIT_ASSERT( value == expected);
}


void JsonConverterTestCase::testJsonToScalar()
{
	JsonConverter converter;
	Scalar scalar = 0;
	Value value(10);
	Scalar expected = 10;
	bool ok = converter.loadScalarFromJson(scalar,value );
	CPPUNIT_ASSERT(ok == true);
	CPPUNIT_ASSERT( scalar == expected);
}


void JsonConverterTestCase::testVector2ToJson()
{
	JsonConverter converter;
	Vector2 vector(0,100);
	Value value;
	Value exp0(0.0),exp1(100.0);
	bool ok = converter.loadJsonFromVector2(value, vector);
	CPPUNIT_ASSERT(ok == true);
	cout << value << endl;

	Value v0 = value.get(ArrayIndex(0), value);
	Value v1 = value.get(ArrayIndex(1), value);
	CPPUNIT_ASSERT( v0 == exp0);
	CPPUNIT_ASSERT( v1 == exp1);

}

void fillJsonWithVector2(Json::Value& value, Scalar base1, Scalar base2)
{
	value.append(Json::Value(base1));
	value.append(Json::Value(base2));
}

void JsonConverterTestCase::testJsonToVector2()
{
	JsonConverter converter;
	Json::Value value;
	Vector2 expected(5,17),converted;
	value.append(Json::Value(5.0));
	value.append(Json::Value(17.0));

	bool ok = converter.loadVector2FromJson(converted, value);
	CPPUNIT_ASSERT(ok == true);
	CPPUNIT_ASSERT(expected.tequals(converted,SCALAR_EPSILON) == true);

	Json::Value value2;
	Vector2 expected2(SCALAR_EPSILON*5,SCALAR_EPSILON*17),converted2;
	value2.append(Json::Value(SCALAR_EPSILON*5.0));
	value2.append(Json::Value(SCALAR_EPSILON*17.0));
	ok = converter.loadVector2FromJson(converted2, value2);
	CPPUNIT_ASSERT(ok == true);
	CPPUNIT_ASSERT(expected2.tequals(converted2,SCALAR_EPSILON) == true);
	CPPUNIT_ASSERT(expected2.tequals(converted2,0) == false); ///too accurate, should fail
}


void fillJsonWithPolygon(Json::Value& value,
		Scalar base1, Scalar base2)
{
	Json::Value v0, v1,v2;
	fillJsonWithVector2(v0, base1, base2);
	fillJsonWithVector2(v1, base1, 1+base2);
	fillJsonWithVector2(v2, 1+base1, 1+base2);
	value.append(v0);
	value.append(v1);
	value.append(v2);
}

bool fillPolygon(Polygon& poly, int baseX=0, int baseY=0, int delta=1)
{
	Vector2 v0(baseX+0,baseY+0),v1(baseX+0,baseY+1),v2(baseX+1,baseY+1);
	poly.push_back(v0); poly.push_back(v1); poly.push_back(v2);
	return true;
}



void JsonConverterTestCase::testPolygonToJson()
{
	JsonConverter converter;

	Polygon polyTest;
	fillPolygon(polyTest);
	Value value;
	//cout << value << endl;
	bool ok = converter.loadJsonFromPolygon(value, polyTest);
	CPPUNIT_ASSERT(ok == true);

	// verify it was built correctly
	Value exp;
	Json::Value t0,t1,t2;
	t0.append(Json::Value(0.0)); t0.append(Json::Value(0.0));
	exp.append(t0);
	t1.append(Json::Value(0.0)); t1.append(Json::Value(1.0));
	exp.append(t1);
	t2.append(Json::Value(1.0)); t2.append(Json::Value(1.0));
	exp.append(t2);
	CPPUNIT_ASSERT(exp == value );
}

void JsonConverterTestCase::testJsonToPolygon( )
{
	JsonConverter converter;
	Json::Value v0,v1;
	Polygon expected, converted0,converted1;
	fillJsonWithPolygon(v0,0.0,0.0);
	fillJsonWithPolygon(v1,0.2,1.0);
	bool ok = fillPolygon(expected);
	CPPUNIT_ASSERT(ok == true);

	ok = converter.loadPolygonFromJson(converted0,v0);
	CPPUNIT_ASSERT(ok == true);

	ok = mgl::tequalsPolygonCompare(expected,converted0,SCALAR_EPSILON);
	CPPUNIT_ASSERT( ok == true);

	ok = converter.loadPolygonFromJson(converted1,v1);
	CPPUNIT_ASSERT(ok == true);

	ok = mgl::tequalsPolygonCompare(expected,converted1,SCALAR_EPSILON);
	CPPUNIT_ASSERT(ok == false);
}


inline bool fillPolygons(Polygons& polys, int baseX=0, int baseY=0, int delta=1)
{
	Vector2 v0(baseX+0,baseY+0),v1(baseX+0,baseY+1),v2(baseX+1,baseY+1);
	Vector2 v3(baseX+1,baseY+0),v4(baseX+1,baseY+1),v5(baseX+2,baseY+1);
	Polygon p0,p1;
	p0.push_back(v0); p0.push_back(v1); p0.push_back(v2);
	p1.push_back(v3); p1.push_back(v4); p1.push_back(v5);
	polys.push_back(p0);	polys.push_back(p1);
	return true;
}

void JsonConverterTestCase::testJsonToPolygons( )
{
	JsonConverter converter;
	Json::Value v0,v1, vPolygons,vPolygons1;
	Polygons expected, converted,converted1;
	fillJsonWithPolygon(v0,0.0,0.0);
	fillJsonWithPolygon(v1,1.0,0.0);
	vPolygons.append(v0);
	vPolygons.append(v1);
	vPolygons1.append(v0);
	vPolygons1.append(v0);
	bool ok = fillPolygons(expected);
	CPPUNIT_ASSERT(ok == true);
	ok = converter.loadPolygonsFromJson(converted,vPolygons);
	CPPUNIT_ASSERT(ok == true);

	ok = tequalsPolygonsCompare(expected,converted,SCALAR_EPSILON);
	CPPUNIT_ASSERT(ok == true);

	ok = converter.loadPolygonsFromJson(converted1,vPolygons1);
	CPPUNIT_ASSERT(ok == true);
	ok = tequalsPolygonsCompare(expected,converted1,SCALAR_EPSILON);
	CPPUNIT_ASSERT(ok == false);

}


void JsonConverterTestCase::testJsonToPolygonsGroup()
{

	ExtruderSlice es;
	Value val;
	JsonConverter converter;
	bool ok = converter.loadExtruderSliceFromJson(es,val);
	CPPUNIT_ASSERT(ok == false);
}


void JsonConverterTestCase::testPolygonsToJson()
{
	JsonConverter converter;
	Vector2 v0(0,0),v1(0,1),v2(1,1);
	Vector2 v3(1,0),v4(1,1),v5(2,1);
	Polygon p0,p1;
	p0.push_back(v0); p0.push_back(v1); p0.push_back(v2);
	p1.push_back(v3); p1.push_back(v4); p1.push_back(v5);
	Polygons polyTest;
	polyTest.push_back(p0);	polyTest.push_back(p1);
	Value value;
	bool ok = converter.loadJsonFromPolygons(value, polyTest);
	CPPUNIT_ASSERT(ok == true);
	//cout << value << endl;

	// verify it was built correctly
	Value expPoly0,expPoly1, expPolys;
	Json::Value t0,t1,t2;
	Json::Value t3,t4,t5;
	t0.append(Json::Value(0.0)); t0.append(Json::Value(0.0));
	expPoly0.append(t0);
	t1.append(Json::Value(0.0)); t1.append(Json::Value(1.0));
	expPoly0.append(t1);
	t2.append(Json::Value(1.0)); t2.append(Json::Value(1.0));
	expPoly0.append(t2);

	t3.append(Json::Value(1.0)); t3.append(Json::Value(0.0));
	expPoly1.append(t3);
	t4.append(Json::Value(1.0)); t4.append(Json::Value(1.0));
	expPoly1.append(t4);
	t5.append(Json::Value(2.0)); t5.append(Json::Value(1.0));
	expPoly1.append(t5);

	expPolys.append(expPoly0);
	expPolys.append(expPoly1);

	CPPUNIT_ASSERT(expPolys == value );
}


inline bool fillPolygonsGroup(PolygonsGroup& g, int factor0=0, int factor1=0)
{
	Polygons p0,p1;
	fillPolygons(p0, factor0, factor0);
	fillPolygons(p1, factor1, factor1);
	g.push_back(p0);
	g.push_back(p1);
	return true;
}

void JsonConverterTestCase::testPolygonsGroupToJson()
{
	PolygonsGroup g;
	JsonConverter converter;
	bool ok = fillPolygonsGroup(g,0,5);
	CPPUNIT_ASSERT(ok == true);

//	Value value(Json::objectValue);
//	value["boundary"] = new Value(0);
//	ok = converter.loadJsonFromPolygonsGroup(value, g);
//	//cout << value << endl;
//	CPPUNIT_ASSERT(ok == true); //just test we don't explode or throw errors
//	//CPPUNIT_FAIL("fail: JsonConverterTestCase::testPolygonsGroupToJson");
}

//void JsonConverterTestCase::testExtruderSliceToJson()
//{
////	CPPUNIT_FAIL("fail: JsonConverterTestCase::testExtruderSliceToJson");
//}

void JsonConverterTestCase::testReadScalarToJson() {

//	CPPUNIT_FAIL ("No test yet"); //just test we don't explode or throw errors
}

void JsonConverterTestCase::testReadVector2ToJson(){
//	CPPUNIT_FAIL ("No test yet"); //just test we don't explode or throw errors
}


void JsonConverterTestCase::testReadPolygonToJson(){
//	CPPUNIT_FAIL ("No test yet"); //just test we don't explode or throw errors
}


void JsonConverterTestCase::testReadPolygonsToJson(){
//	CPPUNIT_FAIL ("No test yet"); //just test we don't explode or throw errors
}


void JsonConverterTestCase::testReadPolygonsGroupToJson(){
//	std::string polygonsGroupFile = testCaseInputsDir + "PolygonsGroup.json";
//	std::ifstream ifs(polygonsGroupFile.c_str());
//	std::string rawText((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
//	Value value;
//	Reader r;
//	bool ok = r.parse(rawText,value,true);
//	CPPUNIT_ASSERT(ok == true); //just test we don't explode or throw errors

	/// verify our file vs generated values
//	PolygonsGroup expectedPG,readPG;
//	ok = fillPolygonsGroup(g,0,5);
//	CPPUNIT_ASSERT(ok == true);
//
//
//	ok = polygonsGroupFromJson(readPg,value);
}
