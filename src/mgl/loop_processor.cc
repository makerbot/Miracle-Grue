/* 
 * File:   loopProcessor.cc
 * Author: Dev
 * 
 * Created on September 20, 2012, 2:58 PM
 */

#include <vector>

#include "loop_processor.h"
#include "log.h"
#include "loop_utils.h"

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
            Loop processed;
            smooth(*loopIter, grueCfg.get_coarseness(), processed, 
                    grueCfg.get_directionWeight());
            currentOutputLayer.push_back(processed);
        }
        
        output.push_back(currentOutputLayer);
        tick();
    }
}

}

