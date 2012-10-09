#ifndef MGL_SIMPLE_TOPOLOGY_IMPL_H
#define	MGL_SIMPLE_TOPOLOGY_IMPL_H

#include "simple_topology_decl.h"

namespace topo {

#define SG_TEMPLATE template <typename _NODE_DATA_T, typename _COST_T>
#define SG_TYPE simple_graph<_NODE_DATA_T, _COST_T>
#define SG_NODE SG_TYPE::node

SG_TEMPLATE
SG_NODE::node(simple_graph& parent, size_t index, const node_data_type& data)
        : m_parent(parent), m_index(index), m_data(data) {}
SG_TEMPLATE
void SG_NODE::connect(const node& other, const cost_type& cost) {
    m_parent.connect(*this, other, cost);
}
SG_TEMPLATE
void SG_NODE::disconnect(const node& other) {
    m_parent.disconnect(*this, other);
}
SG_TEMPLATE
typename SG_NODE::forward_link_iterator SG_NODE::forwardBegin() {
    return forward_link_iterator(
            m_parent.nodes[getIndex()].m_forward_links.begin());
}
SG_TEMPLATE
typename SG_NODE::forward_link_iterator SG_NODE::forwardEnd() {
    return forward_link_iterator(
            m_parent.nodes[getIndex()].m_forward_links.end());
}
SG_TEMPLATE
typename SG_NODE::reverse_link_iterator SG_NODE::reverseBegin() {
    return reverse_link_iterator(
            m_parent.nodes[getIndex()].m_reverse_links.begin());
}
SG_TEMPLATE
typename SG_NODE::reverse_link_iterator SG_NODE::reverseEnd() {
    return reverse_link_iterator(
            m_parent.nodes[getIndex()].m_reverse_links.end());
}
SG_TEMPLATE template <typename BASE>
SG_NODE::link_iterator<BASE>& 
        SG_NODE::link_iterator<BASE>::operator ++() {
    ++m_base;
    return *this;
}
SG_TEMPLATE template <typename BASE>
SG_NODE::link_iterator<BASE> 
        SG_NODE::link_iterator<BASE>::operator ++(int) {
    link_iterator copy = *this;
    ++*this;
    return copy;
}
SG_TEMPLATE template <typename BASE>
typename SG_NODE::connection 
        SG_NODE::link_iterator<BASE>::operator *() {
    return connection(&m_parent.nodes[m_base->first].m_node, 
            &m_parent.costs[m_base->second]);
}
SG_TEMPLATE template <typename BASE>
bool SG_NODE::link_iterator<BASE>::operator ==(
        const link_iterator& other) const {
    return m_base == other.m_base;
}
SG_TEMPLATE
template <typename COST_GEN>
void SG_TYPE::connect(node& a, node& b, const COST_GEN& costGenerator) {
    connect(a, b, costGenerator(a.data(), b.data()));
}
SG_TEMPLATE
void SG_TYPE::connect(node& a, node& b, const cost_type& cost) {
    cost_index genCost = createCost(cost);
    nodes[a.getIndex()].m_forward_links[b.getIndex()] = genCost;
    nodes[b.getIndex()].m_reverse_links[a.getIndex()] = genCost;
}
SG_TEMPLATE
void SG_TYPE::disconnect(node& a, node& b) {
    adjacency_map& forward_map = 
            nodes[a.getIndex()].m_forward_links;
    reverse_adjacency_map& reverse_map = 
            nodes[b.getIndex()].m_reverse_links;
    adjacency_map::iterator forward = forward_map.find(b.getIndex());
    adjacency_map::iterator reverse = reverse_map.find(a.getIndex());
    if(forward != forward_map.end()) {
        free_costs.push_back(forward->second);
        forward_map.erase(forward);
    }
    if(reverse != reverse_map.end()) {
        reverse_map.erase(reverse);
    }
}
SG_TEMPLATE
typename SG_NODE& SG_TYPE::createNode(const node_data_type& data) {
    size_t index = -1;
    if(!free_nodes.empty()) {
        index = free_nodes.back();
        free_nodes.pop_back();
        nodes[index].m_node = node(*this, index, data);
    } else {
        index = nodes.size();
        nodes.push_back(node(*this, index, data));
    }
    nodes[index].m_valid = true;
    return nodes[index].m_node;
}
SG_TEMPLATE
void SG_TYPE::destroyNode(node& a) {
    size_t currentIndex = a.getIndex();
    node_info_group& currentNode = nodes[currentIndex];
    for(adjacency_map::const_iterator iter = 
            currentNode.m_forward_links.begin(); 
            iter != currentNode.m_forward_links.end(); 
            ++iter) {
        nodes[iter->first].m_reverse_links.erase(currentIndex);
        free_costs.push_back(iter->second);
    }
    for(reverse_adjacency_map::const_iterator iter = 
            currentNode.m_reverse_links.begin(); 
            iter != currentNode.m_reverse_links.end(); 
            ++iter) {
        nodes[iter->first].m_forward_links.erase(currentIndex);
        free_costs.push_back(iter->second);
    }
    nodes[currentIndex].m_forward_links.clear();
    nodes[currentIndex].m_reverse_links.clear();
    nodes[currentIndex].m_valid = false;
    free_nodes.push_back(a.getIndex());
}
SG_TEMPLATE
void SG_TYPE::clear() {
    nodes.clear();
    costs.clear();
    free_nodes.clear();
    free_costs.clear();
}
SG_TEMPLATE template <typename BASE>
SG_TYPE::node_iterator<BASE>& SG_TYPE::node_iterator<BASE>::operator ++() {
    do { ++m_base; } while(m_base != m_end && !m_base->m_valid);
    return *this;
}
SG_TEMPLATE template <typename BASE>
SG_TYPE::node_iterator<BASE> SG_TYPE::node_iterator<BASE>::operator ++(int) {
    node_iterator copy = *this;
    ++*this;
    return copy;
}
SG_TEMPLATE template <typename BASE>
typename SG_NODE& SG_TYPE::node_iterator<BASE>::operator *() {
    return m_base->m_node;
}
SG_TEMPLATE template <typename BASE>
bool SG_TYPE::node_iterator<BASE>::operator ==(
        const node_iterator& other) const {
    return m_base == other.m_base;
}
SG_TEMPLATE
typename SG_TYPE::forward_node_iterator SG_TYPE::begin() {
    return forward_node_iterator(nodes.begin(), nodes.end());
}
SG_TEMPLATE
typename SG_TYPE::forward_node_iterator SG_TYPE::end() {
    return forward_node_iterator(nodes.end(), nodes.end());
}
SG_TEMPLATE
typename SG_TYPE::reverse_node_iterator SG_TYPE::rbegin() {
    return reverse_node_iterator(nodes.begin(), nodes.end());
}
SG_TEMPLATE
typename SG_TYPE::reverse_node_iterator SG_TYPE::rend() {
    return reverse_node_iterator(nodes.rend(), nodes.rend());
}

SG_TEMPLATE
typename SG_TYPE::cost_index SG_TYPE::createCost(const cost_type& cost) {
    cost_index ret = -1;
    if(!free_costs.empty()) {
        ret = free_costs.back();
        free_costs.pop_back();
        costs[ret] = cost;
    } else {
        ret = costs.size();
        costs.push_back(cost);
    }
    return ret;
}

#undef SG_NODE
#undef SG_TYPE
#undef SG_TEMPLATE

}

#endif	/* SIMPLE_TOPOLOGY_IMPL_H */

