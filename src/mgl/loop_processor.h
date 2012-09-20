#ifndef LOOP_PROCESSOR_H
#define	LOOP_PROCESSOR_H

#include "abstractable.h"
#include "loop_path.h"
#include "configuration.h"
#include "slicer_loops.h"

namespace mgl {

class LPConfig {
public:
    LPConfig(Scalar c = 0.0, Scalar dw = 1.0) 
            : coarseness(c), directionWeight(dw) {}
    Scalar coarseness;
    Scalar directionWeight;
};

class LoopProcessor : public Progressive {
public:
    LoopProcessor(const LPConfig& conf, ProgressBar* progress = NULL) 
            : Progressive(progress), lpCfg(conf) {}
    void processLoops(const LayerLoops& input, LayerLoops& output);
private:
    void processSingleLoop(const Loop& input, Loop& output);
    Loop processSingleLoop(const Loop& input);
    LPConfig lpCfg;
};

}


#endif	/* LOOPPROCESSOR_H */

