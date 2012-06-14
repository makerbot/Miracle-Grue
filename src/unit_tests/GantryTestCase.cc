#include "UnitTestUtils.h"
#include "GantryTestCase.h"

#include "mgl/gcoder_gantry.h"
#include "mgl/gcoder.h"

#include <iostream>
#include <sstream>

using namespace std;
using namespace mgl;
using namespace libthing;

CPPUNIT_TEST_SUITE_REGISTRATION( GantryTestCase );

void GantryTestCase::setUp(){
	std::cout << " Setup Done" << endl;
}

void GantryTestCase::testInitConfig(){
	GantryConfig gantryCfg;
	Gantry gantry(gantryCfg);
	
	CPPUNIT_ASSERT_EQUAL(gantryCfg.get_start_x(), gantry.get_x());
	CPPUNIT_ASSERT_EQUAL(gantryCfg.get_start_y(), gantry.get_y());
	CPPUNIT_ASSERT_EQUAL(gantryCfg.get_start_z(), gantry.get_z());
	CPPUNIT_ASSERT_EQUAL(gantryCfg.get_start_a(), gantry.get_a());
	CPPUNIT_ASSERT_EQUAL(gantryCfg.get_start_b(), gantry.get_b());
	CPPUNIT_ASSERT_EQUAL(gantryCfg.get_start_feed(), gantry.get_feed());
}
void GantryTestCase::testManualMove(){
	static const Scalar x = 10, y = 20, z = 50;
		
	GantryConfig gantryCfg;
	Gantry gantry(gantryCfg);
	
	gantry.set_x(x);
	gantry.set_y(y);
	gantry.set_z(z);
	
	CPPUNIT_ASSERT_EQUAL(x, gantry.get_x());
	CPPUNIT_ASSERT_EQUAL(y, gantry.get_y());
	CPPUNIT_ASSERT_EQUAL(z, gantry.get_z());
}
void GantryTestCase::testG1Move(){
	static const Scalar x = 75, y = 42, z = 13;
	
	stringstream ss;
	stringstream expected;
	
	GantryConfig gantryCfg;
	Gantry gantry(gantryCfg);
	
	gantry.set_x(0);
	gantry.set_y(0);
	gantry.set_z(0);
	gantry.set_feed(3200);
	
	expected << "G1 X" << x << " Y" << y << " Z" << z << 
			" F" << 3200 << " ()" << endl;
	
	try{
		gantry.g1(ss, x, y, z, gantry.get_feed(), "");
	} catch (mgl::Exception thrown){
		CPPUNIT_ASSERT_MESSAGE(thrown.error, false);
	}
	string estring = expected.str();
	string astring = ss.str();
	
	cout << "Expected GCode: \t" << estring << endl;
	cout << "Actual GCode:   \t" << astring << endl;
	
	CPPUNIT_ASSERT(estring == astring);
}
void GantryTestCase::testG1Extrude(){
	static const Scalar x = 75, y = 42, z = 0;
	
	stringstream ss;
	stringstream expected;
	
	GantryConfig gantryCfg;
	Gantry gantry(gantryCfg);
	
	Extruder uder;
	Extrusion usion;
	
	gantry.set_x(0);
	gantry.set_y(0);
	gantry.set_z(0);
	gantry.set_feed(3200);
	
	gantry.set_extruding(true);
	
	try{
		gantry.g1(ss, uder, usion, x, y, z, usion.feedrate, "");
	} catch (mgl::Exception thrown){
		CPPUNIT_ASSERT_MESSAGE(thrown.error, false);
	}
	
	string estring = expected.str();
	string astring = ss.str();
	
	cout << "Expected GCode: \t" << estring << endl;
	cout << "Actual GCode:   \t" << astring << endl;
	
	CPPUNIT_ASSERT(astring.size());
}
void GantryTestCase::testSquirtSnort(){
	stringstream ss;
	stringstream expected;
	string astring;
	string estring;
	
	GantryConfig gantryCfg;
	Gantry gantry(gantryCfg);
	
	gantry.set_x(0);
	gantry.set_y(0);
	gantry.set_z(0);
	gantry.set_feed(3200);
	gantry.setCurrentE(0);
	
	Extruder uder;
	Extrusion usion;
	
	usion.retractDistance = -0.5;
	
	cout << "\nSnort test\n" << endl;
	
	gantry.snort(ss, Vector2(20,20), uder, usion);
	expected << "G1" << " F" << usion.retractRate << 
			" A" << -usion.retractDistance << " (snort)" << endl;
	astring = ss.str();
	estring = expected.str();
	cout << "Expected: \t" << estring << endl;
	cout << "Actual:   \t" << astring << endl;
	CPPUNIT_ASSERT_MESSAGE("Snort test failed", astring == estring);
	ss.str("");
	expected.str("");
	
	cout << "\nSquirt test\n" << endl;
	
	gantry.squirt(ss, Vector2(30,30), uder, usion);
	expected << "G1" << " F" << usion.retractRate << 
			" A" << 0.0 << " (squirt)" << endl;
	astring = ss.str();
	estring = expected.str();
	cout << "Expected: \t" << estring << endl;
	cout << "Actual:   \t" << astring << endl;
	CPPUNIT_ASSERT_MESSAGE("Squirt test failed", astring == estring);
	
}

void GantryTestCase::testConfig(){
	
	const Scalar x = 4, y = 5, z = 6;
	
	GantryConfig gantryCfg;
	
	CPPUNIT_ASSERT_EQUAL(gantryCfg.get_start_x(), 
			MUCH_LARGER_THAN_THE_BUILD_PLATFORM_MM);
	CPPUNIT_ASSERT_EQUAL(gantryCfg.get_start_y(), 
			MUCH_LARGER_THAN_THE_BUILD_PLATFORM_MM);
	CPPUNIT_ASSERT_EQUAL(gantryCfg.get_start_z(), 
			MUCH_LARGER_THAN_THE_BUILD_PLATFORM_MM);
	CPPUNIT_ASSERT(gantryCfg.get_start_a() == 0);
	CPPUNIT_ASSERT(gantryCfg.get_start_b() == 0);
	CPPUNIT_ASSERT(gantryCfg.get_start_feed() == 0);
	
	gantryCfg.set_start_x(x);
	CPPUNIT_ASSERT(gantryCfg.get_start_x() == x);
	gantryCfg.set_start_y(y);
	CPPUNIT_ASSERT(gantryCfg.get_start_y() == y);
	gantryCfg.set_start_z(z);
	CPPUNIT_ASSERT(gantryCfg.get_start_z() == z);
}


