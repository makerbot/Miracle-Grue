/* 
 * File:   pather_hierarchical.cpp
 * Author: Dev
 * 
 * Created on December 14, 2012, 11:25 AM
 */

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
void pather_hierarchical::InsetTree::swap(InsetTree& other) {
    std::swap(m_label, other.m_label);
    m_graph.swap(other.m_graph);
    parent_class::swap(other);
}

}

