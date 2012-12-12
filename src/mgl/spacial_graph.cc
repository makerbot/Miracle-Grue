#include <utility>

#include "spacial_graph_decl.h"

namespace mgl {

template <>
AABBox to_bbox<SpacialGraph::graph_node_reference>::bound(
        const SpacialGraph::graph_node_reference& value) {
    return AABBox((*value.first)[value.second].data().position());
}

void SpacialGraph::insertPath(const OpenPath& path, const PathLabel& label) {
    graph_type::node_index lastIndex;
    for(OpenPath::const_iterator iter = path.fromStart(); 
            iter != path.end(); 
            ++iter) {
        OpenPath::const_iterator nextIter = iter;
        ++nextIter;
        bool isFirst = iter == path.fromStart();
        bool isLast = nextIter == path.end();
        bool isEntry = isFirst || isLast;
        NodeData data(*iter, label, isEntry);
        graph_type::node_index currentIndex = createNode(data);
        if(!isFirst) {
            Point2Type lastPos = m_graph[lastIndex].data().position();
            Scalar squaredDistance = Point2Type(*iter - lastPos).squaredMagnitude();
            CostData cost(label, squaredDistance);
            m_graph[lastIndex].connect(m_graph[currentIndex], cost);
            m_graph[currentIndex].connect(m_graph[lastIndex], cost);
        }
        lastIndex = currentIndex;
    }
}

void SpacialGraph::insertPath(const Loop& loop, const PathLabel& label) {
    graph_type::node_index firstIndex, lastIndex;
    for(Loop::const_finite_cw_iterator iter = loop.clockwiseFinite(); 
            iter != loop.clockwiseEnd(); 
            ++iter) {
        Loop::const_finite_cw_iterator nextIter = iter;
        ++nextIter;
        bool isFirst = iter == loop.clockwiseFinite();
        bool isLast = nextIter == loop.clockwiseEnd();
        NodeData data(*iter, label, true);
        graph_type::node_index currentIndex = createNode(data);
        if(isFirst) {
            firstIndex = currentIndex;
        } else {
            Point2Type lastPos = m_graph[lastIndex].data().position();
            Scalar squaredDistance = (Point2Type(*iter) - lastPos).squaredMagnitude();
            CostData cost(label, squaredDistance);
            m_graph[lastIndex].connect(m_graph[currentIndex], cost);
            //close the loop
            if(isLast) {
                lastPos = m_graph[firstIndex].data().position();
                squaredDistance = (Point2Type(*iter) - lastPos).squaredMagnitude();
                cost = CostData(label, squaredDistance);
                m_graph[currentIndex].connect(m_graph[firstIndex], cost);
            }
        }
        lastIndex = currentIndex;
    }
}

SpacialGraph::entry_iterator SpacialGraph::entryBegin() {
    entry_iterator iter(m_graph.begin(), m_graph.end());
    if(!iter->data().isEntry())
        ++iter;
    return iter;
}

SpacialGraph::entry_iterator SpacialGraph::entryEnd() {
    return entry_iterator(m_graph.end(), m_graph.end());
}

void SpacialGraph::repr(std::ostream& out) {
    m_tree.repr(out);
}

void SpacialGraph::repr_svg(std::ostream& out) {
    m_tree.repr_svg(out);
}

SpacialGraph::graph_type::node_index SpacialGraph::createNode(
        const NodeData& data) {
    node& createdNode = m_graph.createNode(data);
    tree_type::iterator treeRef = m_tree.insert(
            graph_node_reference(&m_graph, createdNode.getIndex()));
    createdNode.data().m_treeIterator = treeRef;
    return createdNode.getIndex();
}

void SpacialGraph::destroyNode(graph_type::node_index index) {
    node& victimNode = m_graph[index];
    m_tree.erase(victimNode.data().m_treeIterator);
    m_graph.destroyNode(victimNode);
}

}
