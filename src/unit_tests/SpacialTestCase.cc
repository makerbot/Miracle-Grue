#include <cppunit/config/SourcePrefix.h>
#include <vector>
#include "UnitTestUtils.h"
#include "SpacialTestCase.h"
#include "mgl/intersection_index.h"
#include "mgl/basic_boxlist.h"
#include "mgl/basic_rtree.h"
#include <cmath>
#include <ctime>

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
    basic_boxlist<SegmentType> lines;
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

void SpacialTestCase::testEmpty() {
    basic_boxlist<SegmentType> lines;
    SegmentType testLine(PointType(-1.0,0.0), PointType(1.0,0.0));
    std::vector<SegmentType> result;
    lines.search(result, LineSegmentFilter(testLine));
    CPPUNIT_ASSERT(result.empty());
    lines.insert(SegmentType(PointType(-1.0,1.0),PointType(1.0,1.0)));
    lines.search(result, LineSegmentFilter(testLine));
    CPPUNIT_ASSERT(result.empty());
    lines.insert(SegmentType(PointType(0.0,1.0),PointType(0.0,-1.0)));
    lines.search(result, LineSegmentFilter(testLine));
    CPPUNIT_ASSERT(result.size() == 1);
}

Scalar randScalar(Scalar Range) {
    return (Scalar(rand()) * Range) / RAND_MAX;
}
PointType randVector(Scalar Range) {
    return PointType(randScalar(Range), randScalar(Range));
}
SegmentType randSegment(Scalar Range) {
    return SegmentType(randVector(Range), randVector(Range));
}

void SpacialTestCase::testStress() {
    srand(static_cast<unsigned int>(time(NULL)));
    static const size_t SET_SIZE = 1000000;
    std::vector<SegmentType> dataset;
    std::cout << "Making " << SET_SIZE << " lines" << std::endl;
    Scalar range = 500;
    for(size_t i=0; i < SET_SIZE; ++i) {
        dataset.push_back(randSegment(range));
    }
    SegmentType testLine = randSegment(range);
    basic_boxlist<SegmentType> boxlist;
    std::cout << "Building index" << std::endl;
    for(std::vector<SegmentType>::const_iterator iter = dataset.begin(); 
            iter != dataset.end(); 
            ++iter) {
        boxlist.insert(*iter);
    }
    std::cout << "Filtering set" << std::endl;
    std::vector<SegmentType> result;
    boxlist.search(result, LineSegmentFilter(testLine));
    std::cout << "Remaining " << result.size() << " to test" << std::endl;
    std::vector<SegmentType> finalFiltered;
    std::vector<SegmentType> finalBrute;
    for(std::vector<SegmentType>::const_iterator iter = result.begin(); 
            iter != result.end(); 
            ++iter) {
        if(testLine.intersects(*iter))
            finalFiltered.push_back(*iter);
    }
    std::cout << "Final outcome of filtering  : " << finalFiltered.size() 
            << std::endl;
    std::cout << "Brute force test" << std::endl;
    for(std::vector<SegmentType>::const_iterator iter = dataset.begin(); 
            iter != dataset.end(); 
            ++iter) {
        if(testLine.intersects(*iter))
            finalBrute.push_back(*iter);
    }
    std::cout << "Final outcome of brute check: " << finalBrute.size() 
            << std::endl;
    CPPUNIT_ASSERT_EQUAL(finalBrute.size(), finalFiltered.size());
}

void SpacialTestCase::testRtree() {
    srand(static_cast<unsigned int>(time(NULL)));
    static const size_t SET_SIZE = 1000000;
    std::vector<SegmentType> dataset;
    std::cout << "Making " << SET_SIZE << " lines" << std::endl;
    Scalar range = 500;
    for(size_t i=0; i < SET_SIZE; ++i) {
        dataset.push_back(randSegment(range));
    }
    SegmentType testLine = randSegment(range);
    basic_rtree<SegmentType> boxlist;
    std::cout << "Building index" << std::endl;
    for(std::vector<SegmentType>::const_iterator iter = dataset.begin(); 
            iter != dataset.end(); 
            ++iter) {
        const SegmentType& cur = *iter;
        boxlist.insert(cur);
        //boxlist.repr(std::cout);
        //std::cout << std::endl;
    }
    std::cout << "Filtering set" << std::endl;
    std::vector<SegmentType> result;
    boxlist.search(result, LineSegmentFilter(testLine));
    std::cout << "Remaining " << result.size() << " to test" << std::endl;
    std::vector<SegmentType> finalFiltered;
    std::vector<SegmentType> finalBrute;
    for(std::vector<SegmentType>::const_iterator iter = result.begin(); 
            iter != result.end(); 
            ++iter) {
        if(testLine.intersects(*iter))
            finalFiltered.push_back(*iter);
    }
    std::cout << "Final outcome of filtering  : " << finalFiltered.size() 
            << std::endl;
    std::cout << "Brute force test" << std::endl;
    for(std::vector<SegmentType>::const_iterator iter = dataset.begin(); 
            iter != dataset.end(); 
            ++iter) {
        if(testLine.intersects(*iter))
            finalBrute.push_back(*iter);
    }
    std::cout << "Final outcome of brute check: " << finalBrute.size() 
            << std::endl;
    CPPUNIT_ASSERT_EQUAL(finalBrute.size(), finalFiltered.size());
}



