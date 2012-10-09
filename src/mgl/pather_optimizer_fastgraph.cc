#include "pather_optimizer_fastgraph.h"

namespace mgl {

void pather_optimizer_fastgraph::addPath(const OpenPath& path, 
        const PathLabel& label) {
    
}
void pather_optimizer_fastgraph::addPath(const Loop& loop, 
        const PathLabel& label) {
    
}
void pather_optimizer_fastgraph::addBoundary(const OpenPath& path) {
    for(OpenPath::const_iterator iter = path.fromStart(); 
            iter != path.end(); 
            ++iter) {
        OpenPath::const_iterator next = iter;
        ++next;
        if(next != path.end()) {
            boundaries.insert(libthing::LineSegment2(*iter, *next));
        }
    }
}
void pather_optimizer_fastgraph::addBoundary(const Loop& loop) {
    for(Loop::const_finite_cw_iterator iter = loop.clockwiseFinite(); 
            iter != loop.clockwiseEnd(); 
            ++iter) {
        boundaries.insert(loop.segmentAfterPoint(iter));
    }
}
void pather_optimizer_fastgraph::clearBoundaries() {
    boundaries = boundary_container();
}
void pather_optimizer_fastgraph::clearPaths() {
    graph.clear();
}
void pather_optimizer_fastgraph::optimizeInternal(LabeledOpenPaths& labeledpaths) {
    
}

pather_optimizer_fastgraph::entry_iterator& 
        pather_optimizer_fastgraph::entry_iterator::operator ++() {
    do { ++m_base; } while(m_base != m_end && !m_base->data().isEntry());
    return *this;
}
pather_optimizer_fastgraph::entry_iterator
        pather_optimizer_fastgraph::entry_iterator::operator ++(int) {
    entry_iterator copy = *this;
    ++*this;
    return copy;
}
pather_optimizer_fastgraph::node&
        pather_optimizer_fastgraph::entry_iterator::operator *() {
    return *m_base;
}
bool pather_optimizer_fastgraph::entry_iterator::operator ==(
        const entry_iterator& other) const {
    return m_base == other.m_base;
}
}

