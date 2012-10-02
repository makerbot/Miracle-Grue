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
    typedef basic_boxlist<SegmentType> lineIndexType;
    std::cout << "Testing insertion of things" << std::endl;
    lineIndexType  lines;
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
    for(lineIndexType::iterator iter = lines.begin(); 
            iter != lines.end(); 
            ++iter, ++counter);
    std::cout << "Are there enough things?" << std::endl;
    std::cout << "Expected : " << expected << std::endl;
    std::cout << "Actual   : " << counter << std::endl;
    CPPUNIT_ASSERT_EQUAL(expected, counter);
}

void SpacialTestCase::testFilter() {
    typedef basic_boxlist<SegmentType> lineIndexType;
    typedef std::vector<SegmentType> simpleCollectionType;
    std::cout << "Testing filtering of things" << std::endl;
    lineIndexType lines;
    SegmentType line1(PointType(-1,0), PointType(0,1));
    SegmentType line2(PointType(-1,1), PointType(0,2));
    SegmentType line3(PointType(0,1), PointType(1,0));
    std::cout << "Inserting the things" << std::endl;
    lines.insert(line1);
    lines.insert(line2);
    lines.insert(line3);
    SegmentType testLine(PointType(-0.5,0.1), PointType(-0.5,2));
    simpleCollectionType result;
    std::cout << "Testing the things" << std::endl;
    lines.search(result, LineSegmentFilter(testLine));
    std::cout << "All the things?" << std::endl;
    size_t counter = 0;
    size_t expected = 2;
    for(simpleCollectionType::const_iterator iter = result.begin(); 
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
    typedef basic_boxlist<SegmentType> lineIndexType;
    typedef std::vector<SegmentType> simpleCollectionType;
    lineIndexType lines;
    SegmentType testLine(PointType(-1.0,0.0), PointType(1.0,0.0));
    simpleCollectionType result;
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
    typedef basic_boxlist<SegmentType> lineIndexType;
    typedef std::vector<SegmentType> simpleCollectionType;
    srand(static_cast<unsigned int>(time(NULL)));
    simpleCollectionType dataset;
    std::cout << "Making " << SET_SIZE << " lines" << std::endl;
    Scalar range = 500;
    for(size_t i=0; i < SET_SIZE; ++i) {
        dataset.push_back(randSegment(range));
    }
    SegmentType testLine = randSegment(range);
    lineIndexType boxlist;
    std::cout << "Building index" << std::endl;
    for(simpleCollectionType::const_iterator iter = dataset.begin(); 
            iter != dataset.end(); 
            ++iter) {
        boxlist.insert(*iter);
    }
    clock_t start = clock();
    std::cout << "Filtering set" << std::endl;
    simpleCollectionType result;
    boxlist.search(result, LineSegmentFilter(testLine));
    std::cout << "Remaining " << result.size() << " to test" << std::endl;
    simpleCollectionType finalFiltered;
    simpleCollectionType finalBrute;
    for(simpleCollectionType::const_iterator iter = result.begin(); 
            iter != result.end(); 
            ++iter) {
        if(testLine.intersects(*iter))
            finalFiltered.push_back(*iter);
    }
    std::cout << "Final outcome of filtering  : " << finalFiltered.size() 
            << std::endl;
    std::cout << clock() - start << std::endl;
    start = clock();
    std::cout << "Brute force test" << std::endl;
    for(simpleCollectionType::const_iterator iter = dataset.begin(); 
            iter != dataset.end(); 
            ++iter) {
        if(testLine.intersects(*iter))
            finalBrute.push_back(*iter);
    }
    std::cout << "Final outcome of brute check: " << finalBrute.size() 
            << std::endl;
    std::cout << clock() - start << std::endl;
    CPPUNIT_ASSERT_EQUAL(finalBrute.size(), finalFiltered.size());
}

void SpacialTestCase::testRtreeFilter() {
    typedef basic_rtree<SegmentType> lineIndexType;
    typedef std::vector<SegmentType> simpleCollectionType;
    std::cout << "Testing filtering of things" << std::endl;
    lineIndexType lines;
    SegmentType line1(PointType(-1,0), PointType(0,1));
    SegmentType line2(PointType(-1,1), PointType(0,2));
    SegmentType line3(PointType(0,1), PointType(1,0));
    std::cout << "Inserting the things" << std::endl;
    lines.insert(line1);
    lines.insert(line2);
    lines.insert(line3);
    SegmentType testLine(PointType(-0.5,0.1), PointType(-0.5,2));
    simpleCollectionType result;
    std::cout << "Testing the things" << std::endl;
    lines.search(result, LineSegmentFilter(testLine));
    std::cout << "All the things?" << std::endl;
    size_t counter = 0;
    size_t expected = 2;
    for(simpleCollectionType::const_iterator iter = result.begin(); 
            iter != result.end(); 
            ++iter, ++counter) {
        std::cout << "Segment " << counter + 1 << ": \t" 
                << iter->a << " - " << iter->b << std::endl;
    }
    std::cout << "Expected : " << expected << std::endl;
    std::cout << "Actual   : " << counter << std::endl;
    CPPUNIT_ASSERT_EQUAL(expected, counter);
}

void SpacialTestCase::testRtreeEmpty() {
    typedef basic_rtree<SegmentType> lineIndexType;
    typedef std::vector<SegmentType> simpleCollectionType;
    lineIndexType lines;
    SegmentType testLine(PointType(-1.0,0.0), PointType(1.0,0.0));
    simpleCollectionType result;
    lines.search(result, LineSegmentFilter(testLine));
    CPPUNIT_ASSERT(result.empty());
    lines.insert(SegmentType(PointType(-1.0,1.0),PointType(1.0,1.0)));
    lines.search(result, LineSegmentFilter(testLine));
    CPPUNIT_ASSERT(result.empty());
    lines.insert(SegmentType(PointType(0.0,1.0),PointType(0.0,-1.0)));
    lines.search(result, LineSegmentFilter(testLine));
    CPPUNIT_ASSERT(result.size() == 1);
}

void SpacialTestCase::testRtreeStress() {
    typedef basic_rtree<SegmentType> lineIndexType;
    typedef std::vector<SegmentType> simpleCollectionType;
    srand(static_cast<unsigned int>(time(NULL)));
    simpleCollectionType dataset;
    std::cout << "Making " << SET_SIZE << " lines" << std::endl;
    Scalar range = 500;
    for(size_t i=0; i < SET_SIZE; ++i) {
        dataset.push_back(randSegment(range));
    }
    SegmentType testLine = randSegment(range);
    lineIndexType boxlist;
    std::cout << "Building index" << std::endl;
    for(simpleCollectionType::const_iterator iter = dataset.begin(); 
            iter != dataset.end(); 
            ++iter) {
        boxlist.insert(*iter);
    }
    clock_t start = clock();
    std::cout << "Filtering set" << std::endl;
    simpleCollectionType result;
    boxlist.search(result, LineSegmentFilter(testLine));
    std::cout << "Remaining " << result.size() << " to test" << std::endl;
    simpleCollectionType finalFiltered;
    simpleCollectionType finalBrute;
    for(simpleCollectionType::const_iterator iter = result.begin(); 
            iter != result.end(); 
            ++iter) {
        if(testLine.intersects(*iter))
            finalFiltered.push_back(*iter);
    }
    std::cout << "Final outcome of filtering  : " << finalFiltered.size() 
            << std::endl;
    std::cout << clock() - start << std::endl;
    start = clock();
    std::cout << "Brute force test" << std::endl;
    for(simpleCollectionType::const_iterator iter = dataset.begin(); 
            iter != dataset.end(); 
            ++iter) {
        if(testLine.intersects(*iter))
            finalBrute.push_back(*iter);
    }
    std::cout << "Final outcome of brute check: " << finalBrute.size() 
            << std::endl;
    std::cout << clock() - start << std::endl;
    CPPUNIT_ASSERT_EQUAL(finalBrute.size(), finalFiltered.size());
}



