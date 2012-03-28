#ifndef SLICERCUP_TEST_CASE_H
#define SLICERCUP_TEST_CASE_H

#include <cppunit/extensions/HelperMacros.h>



class SlicerCupTestCase : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( SlicerCupTestCase );
//    //    CPPUNIT_TEST( testAllTogeter );
//        CPPUNIT_TEST( testIndividuals );
//        CPPUNIT_TEST( testSpecificIssues );
//        CPPUNIT_TEST( testSpecificIssuesB );
//        CPPUNIT_TEST( testSpecificIssuesC );
//        //CPPUNIT_TEST( testCathedral_Crossing_bad);
//        // CPPUNIT_TEST( testCathedral_Crossing_fixed);
	CPPUNIT_TEST( testSliceShifting );
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
protected:

  void testAllTogeter();
  void testIndividuals();
  void testCathedral_Crossing_bad();
  void testCathedral_Crossing_fixed();
  void testSpecificIssues();
  void testSpecificIssuesB();
  void testSpecificIssuesC();

  void testSliceShifting();

};


#endif
