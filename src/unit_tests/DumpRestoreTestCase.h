#ifndef DUMPRESTORE_H
#define	DUMPRESTORE_H

#include <cppunit/extensions/HelperMacros.h>

#include "mgl/loop_path.h"

class DumpRestoreTestCase : public CPPUNIT_NS::TestFixture{

	CPPUNIT_TEST_SUITE( DumpRestoreTestCase );

    CPPUNIT_TEST( testDumpPoint );
    CPPUNIT_TEST( testDumpLoop );
    CPPUNIT_TEST( testDumpLoopList );
    CPPUNIT_TEST( testRestorePoint );
    CPPUNIT_TEST( testRestoreLoop );
    CPPUNIT_TEST( testRestoreLoopList );

	CPPUNIT_TEST_SUITE_END();
	
public:
	void setUp();

protected:
    void testDumpPoint();
    void testDumpLoop();
    void testDumpLoopList();
    void testRestorePoint();
    void testRestoreLoop();
    void testRestoreLoopList();

private:
	mgl::Loop loop1;
    mgl::Loop loop2;
    libthing::Vector2 point;

    std::string loopstr;
    std::string loopliststr;
    std::string pointstr;
};


#endif	/* LOOPPATHTESTCASE_H */

