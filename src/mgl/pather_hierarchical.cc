/* 
 * File:   pather_hierarchical.cpp
 * Author: Dev
 * 
 * Created on December 14, 2012, 11:25 AM
 */

#include <limits>

#include "pather_hierarchical.h"

namespace mgl {

pather_hierarchical::InsetTree::InsetTree() {}
pather_hierarchical::InsetTree::InsetTree(const Loop& loop, 
        const PathLabel& label) : parent_class(loop), 
        m_label(label) {}
void pather_hierarchical::InsetTree::insert(const OpenPath& path, 
        const PathLabel& label) {
    m_graph.insertPath(path, label);
}
void pather_hierarchical::InsetTree::insert(const Loop& loop, 
        const PathLabel& label) {
    m_graph.insertPath(loop, label);
}
pather_hierarchical::InsetTree& pather_hierarchical::InsetTree::insert(
        InsetTree& other) {
    return parent_class::insert(other);
}
void pather_hierarchical::InsetTree::swap(InsetTree& other) {
    std::swap(m_label, other.m_label);
    m_graph.swap(other.m_graph);
    parent_class::swap(other);
}
pather_hierarchical::OutlineTree::OutlineTree() {}
pather_hierarchical::OutlineTree::OutlineTree(const Loop& loop) 
        : parent_class(loop) {}
void pather_hierarchical::OutlineTree::insert(const Loop& loop, 
        const PathLabel& label) {
    if(label.isInset()) {
        InsetTree createdNode(loop, label);
        m_insets.insert(createdNode);
    } else {
        m_graph.insertPath(loop, label);
    }
}
void pather_hierarchical::OutlineTree::insert(const OpenPath& path, 
        const PathLabel& label) {
    m_graph.insertPath(path, label);
}
void pather_hierarchical::OutlineTree::swap(OutlineTree& other) {
    m_insets.swap(other.m_insets);
    m_graph.swap(other.m_graph);
    parent_class::swap(other);
}
pather_hierarchical::OutlineTree::iterator 
        pather_hierarchical::OutlineTree::selectBestChild(
        Point2Type& entryPoint) {
    iterator bestIter = end();
    Scalar bestSquaredMagnitude = std::numeric_limits<Scalar>::max();
    for(iterator iter = begin(); iter != end(); ++iter) {
        Scalar currentSquaredMagnitude = std::numeric_limits<Scalar>::max();
        for(Loop::const_finite_cw_iterator loopIter = 
                iter->boundary().clockwiseFinite(); 
                loopIter != iter->boundary().clockwiseEnd(); 
                ++loopIter) {
            Scalar squaredMagnitude = (Point2Type(*loopIter) - 
                    entryPoint).squaredMagnitude();
            if(squaredMagnitude < currentSquaredMagnitude) {
                currentSquaredMagnitude = squaredMagnitude;
            }
        }
        if(currentSquaredMagnitude < bestSquaredMagnitude) {
            bestSquaredMagnitude = currentSquaredMagnitude;
            bestIter = iter;
        }
    }
    return bestIter;
}

}

