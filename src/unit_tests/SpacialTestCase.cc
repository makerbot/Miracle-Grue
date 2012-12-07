#include <cppunit/config/SourcePrefix.h>
#include <vector>
#include "UnitTestUtils.h"
#include "SpacialTestCase.h"

//#define RTREE_DIAG (1)

#include "mgl/intersection_index.h"
#include "mgl/basic_boxlist.h"
#include "mgl/basic_rtree.h"
#include "mgl/basic_quadtree.h"
#include "mgl/basic_local_rtree.h"
#include <cmath>
#include <ctime>

CPPUNIT_TEST_SUITE_REGISTRATION( SpacialTestCase );

using namespace mgl;

void SpacialTestCase::setUp() {
    //nothing
    std::cout << "\nNo Setup" << std::endl;
}

void SpacialTestCase::testInsertion() {
    typedef basic_boxlist<Segment2Type> lineIndexType;
    std::cout << "Testing insertion of things" << std::endl;
    lineIndexType  lines;
    Segment2Type line1(Point2Type(-1,0), Point2Type(0,1));
    Segment2Type line2(Point2Type(-1,1), Point2Type(0,2));
    Segment2Type line3(Point2Type(0,1), Point2Type(1,0));
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
    typedef basic_boxlist<Segment2Type> lineIndexType;
    typedef std::vector<Segment2Type> simpleCollectionType;
    std::cout << "Testing filtering of things" << std::endl;
    lineIndexType lines;
    Segment2Type line1(Point2Type(-1,0), Point2Type(0,1));
    Segment2Type line2(Point2Type(-1,1), Point2Type(0,2));
    Segment2Type line3(Point2Type(0,1), Point2Type(1,0));
    std::cout << "Inserting the things" << std::endl;
    lines.insert(line1);
    lines.insert(line2);
    lines.insert(line3);
    Segment2Type testLine(Point2Type(-0.5,0.1), Point2Type(-0.5,2));
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
    typedef basic_boxlist<Segment2Type> lineIndexType;
    typedef std::vector<Segment2Type> simpleCollectionType;
    lineIndexType lines;
    Segment2Type testLine(Point2Type(-1.0,0.0), Point2Type(1.0,0.0));
    simpleCollectionType result;
    lines.search(result, LineSegmentFilter(testLine));
    CPPUNIT_ASSERT(result.empty());
    lines.insert(Segment2Type(Point2Type(-1.0,1.0),Point2Type(1.0,1.0)));
    lines.search(result, LineSegmentFilter(testLine));
    CPPUNIT_ASSERT(result.empty());
    lines.insert(Segment2Type(Point2Type(0.0,1.0),Point2Type(0.0,-1.0)));
    lines.search(result, LineSegmentFilter(testLine));
    CPPUNIT_ASSERT(result.size() == 1);
}

Scalar randScalar(Scalar Range) {
    return (Scalar(rand()) * Range) / RAND_MAX;
}
Point2Type randVector(Scalar Range) {
    return Point2Type(randScalar(Range), randScalar(Range));
}
Segment2Type randSegment(Scalar Range, Scalar Range2) {
    Point2Type a = randVector(Range);
    Point2Type b = Point2Type(-0.5 * Range2, -0.5 * Range2) + randVector(Range2);
    return Segment2Type(a, a+b);
}

void SpacialTestCase::testStress() {
    typedef basic_boxlist<Segment2Type> lineIndexType;
    typedef std::vector<Segment2Type> simpleCollectionType;
    srand(static_cast<unsigned int>(time(NULL)));
    simpleCollectionType dataset;
    std::cout << "Making " << SET_SIZE << " lines" << std::endl;
    Scalar range = 500;
    Scalar range2 = 50;
    for(size_t i=0; i < SET_SIZE; ++i) {
        dataset.push_back(randSegment(range, range2));
    }
    Segment2Type testLine = randSegment(range, range2);
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
    typedef basic_rtree<Segment2Type> lineIndexType;
    typedef std::vector<Segment2Type> simpleCollectionType;
    std::cout << "Testing filtering of things" << std::endl;
    lineIndexType lines;
    Segment2Type line1(Point2Type(-1,0), Point2Type(0,1));
    Segment2Type line2(Point2Type(-1,1), Point2Type(0,2));
    Segment2Type line3(Point2Type(0,1), Point2Type(1,0));
    std::cout << "Inserting the things" << std::endl;
    lines.insert(line1);
    lines.insert(line2);
    lines.insert(line3);
    Segment2Type testLine(Point2Type(-0.5,0.1), Point2Type(-0.5,2));
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
    typedef basic_rtree<Segment2Type> lineIndexType;
    typedef std::vector<Segment2Type> simpleCollectionType;
    lineIndexType lines;
    Segment2Type testLine(Point2Type(-1.0,0.0), Point2Type(1.0,0.0));
    simpleCollectionType result;
    lines.search(result, LineSegmentFilter(testLine));
    CPPUNIT_ASSERT(result.empty());
    lines.insert(Segment2Type(Point2Type(-1.0,1.0),Point2Type(1.0,1.0)));
    lines.search(result, LineSegmentFilter(testLine));
    CPPUNIT_ASSERT(result.empty());
    lines.insert(Segment2Type(Point2Type(0.0,1.0),Point2Type(0.0,-1.0)));
    lines.search(result, LineSegmentFilter(testLine));
    CPPUNIT_ASSERT(result.size() == 1);
}

void SpacialTestCase::testRtreeStress() {
    typedef basic_rtree<Segment2Type> lineIndexType;
    typedef std::vector<Segment2Type> simpleCollectionType;
    srand(static_cast<unsigned int>(time(NULL)));
    srand(rand());
    simpleCollectionType dataset;
    //size_t SET_SIZE = 200;
    std::cout << "Making " << SET_SIZE << " lines" << std::endl;
    Scalar range = 500;
    Scalar range2 = 50;
    for(size_t i=0; i < SET_SIZE; ++i) {
        dataset.push_back(randSegment(range, range2));
    }
    Segment2Type testLine = randSegment(range, range2);
    lineIndexType boxlist;
    std::cout << "Building index" << std::endl;
    clock_t start = clock();
    for(simpleCollectionType::const_iterator iter = dataset.begin(); 
            iter != dataset.end(); 
            ++iter) {
        boxlist.insert(*iter);
    }
    std::cout << clock() - start << std::endl;
    //boxlist.repr(std::cerr);
    start = clock();
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

void SpacialTestCase::testQtreeFilter() {
    typedef basic_quadtree<Segment2Type> lineIndexType;
    typedef std::vector<Segment2Type> simpleCollectionType;
    std::cout << "Testing filtering of things" << std::endl;
    lineIndexType lines(AABBox(Point2Type(-2,-2), Point2Type(2,2)));
    Segment2Type line1(Point2Type(-1,0), Point2Type(0,1));
    Segment2Type line2(Point2Type(-1,1), Point2Type(0,2));
    Segment2Type line3(Point2Type(0,1), Point2Type(1,0));
    std::cout << "Inserting the things" << std::endl;
    lines.insert(line1);
    lines.insert(line2);
    lines.insert(line3);
    Segment2Type testLine(Point2Type(-0.5,0.1), Point2Type(-0.5,2));
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

void SpacialTestCase::testQtreeEmpty() {
    typedef basic_quadtree<Segment2Type> lineIndexType;
    typedef std::vector<Segment2Type> simpleCollectionType;
    lineIndexType lines(AABBox(Point2Type(-2,-2), Point2Type(2,2)));
    Segment2Type testLine(Point2Type(-1.0,0.0), Point2Type(1.0,0.0));
    simpleCollectionType result;
    lines.search(result, LineSegmentFilter(testLine));
    CPPUNIT_ASSERT(result.empty());
    lines.insert(Segment2Type(Point2Type(-1.0,1.0),Point2Type(1.0,1.0)));
    lines.search(result, LineSegmentFilter(testLine));
    CPPUNIT_ASSERT(result.empty());
    lines.insert(Segment2Type(Point2Type(0.0,1.0),Point2Type(0.0,-1.0)));
    lines.search(result, LineSegmentFilter(testLine));
    CPPUNIT_ASSERT(result.size() == 1);
}

void SpacialTestCase::testQtreeStress() {
    typedef basic_quadtree<Segment2Type> lineIndexType;
    typedef std::vector<Segment2Type> simpleCollectionType;
    srand(static_cast<unsigned int>(time(NULL)));
    srand(rand());
    simpleCollectionType dataset;
    size_t SET_SIZE = 50000;
    std::cout << "Making " << SET_SIZE << " lines" << std::endl;
    Scalar range = 500;
    Scalar range2 = 50;
    for(size_t i=0; i < SET_SIZE; ++i) {
        dataset.push_back(randSegment(range, range2));
    }
    Segment2Type testLine = Segment2Type(randVector(range), randVector(range));
    lineIndexType boxlist(AABBox(Point2Type(-2,-2), Point2Type(range + 2,range + 2)));
    std::cout << "Building index" << std::endl;
    clock_t start = clock();
    for(simpleCollectionType::const_iterator iter = dataset.begin(); 
            iter != dataset.end(); 
            ++iter) {
        boxlist.insert(*iter);
    }
    std::cout << clock() - start << std::endl;
    //boxlist.repr_svg(std::cerr);
    start = clock();
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

void SpacialTestCase::testPerformance() {
//    srand(static_cast<unsigned int>(time(NULL)));
    srand(0);
    typedef std::vector<Segment2Type> vector;
    
    vector dataset;
    //basic_boxlist<Segment2Type> boxlist;
    basic_rtree<Segment2Type, 4> rtree;
    vector testset;
    
    static const size_t SET_SIZE = TEST_SET_SIZE;
    static const size_t TEST_SIZE = TEST_TEST_SIZE;
    Scalar range = 200;
    Scalar range2 = 20;
    
    std::cout << "Making " << SET_SIZE << " lines" << std::endl;
    for(size_t i=0; i < SET_SIZE; ++i) {
        dataset.push_back(randSegment(range, range2));
    }
    std::cout << "Making " << TEST_SIZE << " lines for testing" << std::endl;
    for(size_t i=0; i < TEST_SIZE; ++i) {
        testset.push_back(randSegment(range, range2));
    }
    time_t start = clock();
//    std::cout << "Building Boxlist" << std::endl;
//    for(vector::const_iterator iter = dataset.begin();
//            iter != dataset.end(); 
//            ++iter)
//        boxlist.insert(*iter);
//    std::cout << clock() - start << std::endl;
    start = clock();
    std::cout << "Building Rtree" << std::endl;
    for(vector::const_iterator iter = dataset.begin();
            iter != dataset.end(); 
            ++iter)
        rtree.insert(*iter);
    std::cout << clock() - start << std::endl;
    start = clock();
//    std::cout << "Testing Boxlist" << std::endl;
//    for(vector::const_iterator iter = testset.begin();
//            iter != testset.end(); 
//            ++iter) {
//        vector result;
//        boxlist.search(result, LineSegmentFilter(*iter));
//    }
//    std::cout << clock() - start << std::endl;
    start = clock();
    std::cout << "Testing Rtree" << std::endl;
    for(vector::const_iterator iter = testset.begin();
            iter != testset.end(); 
            ++iter) {
        vector result;
        rtree.search(result, LineSegmentFilter(*iter));
    }
    std::cout << clock() - start << std::endl;
    start = clock();
//    std::cout << "Writing svg to cerr" << std::endl;
//    rtree.repr_svg(std::cerr);
//    std::cout << "Writing tree to cout" << std::endl;
//    rtree.repr(std::cout);
}
void SpacialTestCase::testQPerformance() {
//    srand(static_cast<unsigned int>(time(NULL)));
    srand(0);
    typedef std::vector<Segment2Type> vector;
    
    vector dataset;
    basic_boxlist<Segment2Type> boxlist;
    vector testset;
    
    static const size_t SET_SIZE = TEST_SET_SIZE;
    static const size_t TEST_SIZE = TEST_TEST_SIZE;
    Scalar range = 200;
    Scalar range2 = 20;
    
    basic_quadtree<Segment2Type> rtree(AABBox(Point2Type(-2,-2), Point2Type(range + 2,range + 2)));
    
    std::cout << "Making " << SET_SIZE << " lines" << std::endl;
    for(size_t i=0; i < SET_SIZE; ++i) {
        dataset.push_back(randSegment(range, range2));
    }
    std::cout << "Making " << TEST_SIZE << " lines for testing" << std::endl;
    for(size_t i=0; i < TEST_SIZE; ++i) {
        testset.push_back(randSegment(range, range2));
    }
    time_t start = clock();
//    std::cout << "Building Boxlist" << std::endl;
//    for(vector::const_iterator iter = dataset.begin();
//            iter != dataset.end(); 
//            ++iter)
//        boxlist.insert(*iter);
//    std::cout << clock() - start << std::endl;
    start = clock();
    std::cout << "Building Quadtree" << std::endl;
    for(vector::const_iterator iter = dataset.begin();
            iter != dataset.end(); 
            ++iter)
        rtree.insert(*iter);
    std::cout << clock() - start << std::endl;
    start = clock();
//    std::cout << "Testing Boxlist" << std::endl;
//    for(vector::const_iterator iter = testset.begin();
//            iter != testset.end(); 
//            ++iter) {
//        vector result;
//        boxlist.search(result, LineSegmentFilter(*iter));
//    }
//    std::cout << clock() - start << std::endl;
    start = clock();
    std::cout << "Testing Quadtree" << std::endl;
    for(vector::const_iterator iter = testset.begin();
            iter != testset.end(); 
            ++iter) {
        vector result;
        rtree.search(result, LineSegmentFilter(*iter));
    }
    std::cout << clock() - start << std::endl;
    start = clock();
//    std::cout << "Writing svg to cerr" << std::endl;
//    rtree.repr_svg(std::cerr);
//    std::cout << "Writing tree to cout" << std::endl;
//    rtree.repr(std::cout);
}

void SpacialTestCase::testLPerformance() {
    //    srand(static_cast<unsigned int>(time(NULL)));
    srand(0);
    typedef std::vector<Segment2Type> vector;
    
    vector dataset;
    //basic_boxlist<Segment2Type> boxlist;
    vector testset;
    
    static const size_t SET_SIZE = TEST_SET_SIZE;
    static const size_t TEST_SIZE = TEST_TEST_SIZE;
    Scalar range = 200;
    Scalar range2 = 20;
    
    basic_local_rtree<Segment2Type> rtree;
    
    std::cout << "Making " << SET_SIZE << " lines" << std::endl;
    for(size_t i=0; i < SET_SIZE; ++i) {
        dataset.push_back(randSegment(range, range2));
    }
    std::cout << "Making " << TEST_SIZE << " lines for testing" << std::endl;
    for(size_t i=0; i < TEST_SIZE; ++i) {
        testset.push_back(randSegment(range, range2));
    }
    time_t start = clock();
//    std::cout << "Building Boxlist" << std::endl;
//    for(vector::const_iterator iter = dataset.begin();
//            iter != dataset.end(); 
//            ++iter)
//        boxlist.insert(*iter);
//    std::cout << clock() - start << std::endl;
    start = clock();
    std::cout << "Building Localized Rtree" << std::endl;
    for(vector::const_iterator iter = dataset.begin();
            iter != dataset.end(); 
            ++iter)
        rtree.insert(*iter);
    std::cout << clock() - start << std::endl;
    start = clock();
//    std::cout << "Testing Boxlist" << std::endl;
//    for(vector::const_iterator iter = testset.begin();
//            iter != testset.end(); 
//            ++iter) {
//        vector result;
//        boxlist.search(result, LineSegmentFilter(*iter));
//    }
//    std::cout << clock() - start << std::endl;
    start = clock();
    std::cout << "Testing Localized Rtree" << std::endl;
    for(vector::const_iterator iter = testset.begin();
            iter != testset.end(); 
            ++iter) {
        vector result;
        rtree.search(result, LineSegmentFilter(*iter));
    }
    std::cout << clock() - start << std::endl;
    start = clock();
//    std::cout << "Writing svg to cerr" << std::endl;
//    rtree.repr_svg(std::cerr);
//    std::cout << "Writing tree to cout" << std::endl;
//    rtree.repr(std::cout);
}




