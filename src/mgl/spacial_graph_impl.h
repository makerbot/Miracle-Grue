/* 
 * File:   spacial_graph_impl.h
 * Author: Dev
 *
 * Created on December 12, 2012, 2:22 PM
 */

#ifndef MGL_SPACIAL_GRAPH_IMPL_H
#define	MGL_SPACIAL_GRAPH_IMPL_H

#include "spacial_graph_decl.h"
#include "intersection_index.h"
#include <algorithm>

namespace mgl {

template <typename PATH_TYPE>
void SpacialGraph::insertPath(
        const basic_labeled_path<PATH_TYPE>& labeledPath) {
    insertPath(labeledPath.myPath, labeledPath.myPath);
}
template <typename COLLECTION>
void SpacialGraph::insertPaths(const COLLECTION& collection) {
    for(typename COLLECTION::const_iterator iter = collection.begin(); 
            iter != collection.end(); 
            ++iter) {
        insertPath(*iter);
    }
}
template <typename COLLECTION>
void SpacialGraph::insertPaths(
        const COLLECTION& collection, const PathLabel& label) {
    for(typename COLLECTION::const_iterator iter = collection.begin(); 
            iter != collection.end(); 
            ++iter) {
        insertPath(*iter, label);
    }
}

template <typename LABEL_PREDICATE, typename BOUNDARY_TEST>
void SpacialGraph::optimize(LabeledOpenPaths& result, Point2Type& entryPoint, 
        const LABEL_PREDICATE& labeler, const BOUNDARY_TEST& bounder) {
    LabeledOpenPath currentPath;
    cost_predicate<LABEL_PREDICATE> costCompare(labeler);
    node::forward_link_iterator linkIter;
    while(!m_graph.empty()) {
        graph_type::node_index currentNode = 
                findBestNode(entryPoint, labeler);
        smartAppendPath(result, currentPath);
        smartConnect(entryPoint, m_graph[currentNode].data().position(), 
                result, bounder);
        smartAppendPoint(m_graph[currentNode].data().position(), 
                m_graph[currentNode].data().label(), 
                result, currentPath, entryPoint);
        while((linkIter = 
                selectBestLink(currentNode, costCompare, bounder, entryPoint)) 
                != m_graph[currentNode].forwardEnd()) {
            graph_type::node_index nextNode = linkIter->first->getIndex();
            node& currentNodeRef = m_graph[currentNode];
            node& nextNodeRef = m_graph[nextNode];
            smartAppendPoint(nextNodeRef.data().position(), 
                    *(linkIter->second), result, currentPath, entryPoint);
            currentNodeRef.disconnect(nextNodeRef);
            nextNodeRef.disconnect(currentNodeRef);
            if(currentNodeRef.forwardEmpty() && 
                    currentNodeRef.reverseEmpty()) {
                destroyNode(currentNode);
            }
            currentNode = nextNode;
        }
        node& oldNodeRef = m_graph[currentNode];
        if(oldNodeRef.forwardEmpty() && 
                oldNodeRef.reverseEmpty()) {
            destroyNode(oldNodeRef.getIndex());
        }
    }
    smartAppendPath(result, currentPath);
}

template <typename LABEL_PREDICATE, typename BOUNDARY_TEST>
SpacialGraph::node::forward_link_iterator 
        SpacialGraph::selectBestLink(graph_type::node_index index, 
        const cost_predicate<LABEL_PREDICATE>& labeler, 
        const BOUNDARY_TEST&, //bounder, 
        const Point2Type& //entryPoint
        ) {
    return std::min_element(m_graph[index].forwardBegin(), 
            m_graph[index].forwardEnd(), 
            labeler);
}

template <typename LABEL_PREDICATE>
SpacialGraph::graph_type::node_index SpacialGraph::findBestNode(
        const Point2Type& point, const LABEL_PREDICATE& labeler) {
    std::vector<graph_node_reference> candidates;
    m_tree.search(candidates, BBoxFilter(AABBox(point).adjusted(
            Point2Type(-NEAREST_QUICKTEST_THRESHOLD, 
            -NEAREST_QUICKTEST_THRESHOLD), 
            Point2Type(NEAREST_QUICKTEST_THRESHOLD, 
            NEAREST_QUICKTEST_THRESHOLD))));
    distance_predicate<LABEL_PREDICATE> myComparator(labeler, m_graph, point);
    if(!candidates.empty()) {
        //select from the small data set we got from the r-tree
        return std::min_element(candidates.begin(), candidates.end(), 
                myComparator)->second;
    } else {
        //search all the nodes
        return std::min_element(m_graph.begin(), 
                m_graph.end(), myComparator)->getIndex();
    }
}

template <typename BOUNDARY_TEST>
void SpacialGraph::smartConnect(Point2Type& entryPoint, 
        Point2Type destPoint, 
        LabeledOpenPaths& result, 
        const BOUNDARY_TEST& bounder) {
    Segment2Type connection(entryPoint, destPoint);
    if(bounder(connection)) {
        PathLabel label(PathLabel::TYP_CONNECTION, PathLabel::OWN_MODEL, 0);
        LabeledOpenPath connectionPath(label);
        connectionPath.myPath.appendPoint(entryPoint);
        connectionPath.myPath.appendPoint(destPoint);
        result.push_back(connectionPath);
    }
    entryPoint = destPoint;
}

template <typename BASE>
int SpacialGraph::cost_predicate<BASE>::compare(
        const value_type& lhs, const value_type& rhs) const {
    int result = m_base.compare(lhs, rhs);
    if(result)
        return result;
    return (lhs.squaredDistance() < rhs.squaredDistance()) ? 
            BETTER : ((lhs.squaredDistance() > rhs.squaredDistance()) ? 
            WORSE : SAME);
}

template <typename BASE>
bool SpacialGraph::cost_predicate<BASE>::operator ()(
        const node::connection& lhs, 
        const node::connection& rhs) const {
    return abstract_predicate<CostData>::operator ()(
            *lhs.second, *rhs.second);
}

///compare node refs
template <typename BASE>
int SpacialGraph::distance_predicate<BASE>::compare(
        const value_type& lhs, const value_type& rhs) const {
    int result = m_base.compare(lhs.data().label(), 
            rhs.data().label());
    if(result)
        return result;
    Scalar ldistance = (lhs.data().position() - 
            m_point).squaredMagnitude();
    Scalar rdistance = (rhs.data().position() - 
            m_point).squaredMagnitude();
    return (ldistance < rdistance) ? BETTER : 
            ((ldistance > rdistance) ? WORSE : SAME);
}
///compare node indexes, use internal graph ref
template <typename BASE>
int SpacialGraph::distance_predicate<BASE>::compare(
        const graph_type::node_index& lhs, 
        const graph_type::node_index& rhs) const {
    return compare(m_graph[lhs], m_graph[rhs]);
}
///compare graph node refs, use stored graph ptr and node index
template <typename BASE>
int SpacialGraph::distance_predicate<BASE>::compare(
        const graph_node_reference& lhs, 
        const graph_node_reference& rhs) const {
    return compare(lhs.first->operator [](lhs.second), 
            rhs.first->operator [](rhs.second));
}


template <typename SPACIAL_CONTAINER>
basic_boundary_test<SPACIAL_CONTAINER>::basic_boundary_test(
        const container_type& container) : m_container(container) {}
template <typename SPACIAL_CONTAINER>
bool basic_boundary_test<SPACIAL_CONTAINER>::operator ()(
        const Segment2Type& testSegment) const {
    typedef std::vector<Segment2Type> candidate_container;
    candidate_container candidates;
    LineSegmentFilter filter(testSegment);
    m_container.search(candidates, filter);
    for(candidate_container::const_iterator iter = candidates.begin(); 
            iter != candidates.end(); 
            ++iter) {
        if(iter->intersects(testSegment))
            return false;
    }
    return true;
}

}

#endif	/* MGL_SPACIAL_GRAPH_IMPL_H */

