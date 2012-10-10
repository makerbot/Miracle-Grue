#include "pather_optimizer_fastgraph.h"

namespace mgl {

void pather_optimizer_fastgraph::addPath(const OpenPath& path, 
        const PathLabel& label) {
    node_index last = -1;
    for(OpenPath::const_iterator iter = path.fromStart(); 
            iter != path.end(); 
            ++iter) {
        OpenPath::const_iterator next = iter;
        ++next;
        if(iter == path.fromStart()) {
            last = graph.createNode(NodeData(*iter, 
                    label.myValue, true)).getIndex();
        }
        if(next != path.end()) {
            OpenPath::const_iterator future = next;
            ++future;
            node& curNode = graph.createNode(NodeData(*next, 
                    label.myValue, future==path.end()));
            node& lastNode = graph[last];
            libthing::LineSegment2 connection( 
                    curNode.data().getPosition(), 
                    lastNode.data().getPosition());
            PointType normal = (connection.b - connection.a).unit();
            Scalar distance = connection.length();
            Cost frontCost(label, distance, normal);
            Cost backCost(label, distance, normal * -1.0);
            curNode.connect(lastNode, backCost);
            lastNode.connect(curNode, frontCost);
            last = curNode.getIndex();
        }
    }
//    std::cout << "Path Size: " << path.size() << std::endl;
//    std::cout << "Graph Nodes: " << graph.count() << std::endl;
}
void pather_optimizer_fastgraph::addPath(const Loop& loop, 
        const PathLabel& label) {
    node_index last = -1;
    node_index first = -1;
    for(Loop::const_finite_cw_iterator iter = loop.clockwiseFinite(); 
            iter != loop.clockwiseEnd(); 
            ++iter) {
        Loop::const_finite_cw_iterator next = iter;
        ++next;
        if(iter == loop.clockwiseFinite()) {
            last = graph.createNode(NodeData(*iter, 
                    label.myValue, true)).getIndex();
            first = last;
        }
        if(next != loop.clockwiseFinite()) {
            NodeData curNodeData(*next, label.myValue, true);
            node& curNode = graph.createNode(curNodeData);
            node& lastNode = graph[last];
            libthing::LineSegment2 connection( 
                    curNode.data().getPosition(), 
                    lastNode.data().getPosition());
            PointType normal = (connection.b - connection.a).unit();
            Scalar distance = connection.length();
            Cost frontCost(label, distance, normal);
            Cost backCost(label, distance, normal * -1.0);
            curNode.connect(lastNode, backCost);
            lastNode.connect(curNode, frontCost);
            last = curNode.getIndex();
        }
    }
    node& lastNode = graph[last];
    node& curNode = graph[first];
    libthing::LineSegment2 connection( 
            curNode.data().getPosition(), 
            lastNode.data().getPosition());
    PointType normal = (connection.b - connection.a).unit();
    Scalar distance = connection.length();
    Cost frontCost(label, distance, normal);
    Cost backCost(label, distance, normal * -1.0);
    curNode.connect(lastNode, backCost);
    lastNode.connect(curNode, frontCost);
//    std::cout << "Path Size: " << loop.size() << std::endl;
//    std::cout << "Graph Nodes: " << graph.count() << std::endl;
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
pather_optimizer_fastgraph::entry_iterator 
        pather_optimizer_fastgraph::entryBegin() {
    return entry_iterator(graph.begin(), graph.end());
}
pather_optimizer_fastgraph::entry_iterator
        pather_optimizer_fastgraph::entryEnd() {
    return entry_iterator(graph.end(), graph.end());
}




}

