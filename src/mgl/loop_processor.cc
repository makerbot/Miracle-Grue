/* 
 * File:   loopProcessor.cc
 * Author: Dev
 * 
 * Created on September 20, 2012, 2:58 PM
 */

#include <vector>

#include "loop_processor.h"
#include "log.h"

namespace mgl {

void LoopProcessor::processLoops(const LayerLoops& input, LayerLoops& output) {
    if(&input == &output) {
        //to prevent problems writing to the thing on which we work
        throw Exception("Loop Processor attempted to do in-place processing");
    }
    output.layerMeasure = input.layerMeasure;
    initProgress("Loop Processing", input.size());
    
    for(LayerLoops::const_layer_iterator layerIter = input.begin(); 
            layerIter != input.end(); 
            ++layerIter) {
        const LayerLoops::Layer& currentInputLayer = *layerIter; 
        LayerLoops::Layer currentOutputLayer(currentInputLayer.getIndex());
        for(LayerLoops::const_loop_iterator loopIter = currentInputLayer.begin(); 
                loopIter != currentInputLayer.end(); 
                ++loopIter) {
            currentOutputLayer.push_back(processSingleLoop(*loopIter));
        }
        output.push_back(currentOutputLayer);
        tick();
    }
}

void LoopProcessor::processSingleLoop(const Loop& input, Loop& output) {
    if(lpCfg.coarseness == 0) {
		output = input;
        return;
    }
	if(input.size() <= 3) {
        output = input;
		return;
    }
	Loop::const_finite_cw_iterator current;
	current = input.clockwiseFinite();
    
    std::vector<PointType> tmpPoints;
    tmpPoints.push_back(*(current++));
    tmpPoints.push_back(*(current++));
    
	//insert the first two points
    
	Scalar cumulativeError = 0.0;
    
	for(; current != input.clockwiseEnd(); ++current) {
        std::vector<PointType>::reverse_iterator last1 = 
                tmpPoints.rbegin();
        std::vector<PointType>::const_reverse_iterator last2 = 
                tmpPoints.rbegin();
        ++last2;
        const PointType& currentPoint = *current;
        PointType landingPoint = currentPoint;
		bool addPoint = true;
        try {
            const PointType& lp1 = *last1;
            const PointType& lp2 = *last2;
//            std::cout << "LP1 "  << lp1 << "\nLP2 " << lp2 
//                    << "\nCPN " << currentPoint << std::endl;
            const PointType unit = PointType(lp1 - lp2).unit();
            const Scalar component = (currentPoint - lp1).dotProduct(unit);
            const Scalar deviation = abs((currentPoint - lp1).crossProduct(unit));
            
            landingPoint = lp1 + unit*component;

            cumulativeError += deviation;
            
//            std::cout << "DEV " << deviation << std::endl << 
//                    "CUM " << cumulativeError << std::endl << std::endl;

            addPoint = cumulativeError > lpCfg.coarseness;
        } catch(libthing::Exception mixup) {
            //we expect this to be something like a bad normalization
            Log::severe() << "ERROR: " << mixup.what() << std::endl;
        }
		
		if(addPoint) {
            output.insertPointBefore(currentPoint,output.clockwiseEnd());
            tmpPoints.push_back(currentPoint);
			cumulativeError = 0;
		} else {
			*last1 = landingPoint * (1.0 - lpCfg.directionWeight) + 
					currentPoint * lpCfg.directionWeight;
		}
        for(std::vector<PointType>::const_iterator iter = tmpPoints.begin(); 
                iter != tmpPoints.end(); 
                ++iter) {
            output.insertPointBefore(*iter, output.clockwiseEnd());
        }
	}
}

Loop LoopProcessor::processSingleLoop(const Loop& input) {
        Loop output;
        processSingleLoop(input, output);
        return output;
    }

}

