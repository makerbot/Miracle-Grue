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
    node element(*this, iter);
    return iterator(element);
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
BLRT_TYPE::node::node(basic_local_rtree& parent, size_t index) 
        : m_parent(&parent), m_index(index), 
        m_data(parent.DEFAULT_DATA_PTR()) {
    clearChildren();
}

BLRT_TEMPLATE
BLRT_TYPE::node::node(basic_local_rtree& parent, size_t index, 
        data_const_iterator data)
        : m_parent(&parent), m_index(index),
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
void BLRT_TYPE::node::adoptFrom(node& surrogate) {
    for(size_t i = 0; i < CAPACITY; ++i) {
        std::swap(m_children[i], surrogate.m_children[i]);
    }
    std::swap(m_childrenCount, surrogate.m_childrenCount);
    std::swap(m_data, surrogate.m_data);
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

