#include "UnitTestUtils.h"
#include "ContainmentTestCase.h"

#include "mgl/ContainmentTree.h"

#include <iostream>
#include <sstream>

using namespace mgl;

CPPUNIT_TEST_SUITE_REGISTRATION( ContainmentTestCase );

typedef ContainmentTree<int> IntTree;

void ContainmentTestCase::setUp(){
    
    innerPoint = Point2Type(0.5, -0.5);
    
    std::cout << "\nSetting up inner loop" << std::endl;
    Scalar dim = 1;
	m_loopInner.clear();
    m_loopInner.insertPointBefore(Point2Type(-dim,-dim), m_loopInner.clockwiseEnd());
    m_loopInner.insertPointBefore(Point2Type(dim,-dim), m_loopInner.clockwiseEnd());
    m_loopInner.insertPointBefore(Point2Type(dim,dim), m_loopInner.clockwiseEnd());
    m_loopInner.insertPointBefore(Point2Type(-dim,dim), m_loopInner.clockwiseEnd());
    
    outerPoint = Point2Type(-1.5, 1.5);
    
    std::cout << "Setting up outer loop" << std::endl;
    dim *= 2;
    m_loopOuter.clear();
    m_loopOuter.insertPointBefore(Point2Type(-dim,-dim), m_loopOuter.clockwiseEnd());
    m_loopOuter.insertPointBefore(Point2Type(dim,-dim), m_loopOuter.clockwiseEnd());
    m_loopOuter.insertPointBefore(Point2Type(dim,dim), m_loopOuter.clockwiseEnd());
    m_loopOuter.insertPointBefore(Point2Type(-dim,dim), m_loopOuter.clockwiseEnd());
    
    outsidePoint = Point2Type(2.5, 0);
    
    std::cout << "Setup Done!" << std::endl;
}

void ContainmentTestCase::testInsideOutside() {
    IntTree tree(m_loopInner);
    CPPUNIT_ASSERT(tree.contains(innerPoint));
    CPPUNIT_ASSERT(!tree.contains(outerPoint));
    CPPUNIT_ASSERT(!tree.contains(outsidePoint));
}

void ContainmentTestCase::testHierarchy() {
    IntTree innerTree(m_loopInner);
    innerTree.value() = 1;
    IntTree outerTree(m_loopOuter);
    outerTree.value() = 2;
    
    CPPUNIT_ASSERT(outerTree.contains(innerTree));
    
    IntTree& inserted = outerTree.insert(innerTree);
    
    CPPUNIT_ASSERT_EQUAL(1, inserted.value());
}

void ContainmentTestCase::testReverseOrder() {
    IntTree innerTree(m_loopInner);
    innerTree.value() = 1;
    IntTree outerTree(m_loopOuter);
    outerTree.value() = 2;
    
    IntTree& inserted = innerTree.insert(outerTree);
    
    CPPUNIT_ASSERT_EQUAL(2, inserted.value());
}


