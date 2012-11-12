#include <cppunit/config/SourcePrefix.h>
#include "SlicerOutputTestCase.h"
#include "mgl/mgl.h"
#include "mgl/meshy.h"
#include "mgl/slicer.h"
#include "mgl/miracle.h"

CPPUNIT_TEST_SUITE_REGISTRATION( SlicerOutputTestCase );

using namespace std;
using namespace mgl;


string inputsDir;



void SlicerOutputTestCase::setUp() {
	cout << "\nSetting up inputs path" << endl;
	MyComputer computer;
	char pathsep = computer.fileSystem.getPathSeparatorCharacter();
	inputsDir = string("inputs") + pathsep;
}

void SlicerOutputTestCase::testLoopLayer(){
    class MeshCfg : public GrueConfig {
    public:
        MeshCfg() {
            doPutModelOnPlatform = true;
        }
    };
    MeshCfg grueCfg;
	Meshy mesh(grueCfg);
	mesh.readStlFile((inputsDir + "20mm_Calibration_Box.stl").c_str());
//	  SlicerConfig slicerCfg;
//    slicerCfg.firstLayerZ = 0;
//    slicerCfg.layerH = 0.20;
//	  Segmenter segmenter(slicerCfg.firstLayerZ, slicerCfg.layerH);
    Segmenter segmenter(grueCfg);
	Slicer slicer(grueCfg);
	segmenter.tablaturize(mesh);
	LayerLoops layerloops(grueCfg.get_firstLayerZ(), grueCfg.get_layerH());
	slicer.generateLoops(segmenter, layerloops);
	
	Loop expected;
	Loop::cw_iterator iter = expected.clockwiseEnd();
	iter = expected.insertPointAfter(Point2Type(10,9.8), iter);
	iter = expected.insertPointAfter(Point2Type(10,-10), iter);
	iter = expected.insertPointAfter(Point2Type(9.8,-10), iter);
	iter = expected.insertPointAfter(Point2Type(-10,-10), iter);
	iter = expected.insertPointAfter(Point2Type(-10,-9.8), iter);
	iter = expected.insertPointAfter(Point2Type(-10,10), iter);
	iter = expected.insertPointAfter(Point2Type(-9.8,10), iter);
	iter = expected.insertPointAfter(Point2Type(10,10), iter);
	iter = expected.insertPointAfter(Point2Type(10,10), iter);
	
	CPPUNIT_ASSERT_MESSAGE("No layers in layerloops", 
			!layerloops.empty());
	CPPUNIT_ASSERT_MESSAGE("No loops in first layer", 
			!layerloops.begin()->empty());
	CPPUNIT_ASSERT_MESSAGE("First loop in first layer has no segments", 
			layerloops.begin()->begin()->entryBegin() != 
			layerloops.begin()->begin()->entryEnd());
	for(Loop::entry_iterator expectediter = expected.entryBegin(), 
			actual = layerloops.begin()->begin()->entryBegin();
			expectediter != expected.entryEnd() && 
			actual != layerloops.begin()->begin()->entryEnd(); 
			++expectediter, ++actual){
		cout << "- Expected: " << *expectediter << endl;
		cout << "- Actual  : " << *actual << endl;
		CPPUNIT_ASSERT_EQUAL(*expectediter, *actual);
	}
	
//	cout << "Testing LoopLayer" << endl;
//	
//	unsigned lcount = 0;
//	for(LayerLoops::layer_iterator layerit = layerloops.begin(); 
//			layerit != layerloops.end(); 
//			++layerit){
//		cout << "\nLayer: \t" << ++lcount << endl << endl;
//		unsigned count = 0;
//		for(LayerLoops::loop_iterator loopit = layerit->begin(); 
//				loopit != layerit->end(); 
//				++loopit){
//			cout << "Loop: \t"  << ++count << endl;
//			for(Loop::entry_iterator entit = loopit->entryBegin(); 
//					entit != loopit->entryEnd(); 
//					++entit){
//				cout << *entit << endl;
//			}
//			cout << endl;
//		}
//	}
	
}



