/* 
 * File:   loopProcessor.cc
 * Author: Dev
 * 
 * Created on September 20, 2012, 2:58 PM
 */

#include "loop_processor.h"
#include "log.h"

namespace mgl {

void LoopProcessor::processLoops(const LayerLoops& input, LayerLoops& output) {
    if(&input == &output) {
        //to prevent problems writing to the thing on which we work
        throw Exception("Loop Processor attempted to do in-place processing");
    }
    output.layerMeasure = input.layerMeasure;
    
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
	//insert the first two points
    output.insertPointBefore(*(current++),output.clockwiseEnd());
    output.insertPointBefore(*(current++),output.clockwiseEnd());
    
	Scalar cumulativeError = 0.0;
    
	for(; current != output.clockwiseEnd(); ++current) {
        Loop::finite_ccw_iterator last1(output.counterClockwiseFinite());
        Loop::const_finite_ccw_iterator last2(output.counterClockwiseFinite());
        const PointType& currentPoint = *current;
        PointType landingPoint = currentPoint;
		++last2;
		bool addPoint = true;
        try {
            const PointType& lp1 = *last1;
            const PointType& lp2 = *last2;
            const PointType unit = PointType(lp1 - lp2).unit();
            const Scalar component = (currentPoint - lp1).dotProduct(unit);
            const Scalar deviation = abs((currentPoint - lp1).crossProduct(unit));
            landingPoint = lp1 + unit*component;

            cumulativeError += deviation;

            addPoint = cumulativeError > lpCfg.coarseness;
        } catch(libthing::Exception mixup) {
            //we expect this to be something like a bad normalization
            Log::severe() << "ERROR: " << mixup.what() << std::endl;
        }
		
		if(addPoint) {
            output.insertPointBefore(currentPoint,output.clockwiseEnd());
			cumulativeError = 0;
		} else {
			*last1 = landingPoint * (1.0 - lpCfg.directionWeight) + 
					currentPoint * lpCfg.directionWeight;
		}
	}
}

Loop LoopProcessor::processSingleLoop(const Loop& input) {
        Loop output;
        processSingleLoop(input, output);
        return output;
    }

}

