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
bool pather_optimizer_fastgraph::crossesBounds(const libthing::LineSegment2& line) {
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

pather_optimizer_fastgraph::node::forward_link_iterator
        pather_optimizer_fastgraph::bestLink(node& from) {
    if(from.forwardEmpty()) {
        //return from.forwardEnd();
        buildLinks(from);
    }
    return std::max_element(from.forwardBegin(), 
            from.forwardEnd(), compareConnections);
}
void pather_optimizer_fastgraph::buildLinks(node& from) {
    typedef std::vector<probe_link_type> probe_collection;
    probe_collection probes;
    for(entry_iterator iter = entryBegin(); 
            iter != entryEnd(); 
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
        if(iter->second > 10 && false) {
            probe_collection::iterator candidatesEnd = probes.begin();
            ++candidatesEnd;
            for(probe_collection::iterator candidate = probes.begin(); 
                    candidate != candidatesEnd; 
                    ++candidate) {
                PointType unit;
                try {
                    unit = (graph[candidate->first].data().getPosition() - 
                            from.data().getPosition()).unit();
                } catch (const libthing::Exception& le) {}
                from.connect(graph[candidate->first], 
                        Cost(PathLabel(PathLabel::TYP_INVALID, 
                        PathLabel::OWN_INVALID, -1), 
                        candidate->second, 
                        unit));
            }
            break;
        }
        libthing::LineSegment2 probeline(from.data().getPosition(), 
                graph[iter->first].data().getPosition());
        PointType unit;
        try {
            unit = (graph[iter->first].data().getPosition() - 
                    from.data().getPosition()).unit();
        } catch (const libthing::Exception& le) {}
        if(!crossesBounds(probeline)) {
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
    LabeledOpenPaths intermediate;
    LabeledOpenPaths innerIntermediate;
    optimize1(intermediate);
    unsigned iteration = 0;
    do {
        innerIntermediate.clear();
        innerIntermediate.swap(intermediate);
    } while (optimize2(intermediate, innerIntermediate) && 
            ++iteration < grueCfg.get_iterativeEffort());
    optimize2(labeledpaths, intermediate);
}
void pather_optimizer_fastgraph::optimize1(LabeledOpenPaths& labeledpaths) {
    while(!graph.empty()) {
        //init code here
        node_index currentIndex = -1;
        node::forward_link_iterator next;

        //std::cout << "Node Count: " << graph.count() << std::endl;

        currentIndex = std::max_element(graph.begin(), 
                graph.end(), compareNodes)->getIndex();
        LabeledOpenPath activePath;
        if(!graph[currentIndex].forwardEmpty())
            smartAppendPoint(graph[currentIndex].data().getPosition(), 
                    PathLabel(), labeledpaths, activePath);
        while((next = bestLink(graph[currentIndex])) != 
                graph[currentIndex].forwardEnd()) {
            node::connection nextConnection = *next;
            PathLabel currentCost(*nextConnection.second);
            smartAppendPoint(nextConnection.first->data().getPosition(), 
                    currentCost, labeledpaths, activePath);
            graph[currentIndex].disconnect(*nextConnection.first);
            nextConnection.first->disconnect(graph[currentIndex]);
            if(graph[currentIndex].forwardEmpty() && 
                    graph[currentIndex].reverseEmpty())
                graph.destroyNode(graph[currentIndex]);
            currentIndex = nextConnection.first->getIndex();
            //std::cout << "Inner Count: " << graph.count() << std::endl;
        }
        if(graph[currentIndex].forwardEmpty() && 
                graph[currentIndex].reverseEmpty()) {
            graph.destroyNode(graph[currentIndex]);
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
                waste2 += (*labeledopenpaths.back().myPath.fromEnd() - 
                    *(current->front().myPath.fromStart())).magnitude();
                labeledopenpaths.splice(labeledopenpaths.end(), 
                        *current);
                split.erase(current);
                return waste2 <= waste1;
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
            bool connectedCloser = curLength < connectedLength;
            bool connects = !crossesBounds(testJoint);
            if(connects) {
                if(!foundConnected || connectedCloser) {
                    foundConnected = true;
                    joint = testJoint;
                    connected = other;
                    connectedLength = curLength;
                }
            }
            int otherValue = other->front().myLabel.myValue;
            int nearestValue = nearest->front().myLabel.myValue;
            if((otherValue > nearestValue && 
                    curLength - length < 1) || closer) {
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
    return waste2 <= waste1;
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
}

void pather_optimizer_fastgraph::repr_svg(std::ostream& out) {
    out << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"no\"?>" << std::endl;
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">" << std::endl;
    for(graph_type::forward_node_iterator iter = graph.begin(); 
            iter != graph.end(); 
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


