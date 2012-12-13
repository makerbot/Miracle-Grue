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
    
}

template <typename LABEL_PREDICATE, typename BOUNDARY_TEST>
SpacialGraph::node::forward_link_iterator 
        SpacialGraph::selectBestLink(graph_type::node_index index, 
        const cost_predicate<LABEL_PREDICATE>& labeler, 
        const BOUNDARY_TEST& bounder, const Point2Type& entryPoint) {
    return m_graph[index].forwardEnd();
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

