#include <cppunit/config/SourcePrefix.h>
#include <vector>
#include "UnitTestUtils.h"
#include "SpacialTestCase.h"
#include "mgl/intersection_index.h"
#include "mgl/basic_boxlist.h"

CPPUNIT_TEST_SUITE_REGISTRATION( SpacialTestCase );

using namespace mgl;
typedef libthing::LineSegment2 SegmentType;

void SpacialTestCase::setUp() {
    //nothing
    std::cout << "\nNo Setup" << std::endl;
}

void SpacialTestCase::testInsertion() {
    std::cout << "Testing insertion of things" << std::endl;
    basic_boxlist<SegmentType>  lines;
    SegmentType line1(PointType(-1,0), PointType(0,1));
    SegmentType line2(PointType(-1,1), PointType(0,2));
    SegmentType line3(PointType(0,1), PointType(1,0));
    std::cout << "Inserting the things" << std::endl;
    lines.insert(line1);
    lines.insert(line2);
    lines.insert(line3);
    size_t counter = 0;
    size_t expected = 3;
    std::cout << "Counting things" << std::endl;
    for(basic_boxlist<SegmentType>::iterator iter = lines.begin(); 
            iter != lines.end(); 
            ++iter, ++counter);
    std::cout << "Are there enough things?" << std::endl;
    std::cout << "Expected : " << expected << std::endl;
    std::cout << "Actual   : " << counter << std::endl;
    CPPUNIT_ASSERT_EQUAL(expected, counter);
}

void SpacialTestCase::testFilter() {
    std::cout << "Testing filtering of things" << std::endl;
    basic_boxlist<SegmentType>  lines;
    SegmentType line1(PointType(-1,0), PointType(0,1));
    SegmentType line2(PointType(-1,1), PointType(0,2));
    SegmentType line3(PointType(0,1), PointType(1,0));
    std::cout << "Inserting the things" << std::endl;
    lines.insert(line1);
    lines.insert(line2);
    lines.insert(line3);
    SegmentType testLine(PointType(-0.5,0.1), PointType(-0.5,2));
    std::vector<SegmentType> result;
    std::cout << "Testing the things" << std::endl;
    lines.search(result, LineSegmentFilter(testLine));
    std::cout << "All the things?" << std::endl;
    size_t counter = 0;
    size_t expected = 2;
    for(std::vector<SegmentType>::const_iterator iter = result.begin(); 
            iter != result.end(); 
            ++iter, ++counter) {
        std::cout << "Segment " << counter + 1 << ": \t" 
                << iter->a << " - " << iter->b << std::endl;
    }
    std::cout << "Expected : " << expected << std::endl;
    std::cout << "Actual   : " << counter << std::endl;
    CPPUNIT_ASSERT_EQUAL(expected, counter);
}



