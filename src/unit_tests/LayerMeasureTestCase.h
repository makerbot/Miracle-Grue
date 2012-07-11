#ifndef LAYERMEASURETESTCASE_H
#define	LAYERMEASURETESTCASE_H

#include <cppunit/extensions/HelperMacros.h>


class LayerMeasureTestCase : public CPPUNIT_NS::TestFixture {
	
	CPPUNIT_TEST_SUITE( LayerMeasureTestCase );
	CPPUNIT_TEST( testLayer0 );
	CPPUNIT_TEST( testCreatingLayers );
	CPPUNIT_TEST( testOffset );
	CPPUNIT_TEST_SUITE_END();
	
public:
	void setUp();
protected:
	void testLayer0();
	void testCreatingLayers();
	void testOffset();
};



#endif	/* LAYERMEASURETESTCASE_H */

