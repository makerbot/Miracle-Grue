#include <cppunit/config/SourcePrefix.h>

#include <vector>
#include <list>
#include <sstream>

#include "FastgraphDeepTestCase.h"
#include "mgl/meshy.h"
#include "mgl/segmenter.h"
#include "mgl/slicer.h"
#include "mgl/loop_processor.h"
#include "mgl/regioner.h"
#include "mgl/pather_optimizer_fastgraph.h"
#include "mgl/pather.h"

using namespace mgl;

CPPUNIT_TEST_SUITE_REGISTRATION(FastgraphDeepTestCase);

void FastgraphDeepTestCase::testLoopOrdering() {
    class DerivedConfig : public GrueConfig {
    public:
        DerivedConfig() {
            infillDensity = 0.1;
            nbOfShells = 2;
            insetDistanceMultiplier = 0.9;
            roofLayerCount = 4;
            floorLayerCount = 4;
            layerWidthRatio = 1.45;
            preCoarseness = 0.8;
            coarseness = 0.5;
            directionWeight = 0.8;
            doGraphOptimization = true;
            rapidMoveFeedRateXY = 100;
            rapidMoveFeedRateZ = 23;
            doRaft = false;
            doSupport = false;
            firstLayerZ = 0.0;
            layerH = 0.26;
        }
    };
    
    std::cout << "Unit test!" << std::endl;
    
    DerivedConfig grueCfg;
    
    Meshy mesh(grueCfg);
    
    std::cout << "Reading mesh" << std::endl;
    mesh.readStlFile("stl/planetgeartest.stl");
    Segmenter segment(grueCfg);
    
    std::cout << "Segmenting mesh" << std::endl;
    segment.tablaturize(mesh);
    
    Slicer slice(grueCfg);
    
    LayerLoops sliced, diced;
    
    std::cout << "Slicing mesh" << std::endl;
    slice.generateLoops(segment, sliced);
    
    LoopProcessor process(grueCfg);
    
    std::cout << "Processing loops" << std::endl;
    process.processLoops(sliced, diced);
    
    Regioner region(grueCfg);
    
    Limits limits = mesh.readLimits();
	Grid grid;
    LayerMeasure& layerMeasure = diced.layerMeasure;
    RegionList regions;
    
    std::cout << "Regioning" << std::endl;
    region.generateSkeleton(diced, layerMeasure, regions, limits, grid);
    
    pather_optimizer_fastgraph optimizator(grueCfg);
    
    LayerRegions& layerRegions = regions.front();
    
    optimizator.addBoundaries(layerRegions.outlines);	
    
    if(true) {
        int currentShell = LayerPaths::Layer::ExtruderLayer::INSET_LABEL_VALUE;
        int shellSequence = 0;
        const std::list<LoopList>& insetLoops = layerRegions.insetLoops;
        for(std::list<LoopList>::const_iterator listIter = insetLoops.begin(); 
                listIter != insetLoops.end(); 
                ++listIter, ++shellSequence) {
            int shellVal = currentShell;
//                shellVal = shellSequence !=0 && grueCfg.get_nbOfShells() > 1 ? 
//                        shellVal : 
//                        LayerPaths::Layer::ExtruderLayer::OUTLINE_LABEL_VALUE;
            std::cout << "Adding shells " << shellVal << std::endl;
            optimizator.addPaths(*listIter, 
                    PathLabel(PathLabel::TYP_INSET, 
                    PathLabel::OWN_MODEL, shellVal));
            ++currentShell;
        }
    }
    
    std::stringstream output;
    optimizator.buckets.front().m_hierarchy.repr(output);
    std::string expected = (
            "\n|L-0-(1)--1\n||L-47-(1)-10\n|||L-47-(7)-11\n||||L-13-(1)-11\n|||||L-13-(0)-10\n||||L-18-(1)-11\n|||||L-18-(0)-10\n||||L-19-(1)-11\n|||||L-19-(0)-10\n||||L-26-(1)-11\n|||||L-26-(0)-10\n||||L-14-(1)-11\n|||||L-14-(0)-10\n||||L-17-(1)-11\n|||||L-17-(0)-10\n||||L-14-(1)-11\n|||||L-14-(0)-10\n"
            );
    std::cout << "Testing that hierarchy matches expected layout" << std::endl;
    CPPUNIT_ASSERT_EQUAL(expected, output.str());
}

void FastgraphDeepTestCase::displayBucket(mgl::pather_optimizer_fastgraph::bucket& 
        bucket) {
    bucket.m_hierarchy.repr(std::cerr);
    pather_optimizer_fastgraph::bucket_list::iterator iter;
    for(iter = bucket.m_children.begin(); 
            iter != bucket.m_children.end(); 
            ++iter ) {
        displayBucket(*iter);
    }
}
