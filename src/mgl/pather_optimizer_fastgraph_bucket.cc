#include <list>

#include "pather_optimizer_fastgraph.h"

namespace mgl {

#define BUCKET pather_optimizer_fastgraph::bucket

BUCKET::bucket(Point2Type testPoint) 
        : m_testPoint(testPoint), m_empty(true) {}
BUCKET::bucket(const Loop& loop)
        : m_testPoint(*loop.clockwise()), m_empty(false) {
    for(Loop::const_finite_cw_iterator loopIter = loop.clockwiseFinite(); 
            loopIter != loop.clockwiseEnd(); 
            ++loopIter) {
        Segment2Type line = loop.segmentAfterPoint(loopIter);
        insertBoundary(line);
        insertNoCross(line);
    }
    m_loop = loop;
}
bool BUCKET::contains(Point2Type point) const {
    Segment2Type testLine(m_infinitePoint, point);
    bool result = (countIntersections(testLine, m_bounds) & 1) != 0;  //even-odd test
    return result;
}
bool BUCKET::contains(const bucket& other) const {
    return contains(other.m_testPoint);
}
void BUCKET::insertBoundary(const Segment2Type& line) {
    m_bounds.insert(line);
    if(m_empty) {
        m_limits = AABBox(line.a);
        m_limits.expandTo(line.b);
        m_empty = false;
    } else {
        m_limits.expandTo(line.a);
        m_limits.expandTo(line.b);
    }
    updateInfinity();
}
void BUCKET::insertBoundary(const Loop& loop) {
    insertNoCross(loop);
    Point2Type testPoint = *loop.clockwise();
    for(bucket_list::iterator childIter = m_children.begin();
            childIter != m_children.end(); 
            ++childIter) {
        if(childIter->contains(testPoint)) {
            childIter->insertBoundary(loop);
            m_limits.expandTo(childIter->m_limits);
            updateInfinity();
            return;
        }
    }
    bucket createdBucket(loop);
    std::list<bucket_list::iterator> thingsToMove;
    for(bucket_list::iterator bucketIter = m_children.begin(); 
            bucketIter != m_children.end(); 
            ++bucketIter) {
        if(createdBucket.contains(bucketIter->m_testPoint))
            thingsToMove.push_back(bucketIter);
    }
    while(!thingsToMove.empty()) {
        createdBucket.insertNoCross(thingsToMove.front()->m_loop);
        createdBucket.m_children.splice(
                createdBucket.m_children.end(), 
                m_children, thingsToMove.front());
        thingsToMove.pop_front();
    }
    m_children.push_back(bucket());
    m_children.back().swap(createdBucket);
    m_limits.expandTo(m_children.back().m_limits);
    updateInfinity();
}
BUCKET& BUCKET::select(Point2Type point) {
    for(bucket_list::iterator childIter = m_children.begin();
            childIter != m_children.end(); 
            ++childIter) {
        if(childIter->contains(point))
            return childIter->select(point);
    }
    return *this;
}
void BUCKET::optimize(LabeledOpenPaths& output, Point2Type& entryPoint, 
        const GrueConfig& grueConf) {
    bucket_list::iterator bestRecursiveChoice;
    while((bestRecursiveChoice = pickBestChild(m_children.begin(), 
            m_children.end(), entryPoint)) != m_children.end()) {
        bestRecursiveChoice->optimize(output, entryPoint, grueConf);
        m_children.erase(bestRecursiveChoice);
    }
    node_index currentIndex = -1;
    node::forward_link_iterator next;
    graph_type& currentGraph = m_graph;
    boundary_container& currentBounds = m_noCrossing;
    Point2Type currentUnit;
    while(!currentGraph.empty()) {
        currentIndex = std::min_element(entryBegin(currentGraph), 
                entryEnd(currentGraph), 
                nodeComparator(grueConf, currentGraph, entryPoint))->getIndex();
        LabeledOpenPath activePath;
        if(!currentGraph[currentIndex].forwardEmpty()) {
            smartAppendPoint(currentGraph[currentIndex].data().getPosition(), 
                    currentGraph[currentIndex].data().getLabel(), 
                    output, activePath, entryPoint);
        }
        while((next = bestLink(currentGraph[currentIndex], 
                currentGraph, currentBounds, grueConf, currentUnit)) != 
                currentGraph[currentIndex].forwardEnd()) {
            node::connection nextConnection = *next;
            currentUnit = nextConnection.second->normal();
            PathLabel currentCost(*nextConnection.second);
            smartAppendPoint(nextConnection.first->data().getPosition(), 
                    currentCost, output, activePath, entryPoint);
            currentGraph[currentIndex].disconnect(*nextConnection.first);
            nextConnection.first->disconnect(currentGraph[currentIndex]);
            if(currentGraph[currentIndex].forwardEmpty() && 
                    currentGraph[currentIndex].reverseEmpty()) {
                currentGraph.destroyNode(currentGraph[currentIndex]);
            }
            currentIndex = nextConnection.first->getIndex();
            //std::cout << "Inner Count: " << graph.count() << std::endl;
        }
        if(currentGraph[currentIndex].forwardEmpty() && 
                currentGraph[currentIndex].reverseEmpty()) {
            currentGraph.destroyNode(currentGraph[currentIndex]);
        }
        //recover from corners here
        smartAppendPath(output, activePath);
    }
}
void BUCKET::swap(bucket& other) {
    m_bounds.swap(other.m_bounds);
    m_noCrossing.swap(other.m_noCrossing);
    std::swap(m_limits, other.m_limits);
    m_graph.swap(other.m_graph);
    std::swap(m_testPoint, other.m_testPoint);
    std::swap(m_infinitePoint, other.m_infinitePoint);
    std::swap(m_empty, other.m_empty);
    m_children.swap(other.m_children);
    std::swap(m_loop, other.m_loop);
}
BUCKET::edge_iterator BUCKET::edgeBegin() const { 
    return edge_iterator(m_loop.clockwiseFinite()); 
}
BUCKET::edge_iterator BUCKET::edgeEnd() const { 
    return edge_iterator(m_loop.clockwiseEnd()); 
}
void BUCKET::insertNoCross(const Loop& loop) {
    for(Loop::const_finite_cw_iterator loopIter = loop.clockwiseFinite(); 
            loopIter != loop.clockwiseEnd(); 
            ++loopIter) {
        insertNoCross(loop.segmentAfterPoint(loopIter));
    }
}
void BUCKET::insertNoCross(const Segment2Type& line) {
    m_noCrossing.insert(line);
}
void BUCKET::updateInfinity() {
    m_infinitePoint = m_limits.bottom_left() - Point2Type(20,20);
}

pather_optimizer_fastgraph::bucket_list::iterator
        BUCKET::pickBestChild(
        bucket_list::iterator begin, 
        bucket_list::iterator end, 
        const Point2Type& entryPoint) {
    typedef bucket_list::iterator iterator;
    Scalar bestDistance = std::numeric_limits<Scalar>::max();
    iterator best = begin;
    for(; begin != end; ++begin) {
        for(edge_iterator iter = begin->edgeBegin(); 
                iter != begin->edgeEnd(); 
                ++iter) {
            Scalar distance = (entryPoint - *iter).squaredMagnitude();
            if(distance < bestDistance) {
                bestDistance = distance;
                best = begin;
            }
        }
    }
    return best;
}

#undef BUCKET

}


