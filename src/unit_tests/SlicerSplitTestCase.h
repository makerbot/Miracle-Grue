#ifndef SLICER_SPLIT_TEST_CASE_H
#define SLICER_SPLIT_TEST_CASE_H

#include <cppunit/extensions/HelperMacros.h>



class SlicerSplitTestCase : public CPPUNIT_NS::TestFixture
{

private:

	CPPUNIT_TEST_SUITE( SlicerSplitTestCase );
		CPPUNIT_TEST(test_m);
		CPPUNIT_TEST(test_calibration_slice_70);
    CPPUNIT_TEST_SUITE_END();




protected:

  void test_m();

  void test_calibration_slice_70();
};


#endif
