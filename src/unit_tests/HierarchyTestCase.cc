/* 
 * File:   HierarchyTestCase.cc
 * Author: Dev
 * 
 * Created on January 2, 2013, 11:05 AM
 */
#include <list>
#include <limits>

#include "UnitTestUtils.h"
#include "HierarchyTestCase.h"
#include "mgl/pather_hierarchical.h"

using namespace mgl;

CPPUNIT_TEST_SUITE_REGISTRATION( HierarchyTestCase );

HierarchyTestCase::HierarchyTestCase() {
    std::list<Loop> curList;
    Loop curInset;
    curInset.insertPointBefore(Point2Type(-1.5,-1.5), curInset.clockwiseEnd());
    curInset.insertPointBefore(Point2Type(-1.5,1.5), curInset.clockwiseEnd());
    curInset.insertPointBefore(Point2Type(1.5,1.5), curInset.clockwiseEnd());
    curInset.insertPointBefore(Point2Type(1.5,-1.5), curInset.clockwiseEnd());
    curList.push_back(curInset);
    m_insets.push_back(curList);
    curList.clear();
    curInset.clear();
    curInset.insertPointBefore(Point2Type(-1.3,-1.3), curInset.clockwiseEnd());
    curInset.insertPointBefore(Point2Type(-1.3,1.3), curInset.clockwiseEnd());
    curInset.insertPointBefore(Point2Type(1.3,1.3), curInset.clockwiseEnd());
    curInset.insertPointBefore(Point2Type(1.3,-1.3), curInset.clockwiseEnd());
    curList.push_back(curInset);
    m_insets.push_back(curList);
    curList.clear();
    curInset.clear();
    curInset.insertPointBefore(Point2Type(-1.1,-1.1), curInset.clockwiseEnd());
    curInset.insertPointBefore(Point2Type(-1.1,1.1), curInset.clockwiseEnd());
    curInset.insertPointBefore(Point2Type(1.1,1.1), curInset.clockwiseEnd());
    curInset.insertPointBefore(Point2Type(1.1,-1.1), curInset.clockwiseEnd());
    curList.push_back(curInset);
    m_insets.push_back(curList);
    OpenPath curFill;
    curFill.appendPoint(Point2Type(-1.0,0.0));
    curFill.appendPoint(Point2Type(1.0,0.0));
    m_infill.push_back(curFill);
}
void HierarchyTestCase::setUp() {
    //do nothing
}
void HierarchyTestCase::testSanity() {
    class Config : public GrueConfig {
    public:
        Config() {
            doFixedLayerStart = true;
        }
    };
    std::cout << "Begin sanity test of hierarchical optimizer..." << std::endl;
    Config grueCfg;
    pather_hierarchical testPather(grueCfg);
    int curVal = 10;
    for(inset_table::const_iterator insetIter = m_insets.begin(); 
            insetIter != m_insets.end(); 
            ++insetIter) {
        CPPUNIT_ASSERT_NO_THROW(
                testPather.addPaths(*insetIter, PathLabel(PathLabel::TYP_INSET, 
                PathLabel::OWN_MODEL, curVal++)));
    }
    CPPUNIT_ASSERT_NO_THROW(
            testPather.addPaths(m_infill, PathLabel(PathLabel::TYP_INFILL, 
            PathLabel::OWN_MODEL, 5)));
    std::cout << "Output optimization..." << std::endl;
    std::list<LabeledOpenPath> output;
    CPPUNIT_ASSERT_NO_THROW(
            testPather.optimize(output));
    std::cout << "Output testing..." << std::endl;
    int lastVal = std::numeric_limits<int>::max();
    for(std::list<LabeledOpenPath>::const_iterator outIter = output.begin(); 
            outIter != output.end(); 
            ++outIter) {
        if(outIter->myLabel.myValue != 0) {
            CPPUNIT_ASSERT(outIter->myLabel.myValue <= lastVal);
            lastVal = outIter->myLabel.myValue;
        }
    }
    std::cout << "End sanity test of hierarchical optimizer..." << std::endl;
}

