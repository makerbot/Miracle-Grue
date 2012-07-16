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

LoopPath& LoopPath::operator =(const LoopPath& orig) {
	if(this != &orig) {
		parent = orig.parent;
		start = orig.start;
		rstart = orig.rstart;
	}
	return *this;
}

LoopPath::iterator LoopPath::end() {
	return iterator(parent->clockwiseEnd(), *this); 
}

LoopPath::const_iterator LoopPath::end() const {
	return const_iterator(parent->clockwiseEnd(), *this); 
}

LoopPath::reverse_iterator LoopPath::rend() {
	return reverse_iterator(parent->counterClockwiseEnd(), *this); 
}

LoopPath::const_reverse_iterator LoopPath::rend() const  {
	return const_reverse_iterator(
			parent->counterClockwiseEnd(), *this); 
}

LoopPath::iterator LoopPath::fromStart() { 
	return iterator(start, *this); 
}

LoopPath::const_iterator LoopPath::fromStart() const { 
	return const_iterator(iterator(start, *this)); 
}

LoopPath::reverse_iterator LoopPath::fromEnd() { 
	return reverse_iterator(rstart, *this); 
}

LoopPath::const_reverse_iterator LoopPath::fromEnd() const { 
	return const_reverse_iterator(reverse_iterator(rstart, *this)); 
}

LoopPath::iterator LoopPath::getSuspendedPoints() { 
	return fromStart(); 
}

LoopPath::const_iterator LoopPath::getSuspendedPoints() const { 
	return fromStart(); 
}

}





