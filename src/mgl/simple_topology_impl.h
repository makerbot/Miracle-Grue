#ifndef MGL_SIMPLE_TOPOLOGY_IMPL_H
#define	MGL_SIMPLE_TOPOLOGY_IMPL_H

#include "simple_topology_decl.h"

namespace topo {

#define SG_TEMPLATE template <typename _NODE_DATA_T, typename _COST_T>
#define SG_TYPE simple_graph<_NODE_DATA_T, _COST_T>

SG_TEMPLATE
SG_TYPE::node::node(simple_graph& parent, size_t index, const node_data_type& data)
        : m_parent(parent), m_index(index), m_data(data) {}
SG_TEMPLATE
SG_TYPE::node::node(const node& other) 
        : m_parent(other.m_parent), m_index(other.m_index), 
        m_data(other.m_data) {}
SG_TEMPLATE
void SG_TYPE::node::connect(const node& other, const cost_type& cost) {
    m_parent.connect(*this, other, cost);
}
SG_TEMPLATE
void SG_TYPE::node::disconnect(const node& other) {
    m_parent.disconnect(*this, other);
}
SG_TEMPLATE
template <typename COST_GEN>
void SG_TYPE::connect(node& a, node& b, const COST_GEN& costGenerator) {
    connect(a, b, costGenerator(a.data(), b.data()));
}
SG_TEMPLATE
void SG_TYPE::connect(node& a, node& b, const _COST_T& cost) {
    graph[a.getIndex()].m_forward_links[b.getIndex()] = cost;
    graph[b.getIndex()].m_reverse_links[a.getIndex()] = cost;
}
SG_TEMPLATE
void SG_TYPE::disconnect(node& a, node& b) {
    graph[a.getIndex()].m_forward_links.erase(b.getIndex());
    graph[b.getIndex()].m_reverse_links.erase(a.getIndex());
}
SG_TEMPLATE
SG_TYPE::node& SG_TYPE::createNode(const node_data_type& data) {
    if(!free_space.empty()) {
        size_t index = free_space.back();
        free_space.pop_back();
        graph[index].m_node = node(*this, index, data);
    } else {
        size_t index = graph.size();
        graph.push_back(node(*this, index, data));
    }
}
SG_TEMPLATE
void SG_TYPE::destroyNode(node& a) {
    graph[a.getIndex()].m_forward_links.clear();
    graph[a.getIndex()].m_reverse_links.clear();
    free_space.push_back(a.getIndex());
}


#undef SG_TYPE
#undef SG_TEMPLATE

}

#endif	/* SIMPLE_TOPOLOGY_IMPL_H */

