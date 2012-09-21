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

LoopProcessor::PROC_RESULT LoopProcessor::processPoints(
        const PointType& lp1, 
        const PointType& lp2, 
        const PointType& cp, 
        Scalar& cumDeviation, 
        PointType& output) {
    PointType ldelta = lp1 - lp2;
    PointType unit;
    try{
        unit = ldelta.unit();
    } catch (const libthing::Exception& e) {
        output = cp;
        return PROC_ADD;
    }
    PointType delta = cp - lp1;
    Scalar component = delta.dotProduct(unit);
    Scalar deviation = delta.crossProduct(unit);
    deviation = deviation < 0 ? -deviation : deviation;
    PointType landingPoint = lp1 + unit*component;
    cumDeviation += deviation;
    if(cumDeviation > lpCfg.coarseness) {
        output = cp;
        cumDeviation = 0;
        return PROC_ADD;
    } else {
        output = landingPoint;
        return PROC_REPLACE;
    }
}

void LoopProcessor::processSingleLoop(const Loop& input, Loop& output) {
    if(lpCfg.coarseness == 0 || input.size() <= 3) {
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
        const PointType& lp1 = *last1;
        const PointType& lp2 = *last2;
        PointType result;
        PROC_RESULT rslt = processPoints(lp1, lp2, currentPoint, cumulativeError, 
                result);
        if(rslt == PROC_ADD) {
            tmpPoints.push_back(currentPoint);
        } else {
            tmpPoints.back() = result;
        }
	}
    for(std::vector<PointType>::const_iterator iter = tmpPoints.begin(); 
            iter != tmpPoints.end(); 
            ++iter) {
        output.insertPointBefore(*iter, output.clockwiseEnd());
    }
}

Loop LoopProcessor::processSingleLoop(const Loop& input) {
        Loop output;
        processSingleLoop(input, output);
        return output;
    }

}

