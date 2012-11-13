#include "pather_optimizer_fastgraph.h"
#include "intersection_index.h"
#include "pather.h"
#include <algorithm>
#include <list>
#include <vector>

namespace mgl {

bool pather_optimizer_fastgraph::crossesBounds(
        const Segment2Type& line, 
        boundary_container& boundaries) {
    std::vector<Segment2Type> filtered;
    boundaries.search(filtered, LineSegmentFilter(line));
    for(std::vector<Segment2Type>::const_iterator iter = 
            filtered.begin(); 
            iter != filtered.end(); 
            ++iter) {
        if(iter->intersects(line))
            return true;
    }
    return false;
}
pather_optimizer_fastgraph::node::forward_link_iterator
        pather_optimizer_fastgraph::bestLink(node& from, 
        graph_type& graph, boundary_container& boundaries, 
        const GrueConfig& grueConf, 
        Point2Type unit) {
    if(from.forwardEmpty()) {
        //return from.forwardEnd();
        buildLinks(from, graph, boundaries, grueConf);
    }
    return std::min_element(from.forwardBegin(), 
            from.forwardEnd(), NodeConnectionComparator(grueConf, unit));
}
pather_optimizer_fastgraph::node::forward_link_iterator
        pather_optimizer_fastgraph::bestLink(node& from, 
        graph_type& graph, boundary_container& boundaries, 
        bucket::LoopHierarchy::entryIndexVector& entries, 
        const GrueConfig& grueConf, 
        Point2Type unit) {
    if(from.forwardEmpty()) {
        //return from.forwardEnd();
        buildLinks(from, graph, boundaries, entries, grueConf);
    }
    return std::min_element(from.forwardBegin(), 
            from.forwardEnd(), NodeConnectionComparator(grueConf, unit));
}
void pather_optimizer_fastgraph::buildLinks(node& from, graph_type& graph, 
        boundary_container& boundaries, const GrueConfig& grueConf) {
    std::vector<probe_link_type> probes;
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
            probeCompare(from.getIndex(), graph, grueConf));
    buildLinks(from, graph, boundaries, probes, grueConf);
}
void pather_optimizer_fastgraph::buildLinks(node& from, graph_type& graph, 
        boundary_container& boundaries, 
        bucket::LoopHierarchy::entryIndexVector& entries, 
        const GrueConfig& grueConf) {
    typedef bucket::LoopHierarchy::entryIndexVector::iterator iterator;
    std::vector<probe_link_type> probes;
    for(iterator iter = entries.begin(); 
            iter != entries.end(); 
            ++iter) {
        if(*iter == from.getIndex())
            continue;
        probes.push_back(probe_link_type(*iter, 
                (from.data().getPosition() - 
                graph[*iter].data().getPosition()).magnitude()));
    }
    std::sort(probes.begin(), probes.end(), 
            probeCompare(from.getIndex(), graph, grueConf));
    buildLinks(from, graph, boundaries, probes, grueConf);
}
void pather_optimizer_fastgraph::buildLinks(node& from, graph_type& graph, 
        boundary_container& boundaries, 
        std::vector<probe_link_type>& sortedProbes, 
        const GrueConfig& grueConf) {
    LinkBuildingConnectionCutoffComparator connectCompare(grueConf);
    for(std::vector<probe_link_type>::iterator iter = sortedProbes.begin(); 
            iter != sortedProbes.end(); 
            ++iter) {
        if(connectCompare(graph[sortedProbes.front().first].data().getLabel(), 
                graph[iter->first].data().getLabel()))
            break;  //make no connections to things of lower priority
        Segment2Type probeline(from.data().getPosition(), 
                graph[iter->first].data().getPosition());
        Point2Type unit;
        try {
            unit = (graph[iter->first].data().getPosition() - 
                    from.data().getPosition()).unit();
        } catch (const GeometryException& le) {}
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
    optimizeBuckets(firstPass, historyPoint);
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
void pather_optimizer_fastgraph::optimizeBuckets(multipath_type& output, 
        Point2Type& entryPoint) {
    while(!buckets.empty()) {
        bucket_list::iterator currentNearest = buckets.begin();
        currentNearest = bucket::pickBestChild(buckets.begin(), 
                buckets.end(), entryPoint);
        LabeledOpenPaths currentResult;
        //optimize1Inner(currentResult, currentNearest, entryPoint);
        currentNearest->optimize(currentResult, entryPoint, grueCfg);
        
        output.push_back(LabeledOpenPaths());
        
        output.back().splice(output.back().end(), currentResult);
        buckets.erase(currentNearest);
    }
}
bool pather_optimizer_fastgraph::optimizeIterative(LabeledOpenPaths&, // labeledopenpaths, 
        LabeledOpenPaths& // intermediate
        ) {
    return false;
//    multipath_type split;
//    Scalar waste1, waste2;
//    waste1 = splitPaths(split, intermediate);
//    waste2 = 0;
//    //split.sort(comparePathLists);
////    std::cout << "Size of split: " << split.size() << std::endl;;
//    if(split.empty())
//        return false;
//    multipath_type::iterator current = split.begin();
//    multipath_type::iterator other;
//    multipath_type::iterator nearest;
//    multipath_type::iterator connected;
//    while(!split.empty()) {
//        bool foundConnected = false;
//        Segment2Type joint;
//        Scalar length = std::numeric_limits<Scalar>::max();
//        Scalar connectedLength = std::numeric_limits<Scalar>::max();
//        other = current;
//        ++other;
//        if(other == split.end()) {
//            if(current == split.begin()) {
//                //current is last
//                if(!labeledopenpaths.empty()) {
//                    waste2 += (*labeledopenpaths.back().myPath.fromEnd() - 
//                        *(current->front().myPath.fromStart())).magnitude();
//                }
//                labeledopenpaths.splice(labeledopenpaths.end(), 
//                        *current);
//                split.erase(current);
//                return waste2 < waste1;
//            } else {
//                other = split.begin();
//            }
//        }
//        nearest = other;
//        LabeledOpenPaths& currentList = *current;
//        for(; other != split.end(); ++other) {
//            if(other == current)
//                continue;
//            LabeledOpenPaths& otherList = *other;
//            LabeledOpenPath& currentBack = currentList.back();
//            LabeledOpenPath& otherFront = otherList.front();
//            Point2Type currentPoint = *(currentBack.myPath.fromEnd());
//            Point2Type otherPoint = *(otherFront.myPath.fromStart());
//            Segment2Type testJoint(currentPoint, 
//                    otherPoint);
//            Scalar curLength = testJoint.length();
//            bool closer = curLength < length;
//            bool connects = !crossesBounds(testJoint, m_boundaries);
//            int otherValue = other->front().myLabel.myValue;
//            int nearestValue = nearest->front().myLabel.myValue;
//            if(connects) {
//                if(!foundConnected || (otherValue > nearestValue)) {
//                    foundConnected = true;
//                    joint = testJoint;
//                    connected = other;
//                    connectedLength = curLength;
//                }
//            }
//            if((otherValue > nearestValue) || closer) {
//                nearest = other;
//                length = testJoint.length();
//            }
//        }
//        //current goes to output
//        LabeledOpenPaths& currentRef = *current;
//        labeledopenpaths.splice(labeledopenpaths.end(), 
//                currentRef);
//        if(foundConnected && connected->front().myLabel.myValue >= 
//                nearest->front().myLabel.myValue) {
//            LabeledOpenPath connectingPath(PathLabel(PathLabel::TYP_CONNECTION, 
//                    PathLabel::OWN_MODEL));
//            connectingPath.myPath.appendPoint(joint.a);
//            connectingPath.myPath.appendPoint(joint.b);
//            //connection goes to output
//            labeledopenpaths.push_back(connectingPath);
//            //erase current
//            split.erase(current);
//            //search from the one you picked
//            current = connected;
////            std::cout << "Connection made!" << std::endl;
//        } else {
//            //erase current
//            split.erase(current);
//            //search from the nearest one
//            current = nearest;
////            std::cout << "No connection!" << std::endl;
//            waste2 += (*labeledopenpaths.back().myPath.fromEnd() - 
//                    *(current->front().myPath.fromStart())).magnitude();
//        }
//    }
//    return waste2 < waste1;
}
void pather_optimizer_fastgraph::smartAppendPath(LabeledOpenPaths& labeledpaths, 
        LabeledOpenPath& path) {
    if(path.myLabel.isValid() && path.myPath.size() > 1) 
        labeledpaths.push_back(path);
    path.myLabel = PathLabel();
    path.myPath.clear();
}

void pather_optimizer_fastgraph::smartAppendPoint(Point2Type point, 
        PathLabel label, LabeledOpenPaths& labeledpaths, 
        LabeledOpenPath& path, Point2Type& entryPoint) {
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

void pather_optimizer_fastgraph::smartTryConnection(
        LabeledOpenPaths& labeledpaths, Point2Type point, 
        Point2Type& entryPoint, boundary_container& boundaries, 
        const GrueConfig&) {
//    if(!labeledpaths.empty() && labeledpaths.back().myLabel.myValue == 
//            LayerPaths::Layer::ExtruderLayer::INSET_LABEL_VALUE) {
//        //no connection can be made FROM an outline
//    } else {
        Segment2Type testSeg(entryPoint, point);
        if(!crossesBounds(testSeg, boundaries)) {
            LabeledOpenPath connection(PathLabel(PathLabel::TYP_CONNECTION, 
                    PathLabel::OWN_MODEL, 1));
            connection.myPath.appendPoint(entryPoint);
            connection.myPath.appendPoint(point);
            labeledpaths.push_back(connection);
        }
//    }
    entryPoint = point;
}

void pather_optimizer_fastgraph::repr_svg(std::ostream& out) {
    out << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"no\"?>" << std::endl;
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">" << std::endl;
//    for(graph_type::forward_node_iterator iter = m_graph.begin(); 
//            iter != m_graph.end(); 
//            ++iter) {
//        out << "<circle cx=\"" << 200 + iter->data().getPosition().x * 10
//                << "\" cy=\"" <<  200 + iter->data().getPosition().y * 10
//                << "\" r=\"2.0\" style=\"stroke:red; fill:red;\"/>" << std::endl;
//        for(node::forward_link_iterator linkiter = iter->forwardBegin(); 
//                linkiter != iter->forwardEnd(); 
//                ++linkiter) {
//            out << "<line x1=\"" << 200 + iter->data().getPosition().x * 10
//                    << "\" y1=\"" << 200 + iter->data().getPosition().y * 10
//                    << "\" x2=\"" << 200 + (*linkiter).first->data().getPosition().x * 10
//                    << "\" y2=\"" << 200 + (*linkiter).first->data().getPosition().y * 10
//                    << "\" style=\"stroke:black; stroke-width:1.0;\"/>" << std::endl;
//        }
//    }
    out << "</svg>" << std::endl;
}

}


