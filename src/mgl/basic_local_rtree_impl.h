/* 
 * File:   basic_local_rtree_impl.h
 * Author: Dev
 *
 * Created on December 6, 2012, 12:22 PM
 */

#ifndef MGL_BASIC_LOCAL_RTREE_IMPL_H
#define	MGL_BASIC_LOCAL_RTREE_IMPL_H

#include "basic_local_rtree_decl.h"
#include "intersection_index.h"


namespace mgl {

#define BLRT_TEMPLATE template <typename T, size_t C>
#define BLRT_TYPE basic_local_rtree<T, C>


BLRT_TEMPLATE
BLRT_TYPE::basic_local_rtree() : m_root(DEFAULT_CHILD_PTR()) {}
BLRT_TEMPLATE
BLRT_TYPE::basic_local_rtree(const basic_local_rtree& other) 
        : m_root(DEFAULT_CHILD_PTR()) {
    for(data_const_iterator dataIter = other.m_data.begin(); 
            dataIter != other.m_data.end(); 
            ++dataIter) {
        insert(dataIter->second);
    }
}
BLRT_TEMPLATE
BLRT_TYPE& BLRT_TYPE::operator=(const basic_local_rtree& other) {
    if(&other == this)
        return *this;
    m_root = DEFAULT_CHILD_PTR();
    m_data.clear();
    m_nodes.clear();
    m_freenodes.clear();
    for(data_const_iterator dataIter = other.m_data.begin(); 
            dataIter != other.m_data.end(); 
            ++dataIter) {
        insert(dataIter->second);
    }
    return *this;
}
BLRT_TEMPLATE
typename BLRT_TYPE::iterator BLRT_TYPE::insert(const value_type& value) {
    data_const_iterator iter = m_data.insert(
            bound_value(to_bbox<value_type>::bound(value), value));
    node surrogate(*this, DEFAULT_CHILD_PTR(), iter);
    node& child = acquireNode();
    child.adoptFrom(surrogate);
    if(m_root != DEFAULT_CHILD_PTR()) {
        dereferenceNode(dereferenceNode(m_root).
                selectCandidate(child)).insert(child);
    } else {
        m_root = child.index();
    }
    return iterator();
}
BLRT_TEMPLATE
void BLRT_TYPE::erase(iterator) {}
BLRT_TEMPLATE
typename BLRT_TYPE::node& BLRT_TYPE::acquireNode() {
    if(!m_freenodes.empty()) {
        node& ret = m_nodes[m_freenodes.back()];
        m_freenodes.pop_back();
        return ret;
    } else {
        m_nodes.push_back(node(*this, m_nodes.size()));
        return m_nodes.back();
    }
}
BLRT_TEMPLATE
typename BLRT_TYPE::node& BLRT_TYPE::dereferenceNode(size_t index) {
    return m_nodes[index];
}
BLRT_TEMPLATE
const typename BLRT_TYPE::node& BLRT_TYPE::dereferenceNode(size_t index) const {
    return m_nodes[index];
}
BLRT_TEMPLATE
void BLRT_TYPE::insertPrivate(node& child) {
    
}


BLRT_TEMPLATE
BLRT_TYPE::node::node(basic_local_rtree& parent, size_t index) 
        : m_parent(&parent), m_index(index), 
        m_above(parent.DEFAULT_CHILD_PTR()), 
        m_data(parent.DEFAULT_DATA_PTR()) {
    clearChildren();
}

BLRT_TEMPLATE
BLRT_TYPE::node::node(basic_local_rtree& parent, size_t index, 
        data_const_iterator data)
        : m_parent(&parent), m_index(index), 
        m_above(parent.DEFAULT_CHILD_PTR()), 
        m_data(data), m_bounds(data->first) {
    clearChildren();
}

BLRT_TEMPLATE
bool BLRT_TYPE::node::hasData() const {
    return m_data != m_parent->DEFAULT_DATA_PTR();
}

BLRT_TEMPLATE
bool BLRT_TYPE::node::hasChildren() const {
    return m_childrenCount > 0;
}

BLRT_TEMPLATE
bool BLRT_TYPE::node::hasPurpose() const {
    return hasData() || hasChildren();
}

BLRT_TEMPLATE
const typename BLRT_TYPE::bound_value& BLRT_TYPE::node::data() const {
    return *m_data;
}

BLRT_TEMPLATE
size_t BLRT_TYPE::node::index() const {
    return m_index;
}

BLRT_TEMPLATE
size_t BLRT_TYPE::node::above() const {
    return m_above;
}

BLRT_TEMPLATE
void BLRT_TYPE::node::adoptFrom(node& surrogate) {
    for(size_t i = 0; i < CAPACITY; ++i) {
        std::swap(m_children[i], surrogate.m_children[i]);
    }
    std::swap(m_childrenCount, surrogate.m_childrenCount);
    std::swap(m_data, surrogate.m_data);
    std::swap(m_bounds, surrogate.m_bounds);
}

BLRT_TEMPLATE
size_t BLRT_TYPE::node::selectCandidate(node& child) {
    if(hasData() || !hasChildren())
        return m_index;
    if(m_parent->dereferenceNode(m_children[0]).hasData())
        return m_index;
    Scalar minGrowth = std::numeric_limits<Scalar>::max();
    size_t bestCandidate = 0;
    for(size_t i = 0; i < m_childrenCount; ++i) {
        node& current = m_parent->dereferenceNode(m_children[i]);
        Scalar oldP = current.m_bounds.perimeter();
        Scalar newP = current.m_bounds.expandedTo(child.m_bounds).perimeter();
        Scalar diff = newP - oldP;
        if(diff < minGrowth) {
            minGrowth = diff;
            bestCandidate = i;
        }
    }
    return m_parent->dereferenceNode(bestCandidate).selectCandidate(child);
}

BLRT_TEMPLATE
void BLRT_TYPE::node::insert(node& child) {
    if(m_childrenCount >= CAPACITY) {
        throw LocalTreeException("Overfilled node in insertDirectly");
    }
    m_children[m_childrenCount++] = child.m_index;
    child.m_above = m_index;
    if(m_childrenCount == 1)
        m_bounds = child.m_bounds;
    else
        m_bounds.expandTo(child.m_bounds);
}

BLRT_TEMPLATE
void BLRT_TYPE::node::shareWith(node& sibling) {
    std::vector<size_t> best, worst;
    best.push_back(m_children[--m_childrenCount]);
    while(m_childrenCount != 0) {
        if(best.size() <= worst.size()) {
            Scalar minGrowth = std::numeric_limits<Scalar>::max();
            size_t minChild = 0;
            Scalar bestP = m_parent->dereferenceNode(best.front()).
                    m_bounds.perimeter();
            for(size_t i = 0; i < m_childrenCount; ++i) {
                Scalar growth = m_parent->dereferenceNode(
                        best.front()).m_bounds.expandedTo(
                        m_parent->dereferenceNode(
                        m_children[i]).m_bounds) - bestP;
                if(growth < minGrowth) {
                    minGrowth = growth;
                    minChild = i;
                }
            }
            best.push_back(minChild);
            m_children[minChild] = m_children[--m_childrenCount];
        } else {
            Scalar maxGrowth = std::numeric_limits<Scalar>::min();
            Scalar maxChild = 0;
            Scalar bestP = m_parent->dereferenceNode(best.front()).
                    m_bounds.perimeter();
            for(size_t i = 0; i < m_childrenCount; ++i) {
                Scalar growth = m_parent->dereferenceNode(
                        best.front()).m_bounds.expandedTo(
                        m_parent->dereferenceNode(
                        m_children[i]).m_bounds) - bestP;
                if(growth > maxGrowth) {
                    maxGrowth = growth;
                    maxChild = i;
                }
            }
            worst.push_back(maxChild);
            m_children[maxChild] = m_children[--m_childrenCount];
        }
    }
    clearChildren();
    while(!best.empty()) {
        insert(m_parent->dereferenceNode(best.back()));
        best.pop_back();
    }
    while(!worst.empty()) {
        sibling.insert(m_parent->dereferenceNode(worst.back()));
        worst.pop_back();
    }
}

BLRT_TEMPLATE
void BLRT_TYPE::node::clearChildren() {
    for(size_t i = 0; i < CAPACITY; ++i) {
        m_children[i] = m_parent->DEFAULT_CHILD_PTR();
    }
    m_childrenCount = 0;
}

#undef BLRT_TYPE
#undef BLRT_TEMPLATE

}

#endif	/* MGL_BASIC_LOCAL_RTREE_IMPL_H */

