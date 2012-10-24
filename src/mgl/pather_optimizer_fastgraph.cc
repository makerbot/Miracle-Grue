#include <list>
#include <vector>

#include "pather_optimizer_fastgraph.h"
#include "mgl.h"

namespace mgl {

void pather_optimizer_fastgraph::addPath(const OpenPath& path, 
        const PathLabel& label) {
    sortBuckets();
    node_index last = -1;
    bucket_list::iterator bucketIter = pickBucket(*path.fromStart());
    if(bucketIter == buckets.end()) {
        throw GraphException("No bucket for path!");
    }
    bucket& currentBucket = *bucketIter;
    graph_type& currentGraph = currentBucket.m_graph;
    for(OpenPath::const_iterator iter = path.fromStart(); 
            iter != path.end(); 
            ++iter) {
        OpenPath::const_iterator next = iter;
        ++next;
        if(iter == path.fromStart()) {
            last = currentGraph.createNode(NodeData(*iter, 
                    label, true)).getIndex();
        }
        if(next != path.end()) {
            OpenPath::const_iterator future = next;
            ++future;
            node& curNode = currentGraph.createNode(NodeData(*next, 
                    label, future==path.end()));
            node& lastNode = currentGraph[last];
            Segment2Type connection( 
                    curNode.data().getPosition(), 
                    lastNode.data().getPosition());
            Point2Type normal;
            try {
                normal = (connection.b - connection.a).unit();
            } catch (const GeometryException& le) {}
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
    sortBuckets();
    node_index last = -1;
    node_index first = -1;
    bucket_list::iterator bucketIter = pickBucket(*loop.clockwise());
    if(bucketIter == buckets.end()) {
        throw GraphException("No bucket for path!");
    }
    bucket& currentBucket = *bucketIter;
    graph_type& currentGraph = currentBucket.m_graph;
    for(Loop::const_finite_cw_iterator iter = loop.clockwiseFinite(); 
            iter != loop.clockwiseEnd(); 
            ++iter) {
        Loop::const_finite_cw_iterator next = iter;
        ++next;
        if(iter == loop.clockwiseFinite()) {
            last = currentGraph.createNode(NodeData(*iter, 
                    label, true)).getIndex();
            first = last;
        }
        if(next != loop.clockwiseEnd()) {
            NodeData curNodeData(*next, label, true);
            node& curNode = currentGraph.createNode(curNodeData);
            node& lastNode = currentGraph[last];
            Segment2Type connection( 
                    curNode.data().getPosition(), 
                    lastNode.data().getPosition());
            Point2Type normal;
            try {
                normal = (connection.b - connection.a).unit();
            } catch (const GeometryException& le) {}
            Scalar distance = connection.length();
            Cost frontCost(label, distance, normal);
            //Cost backCost(label, distance, normal * -1.0);
            //curNode.connect(lastNode, backCost);
            lastNode.connect(curNode, frontCost);
            last = curNode.getIndex();
        }
    }
    node& lastNode = currentGraph[last];
    node& curNode = currentGraph[first];
    Segment2Type connection( 
            curNode.data().getPosition(), 
            lastNode.data().getPosition());
    Point2Type normal;
    try {
        normal = (connection.b - connection.a).unit();
    } catch (const GeometryException& le) {}
    Scalar distance = connection.length();
    Cost frontCost(label, distance, normal);
    //Cost backCost(label, distance, normal * -1.0);
    //curNode.connect(lastNode, backCost);
    lastNode.connect(curNode, frontCost);
}
void pather_optimizer_fastgraph::addBoundary(const OpenPath& path) {
    for(OpenPath::const_iterator iter = path.fromStart(); 
            iter != path.end(); 
            ++iter) {
        OpenPath::const_iterator next = iter;
        ++next;
        if(next != path.end()) {
            m_boundaries.insert(Segment2Type(*iter, *next));
        }
    }
}
void pather_optimizer_fastgraph::addBoundary(const Loop& loop) {
    bucketsSorted = false;
    bucket_list::iterator iter = buckets.insert(buckets.end(), bucket());
    bucket& destBucket = *iter;
    for(Loop::const_finite_cw_iterator iter = loop.clockwiseFinite(); 
            iter != loop.clockwiseEnd(); 
            ++iter) {
        Segment2Type segment = loop.segmentAfterPoint(iter);
        boundaryLimits.expandTo(segment.a);
        boundaryLimits.expandTo(segment.b);
        m_boundaries.insert(segment);
        destBucket.m_bounds.insert(segment);
        destBucket.m_testPoint = segment.a;
    }
}
void pather_optimizer_fastgraph::clearBoundaries() {
    m_boundaries = boundary_container();
    buckets.clear();
    boundaryLimits.reset();
}
void pather_optimizer_fastgraph::clearPaths() {
    m_graph.clear();
    for(bucket_list::iterator iter = buckets.begin(); 
            iter != buckets.end(); 
            ++iter) {
        iter->m_graph.clear();
    }
}
void pather_optimizer_fastgraph::sortBuckets() {
    if(bucketsSorted)
        return;
    bucketsSorted = true;
    //point at infinity (outside our limits)
    Point2Type infinityPoint(boundaryLimits.bottom_left() - Point2Type(20,20));
    //determine how many buckets each is inside of
    for(bucket_list::iterator currentIter = buckets.begin(); 
            currentIter != buckets.end(); 
            ++currentIter) {
        Segment2Type currentLine(infinityPoint, 
                currentIter->m_testPoint);
        currentIter->m_insideCount = 0;
        for(bucket_list::const_iterator testIter = buckets.begin(); 
                testIter != buckets.end();
                ++testIter) {
            if(testIter == currentIter)
                continue;   //no self tests
            size_t intersectionsCount = countIntersections(currentLine, 
                    testIter->m_bounds);
            currentIter->m_insideCount += intersectionsCount & 1;
        }
    }
    //sort by that number
    buckets.sort(bucketSorter());
}
pather_optimizer_fastgraph::entry_iterator& 
        pather_optimizer_fastgraph::entry_iterator::operator ++() {
    if(m_base != m_end) do { ++m_base; } while(m_base != m_end && !m_base->data().isEntry());
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
        pather_optimizer_fastgraph::entryBegin(graph_type& graph) {
    entry_iterator ret(graph.begin(), graph.end());
    if(ret.m_base != ret.m_end && !ret->data().isEntry())
        ++ret;
    return ret;
}
pather_optimizer_fastgraph::entry_iterator
        pather_optimizer_fastgraph::entryEnd(graph_type& graph) {
    return entry_iterator(graph.end(), graph.end());
}
Scalar pather_optimizer_fastgraph::splitPaths(multipath_type& destionation, 
        const LabeledOpenPaths& source) {
    Point2Type lastPoint(std::numeric_limits<Scalar>::min(), 
            std::numeric_limits<Scalar>::min());
    Scalar ret = 0;
    for(LabeledOpenPaths::const_iterator iter = source.begin(); 
            iter != source.end(); 
            ++iter) {
        if(iter->myPath.size() < 2) 
            continue;
        if(*(iter->myPath.fromStart()) != lastPoint) {
            if(!destionation.empty()) {
                ret += (*(iter->myPath.fromStart()) - lastPoint).magnitude();
            }
            destionation.push_back(LabeledOpenPaths());
        }
        destionation.back().push_back(*iter);
        lastPoint = *destionation.back().back().myPath.fromEnd();
    }
    return ret;
}
size_t pather_optimizer_fastgraph::countIntersections(Segment2Type& line, 
        const boundary_container& boundContainer) {
    typedef std::vector<Segment2Type> result_type;
    result_type result;
    size_t count = 0;
    boundContainer.search(result, LineSegmentFilter(line));
    for(result_type::const_iterator iter = result.begin(); 
            iter != result.end(); 
            ++iter) {
        if(iter->intersects(line))
            ++count;
    }
    return count;
}

pather_optimizer_fastgraph::bucket_list::iterator 
        pather_optimizer_fastgraph::pickBucket(Point2Type point) {
    Segment2Type testLine(point, 
            boundaryLimits.bottom_left() - Point2Type(20,20));
    bucket_list::iterator iter = buckets.begin();
    for(; 
            iter != buckets.end(); 
            ++iter) {
        size_t intersections = countIntersections(testLine, 
                iter->m_bounds);
        if(intersections & 1) { //if odd, then inside
            break;
        }
    }
    return iter;
    std::cout << "Point Location: " << point << std::endl;
    std::cout << "Bounds: " << boundaryLimits.bottom_left() <<
            boundaryLimits.top_right() << std::endl;
    throw GraphException("Thing did not fall into any bucket!");
}




}

