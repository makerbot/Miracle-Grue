#include <list>
#include <vector>

#include "pather_optimizer_fastgraph.h"

namespace mgl {

#define BUCKET pather_optimizer_fastgraph::bucket
#define HIERARCHY BUCKET::LoopHierarchy

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
    Scalar myDistance = std::numeric_limits<Scalar>::max();
    for(edge_iterator edge = edgeBegin(); 
            edge != edgeEnd(); 
            ++edge) {
        Scalar distance = (entryPoint - *edge).squaredMagnitude();
        if(distance < myDistance)
            myDistance = distance;
    }
    bucket_list::iterator bestRecursiveChoice;
    Scalar bestDistance;
    while((bestRecursiveChoice = pickBestChild(m_children.begin(), 
            m_children.end(), entryPoint, bestDistance)) != m_children.end()) {
        if(bestDistance > myDistance)
            break;
        bestRecursiveChoice->optimize(output, entryPoint, grueConf);
        m_children.erase(bestRecursiveChoice);
    }
    node_index currentIndex = -1;
    node::forward_link_iterator next;
    graph_type& currentGraph = m_graph;
    boundary_container& currentBounds = m_noCrossing;
    Point2Type currentUnit;
//    m_hierarchy.repr(std::cerr);
//    std::cout << "That was it for this bucket!" << std::endl;
    m_hierarchy.optimize(output, entryPoint, currentGraph, 
            currentBounds, grueConf);
    while(!currentGraph.empty()) {
        currentIndex = std::min_element(entryBegin(currentGraph), 
                entryEnd(currentGraph), 
                nodeComparator(grueConf, currentGraph, entryPoint))->getIndex();
        LabeledOpenPath activePath;
        if(!currentGraph[currentIndex].forwardEmpty()) {
            //can a connection be made from the last entry to here?
            Segment2Type crossingTest(entryPoint, 
                    currentGraph[currentIndex].data().getPosition());
            if(!crossesBounds(crossingTest, currentBounds)) {
                smartAppendPoint(entryPoint, 
                        PathLabel(PathLabel::TYP_CONNECTION, 
                        PathLabel::OWN_MODEL, 1), output, activePath, 
                        entryPoint);
            }
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
//            std::cout << "Label: " << 
//                    nextConnection.first->data().getLabel().myValue << 
//                    std::endl;
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
    while((bestRecursiveChoice = pickBestChild(m_children.begin(), 
            m_children.end(), entryPoint)) != m_children.end()) {
        bestRecursiveChoice->optimize(output, entryPoint, grueConf);
        m_children.erase(bestRecursiveChoice);
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
    m_hierarchy.swap(other.m_hierarchy);
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
        const Point2Type& entryPoint, 
        Scalar& bestDistanceOutput) {
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
    bestDistanceOutput = bestDistance;
    return best;
}
pather_optimizer_fastgraph::bucket_list::iterator BUCKET::pickBestChild(
        bucket_list::iterator begin, 
        bucket_list::iterator end, 
        const Point2Type& entryPoint) {
    Scalar dummy;
    return pickBestChild(begin, end, entryPoint, dummy);
}

HIERARCHY::LoopHierarchy() : m_testPoint(std::numeric_limits<Scalar>::min(), 
        std::numeric_limits<Scalar>::min()) {}
HIERARCHY::LoopHierarchy(const LabeledLoop& loop) 
        : m_label(loop.myLabel) {
    m_testPoint = *loop.myPath.clockwise();
    m_limits = AABBox(m_testPoint);
    insertBoundary(loop.myPath);
}
HIERARCHY::LoopHierarchy(const Loop& loop, const PathLabel& label) 
        : m_label(label) {
    m_testPoint = *loop.clockwise();
    m_limits = AABBox(m_testPoint);
    m_loop = loop;
    insertBoundary(loop);
}
HIERARCHY& HIERARCHY::insert(const LabeledLoop& loop) {
    return insert(loop.myPath, loop.myLabel);
}
HIERARCHY& HIERARCHY::insert(const Loop& loop, const PathLabel& label) {
    LoopHierarchy temporary(loop, label);
    return insert(temporary);
}
HIERARCHY& HIERARCHY::insert(LoopHierarchy& constructed) {
//    if(m_label.isInvalid()) {
//        constructed.swap(*this);
//        return *this;
//    }
    if(isValid() && constructed.contains(*this)) {
//        std::cout << "Parent-Child inversion!" << std::endl;
        constructed.swap(*this);    //this is actually perfectly safe!
        m_children.push_back(LoopHierarchy());
        m_children.back().swap(constructed);
        return m_children.back();
    }
    for(hierarchy_list::iterator childIter = m_children.begin(); 
            childIter != m_children.end(); 
            ++childIter) {
        if(childIter->contains(constructed))
            return childIter->insert(constructed);
    }
    std::list<hierarchy_list::iterator> thingsToMove;
    for(hierarchy_list::iterator childIter = m_children.begin(); 
            childIter != m_children.end(); 
            ++childIter) {
        if(constructed.contains(*childIter))
            thingsToMove.push_back(childIter);
    }
    if(!thingsToMove.empty()) {
//        std::cout << "Donating " << thingsToMove.size() << " children from " 
//                << m_label.myValue << " to "
//                << constructed.m_label.myValue << std::endl;
        while(!thingsToMove.empty()) {
            constructed.insert(*thingsToMove.front());
            m_children.erase(thingsToMove.front());
            thingsToMove.pop_front();
        }
    }
//    std::cout << "Inserting " << constructed.m_label.myValue 
//            << " into " << m_label.myValue << std::endl;
    m_children.push_back(LoopHierarchy());
    m_children.back().swap(constructed);
    return m_children.back();
}
bool HIERARCHY::contains(Point2Type point) const {
    if(!isValid())
        return true;
    Segment2Type testLine(m_infinitePoint, point);
    bool result = (countIntersections(testLine, m_bounds) & 1) != 0;  //even-odd test
    //bool result2 = m_loop.windingContains(point);
    //std::cout << "NewResult: " << result2 << "\tOldResult: " << result << std::endl;
    return result;
}
bool HIERARCHY::contains(const LoopHierarchy& other) const {
    bool myResult = contains(other.m_testPoint);
    bool otherResult = other.contains(m_testPoint);
    return myResult && !otherResult;
}
void HIERARCHY::optimize(LabeledOpenPaths& output, Point2Type& entryPoint, 
        graph_type& graph, boundary_container& bounds, 
        const GrueConfig& grueConf) {
    if(m_loop.empty()) {
        LoopHierarchyStrictComparator compare(entryPoint, graph, grueConf);
        hierarchy_list::iterator bestChoice;
        while((bestChoice = bestChild(compare)) != m_children.end()) {
            bestChoice->optimize(output, entryPoint, graph, bounds, grueConf);
            m_children.erase(bestChoice);
        }
        return;
    }
    graph_type::node_index dummy = 0;
    optimize(output, entryPoint, graph, dummy, bounds, grueConf);
}
void HIERARCHY::optimize(LabeledOpenPaths& output, 
        Point2Type& entryPoint, 
        graph_type& graph, graph_type::node_index& from, 
        boundary_container& bounds, 
        const GrueConfig& grueConf) {
    LoopHierarchyStrictComparator typeDistComparator(entryPoint, graph, grueConf);
    LoopHierarchyBaseComparator typeComparator(entryPoint, graph, grueConf);
    hierarchy_list::iterator bestChoice;
    while((bestChoice = bestChild(typeDistComparator)) 
           != m_children.end()) {
//            std::cout << "Head recursion into priority " << 
//                    bestChoice->m_label.myValue << std::endl;
       if(typeComparator(*this, *bestChoice)) {
//           std::cout << bestChoice->m_label.myValue << " Not better than " << 
//                   m_label.myValue << std::endl;
           break;
       } else {
//           std::cout << bestChoice->m_label.myValue << " Good to recurse " << 
//                   m_label.myValue << std::endl;
       }
       bestChoice->optimize(output, entryPoint, graph, from, bounds, grueConf);
       m_children.erase(bestChoice);
    }
//    std::cout << "Optimizing priority " << m_label.myValue << 
//            " count " << m_loop.size() << std::endl;
    if(!m_loop.empty()) {
        Scalar minDistance = std::numeric_limits<Scalar>::max();
        Loop::cw_iterator minStart = m_loop.clockwise();
        for(Loop::finite_cw_iterator iter = m_loop.clockwiseFinite(); 
                iter != m_loop.clockwiseEnd(); 
                ++iter) {
            Scalar distance = (entryPoint - *iter).squaredMagnitude();
            if(distance < minDistance) {
                minDistance = distance;
                minStart = Loop::cw_iterator(iter);
            }
        }
        Segment2Type connectLine(entryPoint, *minStart);
        if(!crossesBounds(connectLine, bounds)) {
            LabeledOpenPath connection(PathLabel(PathLabel::TYP_CONNECTION, 
                    PathLabel::OWN_MODEL, 1));
            connection.myPath.appendPoint(connectLine.a);
            connection.myPath.appendPoint(connectLine.b);
            output.push_back(connection);
        }
        entryPoint = connectLine.b;
        LabeledOpenPath thisLoop(m_label);
        LoopPath lp(m_loop, minStart, Loop::ccw_iterator(minStart));
        for(LoopPath::iterator iter = lp.fromStart(); 
                iter != lp.end(); 
                ++iter) {
            thisLoop.myPath.appendPoint(*iter);
        }
        output.push_back(thisLoop);
    }
    
    if(bestChoice != m_children.end()) {
        do {
//            std::cout << "Tail recursion into priority " << 
//                    bestChoice->m_label.myValue << std::endl;
            bestChoice->optimize(output, entryPoint, graph, from, bounds, grueConf);
            m_children.erase(bestChoice);
        } while((bestChoice = bestChild(typeDistComparator)) 
                != m_children.end());
    }
//    std::cout << "Optimizing priority " << m_label.myValue 
//            << " done " << m_loop.size() << std::endl;
}
void HIERARCHY::swap(LoopHierarchy& other) {
    std::swap(m_label, other.m_label);
    m_bounds.swap(other.m_bounds);
    m_children.swap(other.m_children);
    std::swap(m_limits, other.m_limits);
    std::swap(m_testPoint, other.m_testPoint);
    std::swap(m_infinitePoint, other.m_infinitePoint);
    std::swap(m_loop, other.m_loop);
}
void HIERARCHY::repr(std::ostream& out, size_t level) {
    if(!level)
        out << std::endl;
    std::string indent(level + 1, '|');
    out << indent << "L-" << m_loop.size() << "-(" << m_children.size() 
            << ")-" << m_label.myValue << std::endl;
    if(m_children.empty()) {
//        out << indent << std::endl;
    } else {
        for(hierarchy_list::iterator iter = m_children.begin(); 
                iter != m_children.end(); 
                ++iter) {
            iter->repr(out, level + 1);
        }
    }
}
bool HIERARCHY::isValid() const {
    return !m_loop.empty();
}
BUCKET::hierarchy_list::iterator HIERARCHY::bestChild(const 
        LoopHierarchyBaseComparator& compare) {
    return std::min_element(m_children.begin(), m_children.end(), 
            compare);
}
void HIERARCHY::insertBoundary(const Loop& loop) {
    for(Loop::const_finite_cw_iterator iter = loop.clockwiseFinite(); 
            iter != loop.clockwiseEnd(); 
            ++iter) {
        insertBoundary(loop.segmentAfterPoint(iter));
    }
    updateInfinity();
}
void HIERARCHY::insertBoundary(const Segment2Type& line) {
    m_bounds.insert(line);
    m_limits.expandTo(line.a);
    m_limits.expandTo(line.b);
}
void HIERARCHY::updateInfinity() {
    m_infinitePoint = m_limits.bottom_left() - Point2Type(20, 20);
}


#undef HIERARCHY
#undef BUCKET

}


