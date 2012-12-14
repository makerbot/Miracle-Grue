#include <utility>
#include <vector>
#include <algorithm>

#include "spacial_graph_decl.h"

namespace mgl {

template <>
AABBox to_bbox<SpacialGraph::graph_node_reference>::bound(
        const SpacialGraph::graph_node_reference& value) {
    return AABBox((*value.first)[value.second].data().position());
}

const Scalar SpacialGraph::NEAREST_QUICKTEST_THRESHOLD = 10.0;

SpacialGraph::SpacialGraph() {}

SpacialGraph::SpacialGraph(const SpacialGraph& other) 
        : m_graph(other.m_graph) {
    for(graph_type::forward_node_iterator iter = m_graph.begin(); 
            iter != m_graph.end(); 
            ++iter) {
        iter->data().m_treeIterator = m_tree.insert(
                graph_node_reference(&m_graph, iter->getIndex()));
    }
}

SpacialGraph& SpacialGraph::operator =(const SpacialGraph& other) {
    if(&other == this)
        return *this;
    m_tree.clear();
    m_graph = other.m_graph;
    for(graph_type::forward_node_iterator iter = m_graph.begin(); 
            iter != m_graph.end(); 
            ++iter) {
        iter->data().m_treeIterator = m_tree.insert(
                graph_node_reference(&m_graph, iter->getIndex()));
    }
    return *this;
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
    if(iter != entryEnd() && !iter->data().isEntry())
        ++iter;
    return iter;
}

SpacialGraph::entry_iterator SpacialGraph::entryEnd() {
    return entry_iterator(m_graph.end(), m_graph.end());
}

bool SpacialGraph::empty() const {
    return m_graph.empty();
}

void SpacialGraph::clear() {
    m_graph.clear();
    m_tree.clear();
}

void SpacialGraph::repr(std::ostream& out) {
    m_tree.repr(out);
}

void SpacialGraph::repr_svg(std::ostream& out) {
    m_tree.repr_svg(out);
}

class NearestDistanceComparator {
public:
    NearestDistanceComparator(const Point2Type& pt) : m_point(pt) {}
    bool operator ()(const SpacialGraph::graph_node_reference& lhs, 
            const SpacialGraph::graph_node_reference& rhs) const {
        const SpacialGraph::graph_type& lgraph = *lhs.first;
        const SpacialGraph::graph_type& rgraph = *rhs.first;
        return operator ()(lgraph[lhs.second], rgraph[rhs.second]);
    }
    bool operator ()(const SpacialGraph::node& lhs, 
            const SpacialGraph::node& rhs) const {
        return (lhs.data().position() - m_point).squaredMagnitude() < 
                (rhs.data().position() - m_point).squaredMagnitude();
    }
private:
    const Point2Type& m_point;
};

SpacialGraph::graph_type::node_index SpacialGraph::findNearestNode(
        const Point2Type& point) {    
    std::vector<graph_node_reference> candidates;
    m_tree.search(candidates, BBoxFilter(AABBox(point).adjusted(
            Point2Type(-NEAREST_QUICKTEST_THRESHOLD, 
            -NEAREST_QUICKTEST_THRESHOLD), 
            Point2Type(NEAREST_QUICKTEST_THRESHOLD, 
            NEAREST_QUICKTEST_THRESHOLD))));
    if(!candidates.empty()) {
        //select from the small data set we got from the r-tree
        return std::min_element(candidates.begin(), candidates.end(), 
                NearestDistanceComparator(point))->second;
    } else {
        //search all the nodes
        return std::min_element(m_graph.begin(), 
                m_graph.end(), NearestDistanceComparator(point))->getIndex();
    }
}

void SpacialGraph::smartAppendPoint(Point2Type point, PathLabel label, 
        LabeledOpenPaths& labeledpaths, LabeledOpenPath& path, 
        Point2Type& entryPoint) {
    if(path.myLabel.isInvalid()) {
        path.myLabel = label;
        //path.myPath.clear();
        path.myPath.appendPoint(point);
    } else if(path.myLabel == label) {
        path.myPath.appendPoint(point);
    } else {
        Point2Type prevPoint = path.myPath.empty() ? point : *path.myPath.fromEnd();
        smartAppendPath(labeledpaths, path);
        path.myLabel = label;
        path.myPath.appendPoint(prevPoint);
        path.myPath.appendPoint(point);
    }
    entryPoint = point;
}

void SpacialGraph::smartAppendPath(LabeledOpenPaths& labeledpaths, 
        LabeledOpenPath& path) {
    if(path.myLabel.isValid() && path.myPath.size() > 1) 
        labeledpaths.push_back(path);
    path.myLabel = PathLabel();
    path.myPath.clear();
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
