#include "pather_optimizer_fastgraph.h"
#include "intersection_index.h"
#include "pather.h"
#include <algorithm>
#include <list>
#include <vector>

namespace mgl {

bool pather_optimizer_fastgraph::comparePathLists(const LabeledOpenPaths& lhs, 
        const LabeledOpenPaths& rhs) {
    return lhs.front().myLabel.myValue > rhs.front().myLabel.myValue;
}
bool pather_optimizer_fastgraph::crossesBounds(
        const libthing::LineSegment2& line, 
        boundary_container& boundaries) {
    std::vector<libthing::LineSegment2> filtered;
    boundaries.search(filtered, LineSegmentFilter(line));
    for(std::vector<libthing::LineSegment2>::const_iterator iter = 
            filtered.begin(); 
            iter != filtered.end(); 
            ++iter) {
        if(iter->intersects(line))
            return true;
    }
    return false;
}
int pather_optimizer_fastgraph::LabelTypeComparator::compare(
        const value_type& lhs, 
        const value_type& rhs) const {
    static const int OUTLINE_VALUE = 
            LayerPaths::Layer::ExtruderLayer::OUTLINE_LABEL_VALUE;
    const int INNERMOST_VALUE = 
            LayerPaths::Layer::ExtruderLayer::INSET_LABEL_VALUE + 
            grueCfg.get_nbOfShells() - 1;
    if(lhs.myValue == rhs.myValue)
            return SAME;
    //fail outermost shell always
    if(lhs.myValue == OUTLINE_VALUE && rhs.myValue != OUTLINE_VALUE)
        return WORSE;
    if(rhs.myValue == OUTLINE_VALUE && lhs.myValue != OUTLINE_VALUE)
        return BETTER;
    //pass innermost shell always
    if(lhs.myValue == INNERMOST_VALUE && rhs.myValue != INNERMOST_VALUE)
        return BETTER;
    if(rhs.myValue == INNERMOST_VALUE && lhs.myValue != INNERMOST_VALUE)
        return WORSE;
    return SAME;
}
int pather_optimizer_fastgraph::LabelPriorityComparator::compare(
        const value_type& lhs, 
        const value_type& rhs) const {
    int diff = lhs.myValue - rhs.myValue;
    return (diff < 0 ? WORSE : 
        (diff > 0 ? BETTER : 
            SAME));
}
int pather_optimizer_fastgraph::NodeComparator::compare(const value_type& lhs, 
        const value_type& rhs) const {
    return m_labelCompare.compare(lhs.data().getLabel(), 
            rhs.data().getLabel());
}
int pather_optimizer_fastgraph::NodeConnectionComparator::compare(
        const value_type& lhs, const value_type& rhs) const {
    int nc = m_nodeCompare(*lhs.first, *rhs.first);
    if(nc)
        return nc;
    Scalar lunit = m_unit.dotProduct(lhs.second->normal());
    Scalar runit = m_unit.dotProduct(rhs.second->normal());
    return (lunit > runit ? 
        BETTER : (runit < lunit ? 
            WORSE : SAME));
}
bool pather_optimizer_fastgraph::connectionComparator::operator ()(
        const node::connection& lhs, const node::connection& rhs) const {
    if(lhs.second->myValue != rhs.second->myValue)
        return lhs.second->myValue < rhs.second->myValue;
    if(lhs.second->isValid() && rhs.second->isValid()) {
        return m_unit.dotProduct(lhs.second->normal()) < 
                m_unit.dotProduct(rhs.second->normal());
    }
    return lhs.second->distance() > rhs.second->distance();
}
bool pather_optimizer_fastgraph::nodeComparator::operator ()(const node& lhs, 
        const node& rhs) const {
    int nc = m_nodeCompare.compare(lhs, rhs);
    if(nc)
        return nc == BETTER;
    return (lhs.data().getPosition() - m_position).squaredMagnitude() < 
            (rhs.data().getPosition() - m_position).squaredMagnitude();
}
bool pather_optimizer_fastgraph::nodeComparator::operator ()(node_index lhs, 
        node_index rhs) const {
    return operator ()(m_graph[lhs], m_graph[rhs]);
}
bool pather_optimizer_fastgraph::probeCompare::operator ()(
        const probe_link_type& lhs, 
        const probe_link_type& rhs) {
    int nc = m_nodeCompare.compare(m_graph[lhs.first], m_graph[rhs.first]);
    if(nc)
        return nc == BETTER;
    Scalar distDifference = lhs.second - rhs.second;
    return distDifference < 0;
}
bool pather_optimizer_fastgraph::bucketSorter::operator ()(const bucket& lhs, 
        const bucket& rhs) const {
    libthing::LineSegment2 lhsLine = libthing::LineSegment2(m_from, 
            lhs.m_bounds.begin()->a);
    libthing::LineSegment2 rhsLine = libthing::LineSegment2(m_from, 
            rhs.m_bounds.begin()->a);
    size_t lhsCount = countIntersections(lhsLine, m_bounds);
    size_t rhsCount = countIntersections(rhsLine, m_bounds);
    return lhsCount > rhsCount;
}

pather_optimizer_fastgraph::node::forward_link_iterator
        pather_optimizer_fastgraph::bestLink(node& from, 
        graph_type& graph, boundary_container& boundaries, 
        PointType unit) {
    if(from.forwardEmpty()) {
        //return from.forwardEnd();
        buildLinks(from, graph, boundaries);
    }
    return std::min_element(from.forwardBegin(), 
            from.forwardEnd(), NodeConnectionComparator(grueCfg, unit));
}
void pather_optimizer_fastgraph::buildLinks(node& from, graph_type& graph, 
        boundary_container& boundaries) {
    typedef std::vector<probe_link_type> probe_collection;
    probe_collection probes;
    for(entry_iterator iter = entryBegin(graph); 
            iter != entryEnd(graph); 
            ++iter) {
        if(*iter == from)
            continue;
        probes.push_back(probe_link_type(iter->getIndex(), 
                (from.data().getPosition() - 
                iter->data().getPosition()).magnitude()));
    }
    std::sort(probes.begin(), probes.end(), 
            probeCompare(from.getIndex(), graph, grueCfg));
    LabelTypeComparator typeCompare(grueCfg);
    for(probe_collection::iterator iter = probes.begin(); 
            iter != probes.end(); 
            ++iter) {
        if(typeCompare(graph[probes.front().first].data().getLabel(), 
                graph[iter->first].data().getLabel()))
            break;  //make no connections to things of lower priority
        libthing::LineSegment2 probeline(from.data().getPosition(), 
                graph[iter->first].data().getPosition());
        PointType unit;
        try {
            unit = (graph[iter->first].data().getPosition() - 
                    from.data().getPosition()).unit();
        } catch (const libthing::Exception& le) {}
        if(!crossesBounds(probeline, boundaries)) {
            from.connect(graph[iter->first], 
                    Cost(PathLabel(PathLabel::TYP_CONNECTION, 
                    PathLabel::OWN_MODEL, -1), 
                    iter->second, 
                    unit
                    ));
            break;
        }
    }
}
void pather_optimizer_fastgraph::optimizeInternal(LabeledOpenPaths& labeledpaths) {
    multipath_type firstPass;
    //LabeledOpenPaths innerIntermediate;
    optimize1(firstPass, historyPoint);
    for(multipath_type::iterator iter = firstPass.begin(); 
            iter != firstPass.end(); 
            ++iter) {
        std::vector<LabeledOpenPaths::iterator> toClear;
        for(LabeledOpenPaths::iterator iter2 = iter->begin(); 
                iter2 != iter->end(); 
                ++iter2) {
            if(iter2->myPath.size() < 2)
                toClear.push_back(iter2);
        }
        while(!toClear.empty()) {
            iter->erase(toClear.back());
            toClear.pop_back();
        }
    }
//    for(multipath_type::iterator iter = firstPass.begin(); 
//            iter != firstPass.end(); 
//            ++iter) {
//        unsigned iteration = 1;
//        do {
//            innerIntermediate.clear();
//            innerIntermediate.swap(*iter);
//        } while (optimize2(*iter, innerIntermediate) && 
//                ++iteration < grueCfg.get_iterativeEffort());
//    }
    for(multipath_type::iterator iter = firstPass.begin(); 
            iter != firstPass.end(); 
            ++iter) {
        labeledpaths.splice(labeledpaths.end(), 
                *iter);
    }
}
void pather_optimizer_fastgraph::optimize1(multipath_type& output, 
        PointType& entryPoint) {
    while(!buckets.empty()) {
        Scalar distanceToEntry = std::numeric_limits<Scalar>::max();
        bucket_list::iterator currentNearest = buckets.begin();
        for(bucket_list::iterator iter = buckets.begin(); 
                iter != buckets.end(); 
                ++iter) {
            for(entry_iterator entryIter = entryBegin(iter->m_graph); 
                    entryIter != entryEnd(iter->m_graph); 
                    ++entryIter) {
                Scalar entryDistance = (entryIter->data().getPosition() - 
                        entryPoint).squaredMagnitude();
                if(entryDistance < distanceToEntry) {
                    distanceToEntry = entryDistance;
                    currentNearest = iter;
                }
            }
        }
        LabeledOpenPaths currentResult;
        optimize1Inner(currentResult, currentNearest, entryPoint);
        
        output.push_back(LabeledOpenPaths());
        
        output.back().splice(output.back().end(), currentResult);
        buckets.erase(currentNearest);
    }
}
void pather_optimizer_fastgraph::optimize1Inner(LabeledOpenPaths& labeledpaths, 
        bucket_list::iterator input, PointType& entryPoint) {
    node_index currentIndex = -1;
    node::forward_link_iterator next;
    graph_type& currentGraph = input->m_graph;
    boundary_container& currentBounds = m_boundaries;
    PointType currentUnit;
    while(!currentGraph.empty()) {
        currentIndex = std::min_element(entryBegin(currentGraph), 
                entryEnd(currentGraph), 
                nodeComparator(grueCfg, currentGraph, entryPoint))->getIndex();
        LabeledOpenPath activePath;
        if(!currentGraph[currentIndex].forwardEmpty()) {
            smartAppendPoint(currentGraph[currentIndex].data().getPosition(), 
                    PathLabel(), labeledpaths, activePath, entryPoint);
        }
        while((next = bestLink(currentGraph[currentIndex], 
                currentGraph, currentBounds, currentUnit)) != 
                currentGraph[currentIndex].forwardEnd()) {
            node::connection nextConnection = *next;
            currentUnit = nextConnection.second->normal();
            PathLabel currentCost(*nextConnection.second);
            smartAppendPoint(nextConnection.first->data().getPosition(), 
                    currentCost, labeledpaths, activePath, entryPoint);
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
        smartAppendPath(labeledpaths, activePath);
    }
}
bool pather_optimizer_fastgraph::optimize2(LabeledOpenPaths& labeledopenpaths, 
        LabeledOpenPaths& intermediate) {
    multipath_type split;
    Scalar waste1, waste2;
    waste1 = splitPaths(split, intermediate);
    waste2 = 0;
    //split.sort(comparePathLists);
//    std::cout << "Size of split: " << split.size() << std::endl;;
    if(split.empty())
        return false;
    multipath_type::iterator current = split.begin();
    multipath_type::iterator other;
    multipath_type::iterator nearest;
    multipath_type::iterator connected;
    while(!split.empty()) {
        bool foundConnected = false;
        libthing::LineSegment2 joint;
        Scalar length = std::numeric_limits<Scalar>::max();
        Scalar connectedLength = std::numeric_limits<Scalar>::max();
        other = current;
        ++other;
        if(other == split.end()) {
            if(current == split.begin()) {
                //current is last
                if(!labeledopenpaths.empty()) {
                    waste2 += (*labeledopenpaths.back().myPath.fromEnd() - 
                        *(current->front().myPath.fromStart())).magnitude();
                }
                labeledopenpaths.splice(labeledopenpaths.end(), 
                        *current);
                split.erase(current);
                return waste2 < waste1;
            } else {
                other = split.begin();
            }
        }
        nearest = other;
        LabeledOpenPaths& currentList = *current;
        for(; other != split.end(); ++other) {
            if(other == current)
                continue;
            LabeledOpenPaths& otherList = *other;
            LabeledOpenPath& currentBack = currentList.back();
            LabeledOpenPath& otherFront = otherList.front();
            PointType currentPoint = *(currentBack.myPath.fromEnd());
            PointType otherPoint = *(otherFront.myPath.fromStart());
            libthing::LineSegment2 testJoint(currentPoint, 
                    otherPoint);
            Scalar curLength = testJoint.length();
            bool closer = curLength < length;
            bool connects = !crossesBounds(testJoint, m_boundaries);
            int otherValue = other->front().myLabel.myValue;
            int nearestValue = nearest->front().myLabel.myValue;
            if(connects) {
                if(!foundConnected || (otherValue > nearestValue)) {
                    foundConnected = true;
                    joint = testJoint;
                    connected = other;
                    connectedLength = curLength;
                }
            }
            if((otherValue > nearestValue) || closer) {
                nearest = other;
                length = testJoint.length();
            }
        }
        //current goes to output
        LabeledOpenPaths& currentRef = *current;
        labeledopenpaths.splice(labeledopenpaths.end(), 
                currentRef);
        if(foundConnected && connected->front().myLabel.myValue >= 
                nearest->front().myLabel.myValue) {
            LabeledOpenPath connectingPath(PathLabel(PathLabel::TYP_CONNECTION, 
                    PathLabel::OWN_MODEL));
            connectingPath.myPath.appendPoint(joint.a);
            connectingPath.myPath.appendPoint(joint.b);
            //connection goes to output
            labeledopenpaths.push_back(connectingPath);
            //erase current
            split.erase(current);
            //search from the one you picked
            current = connected;
//            std::cout << "Connection made!" << std::endl;
        } else {
            //erase current
            split.erase(current);
            //search from the nearest one
            current = nearest;
//            std::cout << "No connection!" << std::endl;
            waste2 += (*labeledopenpaths.back().myPath.fromEnd() - 
                    *(current->front().myPath.fromStart())).magnitude();
        }
    }
    return waste2 < waste1;
}
void pather_optimizer_fastgraph::smartAppendPath(LabeledOpenPaths& labeledpaths, 
        LabeledOpenPath& path) {
    if(path.myLabel.isValid() && path.myPath.size() > 1) 
        labeledpaths.push_back(path);
    path.myLabel = PathLabel();
    path.myPath.clear();
}

void pather_optimizer_fastgraph::smartAppendPoint(PointType point, 
        PathLabel label, LabeledOpenPaths& labeledpaths, 
        LabeledOpenPath& path, PointType& entryPoint) {
    if(path.myLabel.isInvalid()) {
        path.myLabel = label;
        //path.myPath.clear();
        path.myPath.appendPoint(point);
    } else if(path.myLabel == label) {
        path.myPath.appendPoint(point);
    } else {
        path.myPath.appendPoint(point);
        smartAppendPath(labeledpaths, path);
        path.myLabel = label;
        path.myPath.appendPoint(point);
    }
    entryPoint = point;
}

void pather_optimizer_fastgraph::repr_svg(std::ostream& out) {
    out << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"no\"?>" << std::endl;
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">" << std::endl;
    for(graph_type::forward_node_iterator iter = m_graph.begin(); 
            iter != m_graph.end(); 
            ++iter) {
        out << "<circle cx=\"" << 200 + iter->data().getPosition().x * 10
                << "\" cy=\"" <<  200 + iter->data().getPosition().y * 10
                << "\" r=\"2.0\" style=\"stroke:red; fill:red;\"/>" << std::endl;
        for(node::forward_link_iterator linkiter = iter->forwardBegin(); 
                linkiter != iter->forwardEnd(); 
                ++linkiter) {
            out << "<line x1=\"" << 200 + iter->data().getPosition().x * 10
                    << "\" y1=\"" << 200 + iter->data().getPosition().y * 10
                    << "\" x2=\"" << 200 + (*linkiter).first->data().getPosition().x * 10
                    << "\" y2=\"" << 200 + (*linkiter).first->data().getPosition().y * 10
                    << "\" style=\"stroke:black; stroke-width:1.0;\"/>" << std::endl;
        }
    }
    out << "</svg>" << std::endl;
}

}


