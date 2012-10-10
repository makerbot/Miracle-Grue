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
    if(from.forwardEmpty())
        return from.forwardEnd();
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
        if(iter->second > 10) {
            probe_collection::iterator candidatesEnd = iter;
            ++candidatesEnd;
            for(probe_collection::iterator candidate = probes.begin(); 
                    candidate != candidatesEnd; 
                    ++candidate) {
                from.connect(graph[candidate->first], 
                        Cost(PathLabel(PathLabel::TYP_INVALID, 
                        PathLabel::OWN_INVALID, -1), 
                        candidate->second, 
                        (graph[candidate->first].data().getPosition() - 
                        from.data().getPosition()).unit()
                        ));
            }
            break;
        }
        libthing::LineSegment2 probeline(from.data().getPosition(), 
                graph[iter->first].data().getPosition());
        if(!crossesBounds(probeline)) {
            from.connect(graph[iter->first], 
                    Cost(PathLabel(PathLabel::TYP_CONNECTION, 
                    PathLabel::OWN_MODEL, -1), 
                    iter->second, 
                    (graph[iter->first].data().getPosition() - 
                    from.data().getPosition()).unit()
                    ));
            break;
        }
    }
}
void pather_optimizer_fastgraph::optimizeInternal(LabeledOpenPaths& labeledpaths) {
    while(!graph.empty()) {
        //init code here
        node_index currentIndex = -1;
        node::forward_link_iterator next;

        //std::cout << "Node Count: " << graph.count() << std::endl;

        currentIndex = std::max_element(graph.begin(), 
                graph.end(), compareNodes)->getIndex();

        while((next = bestLink(graph[currentIndex])) != 
                graph[currentIndex].forwardEnd()) {
            node::connection nextConnection = *next;
            OpenPath curPath;
            curPath.appendPoint(graph[currentIndex].data().getPosition());
            curPath.appendPoint(nextConnection.first->data().getPosition());
            labeledpaths.push_back(LabeledOpenPath( 
                    static_cast<PathLabel>(*nextConnection.second), 
                    curPath));
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
    }
    std::cout << "Optimization End!" << std::endl;
}

}


