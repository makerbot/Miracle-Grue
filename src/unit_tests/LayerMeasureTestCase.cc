#include "UnitTestUtils.h"
#include "LayerMeasureTestCase.h"

#include "mgl/mgl.h"

using namespace std;
using namespace mgl;
using namespace libthing;

CPPUNIT_TEST_SUITE_REGISTRATION( LayerMeasureTestCase );

void LayerMeasureTestCase::setUp() {
	cout << endl << "No setup" << endl;
}

void LayerMeasureTestCase::testLayer0() {
	LayerMeasure layerMeasure(0.0, 0.27);
	CPPUNIT_ASSERT_EQUAL(0.0, layerMeasure.getLayerPosition(0));
}

void LayerMeasureTestCase::testCreatingLayers() {
	LayerMeasure layerMeasure(0.0, 0.27);
	
	layer_measure_index_t first; 
	layer_measure_index_t second; 
	first = layerMeasure.createAttributes(
			LayerMeasure::LayerAttributes(0.27, 0.27));
	second = layerMeasure.createAttributes(
			LayerMeasure::LayerAttributes(0.54, 0.27));
	CPPUNIT_ASSERT_EQUAL(0.27, layerMeasure.getLayerPosition(first));
	CPPUNIT_ASSERT_EQUAL(0.54, layerMeasure.getLayerPosition(second));
}

void LayerMeasureTestCase::testOffset() {
	LayerMeasure layerMeasure(0.0, 0.27);
	
	layer_measure_index_t first; 
	layer_measure_index_t second; 
	first = layerMeasure.createAttributes(
			LayerMeasure::LayerAttributes(0.27, 0.27));
	second = layerMeasure.createAttributes(
			LayerMeasure::LayerAttributes(0.54, 0.27));
	layerMeasure.getLayerAttributes(0).delta += 0.27;
	CPPUNIT_ASSERT_EQUAL(0.54 + 0.27, layerMeasure.getLayerPosition(second));
	layerMeasure.getLayerAttributes(second).base = first;
	CPPUNIT_ASSERT_EQUAL(0.54 + 0.27 + 0.27, layerMeasure.getLayerPosition(second));
}



