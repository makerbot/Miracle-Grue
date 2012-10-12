#include "pather_optimizer_fastgraph.h"
#include "intersection_index.h"
#include <algorithm>
#include <list>
#include <vector>

namespace mgl {

bool pather_optimizer_fastgraph::compareConnections(
        const node::connection& lhs, 
        const node::connection& rhs) {
    if(lhs.second->myValue != rhs.second->myValue)
        return lhs.second->myValue < rhs.second->myValue;
    return lhs.second->distance() > rhs.second->distance();
}
bool pather_optimizer_fastgraph::compareNodes(const node& lhs, 
        const node& rhs) {
    return lhs.data().getPriority() < rhs.data().getPriority();
}
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
bool pather_optimizer_fastgraph::nodeComparator::operator ()(const node& lhs, 
        const node& rhs) const {
    if(lhs.data().getPriority() == rhs.data().getPriority())
        return (lhs.data().getPosition() - m_position).squaredMagnitude() > 
                (rhs.data().getPosition() - m_position).squaredMagnitude();
    return lhs.data().getPriority() < rhs.data().getPriority();
}
bool pather_optimizer_fastgraph::nodeComparator::operator ()(node_index lhs, 
        node_index rhs) const {
    return operator ()(m_graph[lhs], m_graph[rhs]);
}
bool pather_optimizer_fastgraph::probeCompare::operator ()(
        const probe_link_type& lhs, 
        const probe_link_type& rhs) {
    int priorityDifference = m_graph[lhs.first].data().getPriority() - 
            m_graph[rhs.first].data().getPriority();
    Scalar distDifference = lhs.second - rhs.second;
    if(priorityDifference == 0) {
        return distDifference < 0;
    } else {
        return priorityDifference > 0;
    }
}

pather_optimizer_fastgraph::node::forward_link_iterator
        pather_optimizer_fastgraph::bestLink(node& from, 
        graph_type& graph, boundary_container& boundaries) {
    if(from.forwardEmpty()) {
        //return from.forwardEnd();
        buildLinks(from, graph, boundaries);
    }
    return std::max_element(from.forwardBegin(), 
            from.forwardEnd(), compareConnections);
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
            probeCompare(from.getIndex(), graph));
    for(probe_collection::iterator iter = probes.begin(); 
            iter != probes.end(); 
            ++iter) {
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
    LabeledOpenPaths innerIntermediate;
    optimize1(firstPass);
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
    for(multipath_type::iterator iter = firstPass.begin(); 
            iter != firstPass.end(); 
            ++iter) {
        unsigned iteration = 1;
        do {
            innerIntermediate.clear();
            innerIntermediate.swap(*iter);
        } while (optimize2(*iter, innerIntermediate) && 
                ++iteration < grueCfg.get_iterativeEffort());
    }
    for(multipath_type::iterator iter = firstPass.begin(); 
            iter != firstPass.end(); 
            ++iter) {
        labeledpaths.splice(labeledpaths.end(), 
                *iter);
    }
}
void pather_optimizer_fastgraph::optimize1(multipath_type& output) {
//    if(m_graph.empty())
//        return;
    //init code here
    node_index currentIndex = -1;
    node::forward_link_iterator next;
    for(bucket_list::iterator iter = buckets.begin(); 
            iter != buckets.end(); 
            ++iter) {
        output.push_back(LabeledOpenPaths());
        LabeledOpenPaths& labeledpaths = output.back();
        graph_type& currentGraph = iter->m_graph;
        boundary_container& currentBounds = m_boundaries;
        while(!currentGraph.empty()) {
            currentIndex = std::max_element(entryBegin(currentGraph), 
                    entryEnd(currentGraph), 
                    nodeComparator(currentGraph, historyPoint))->getIndex();
            LabeledOpenPath activePath;
            if(!currentGraph[currentIndex].forwardEmpty()) {
                smartAppendPoint(currentGraph[currentIndex].data().getPosition(), 
                        PathLabel(), labeledpaths, activePath);
            }
            while((next = bestLink(currentGraph[currentIndex], 
                    currentGraph, currentBounds)) != 
                    currentGraph[currentIndex].forwardEnd()) {
                node::connection nextConnection = *next;
                PathLabel currentCost(*nextConnection.second);
                smartAppendPoint(nextConnection.first->data().getPosition(), 
                        currentCost, labeledpaths, activePath);
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
        if(foundConnected) {
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
        LabeledOpenPath& path) {
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
    historyPoint = point;
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


