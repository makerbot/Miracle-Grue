#ifndef MGL_UNIT_TEST_FASTGRAPHDEEPTESTCASE_H_
#define	MGL_UNIT_TEST_FASTGRAPHDEEPTESTCASE_H_

#include <cppunit/extensions/HelperMacros.h>
#define FASTGRAPH_FRIENDS_LIST
#include "mgl/pather_optimizer_fastgraph.h"

class FastgraphDeepTestCase : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE ( FastgraphDeepTestCase );
    CPPUNIT_TEST( testLoopOrdering );
    CPPUNIT_TEST_SUITE_END();
public:
    void setUp() {}
protected:
    void testLoopOrdering();
private:
    void displayBucket(mgl::pather_optimizer_fastgraph::bucket& bucket);
};

#endif	/* MGL_UNIT_TEST_FASTGRAPHDEEPTESTCASE_H_ */

