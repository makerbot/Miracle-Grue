#include "loop_path_decl.h"
#include "loop_path_iter_impl.h"

namespace mgl {

LoopPath::LoopPath(const Loop& p, Loop::const_cw_iterator s, 
		Loop::const_ccw_iterator r) 
        : parent(&p), start(s), rstart(r) {}

LoopPath::LoopPath(const Loop& p, Loop::cw_iterator s, 
		Loop::ccw_iterator r) 
        : parent(&p), start(s), rstart(r) {}

LoopPath::LoopPath(const LoopPath& orig) 
		: parent(orig.parent), start(orig.start), rstart(orig.rstart) {}

}





