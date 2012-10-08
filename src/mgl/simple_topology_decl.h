#ifndef MGL_SIMPLE_TOPOLOGY_DECL_H
#define	MGL_SIMPLE_TOPOLOGY_DECL_H

#include <utility>
#include <memory>
#include <vector>
#include <map>

namespace topo {

template <typename _NODE_DATA_T, typename _COST_T>
class simple_graph {
public:
    
    class node;
    class node_info_group;
    
    typedef _NODE_DATA_T node_data_type;
    typedef _COST_T cost_type;
    
    typedef std::map<size_t, cost_type> adjacency_map;
    typedef std::map<size_t, cost_type> reverse_adjacency_map;
    
    typedef std::vector<node_info_group> node_container_type;
    typedef std::vector<size_t> free_container_type;
    
    class node {
    public:
        friend class simple_graph;
        friend class node_info_group;
        friend class node_container_type::allocator_type;
        void connect(const node& other, const cost_type& cost);
        void disconnect(const node& other);
        const node_data_type& data() const { return m_data; }
        
    private:
        node(simple_graph& parent, size_t index, 
                const node_data_type& data = node_data_type());
        node(const node& other);                //allocators only
        node& operator =(const node& other);    //NOT ALLOWED
        
        inline size_t getIndex() const { return m_index; }
        
        simple_graph& m_parent;
        const size_t m_index;
        const node_data_type m_data;
    };
    
    template <typename COST_GEN>
    void connect(node& a, node& b, const COST_GEN& costGenerator = COST_GEN());
    void connect(node& a, node& b, const cost_type& cost);
    void disconnect(node& a, node& b);
    
    node& createNode(const node_data_type& data = node_data_type());
    void destroyNode(node& a);
private:
    
    class node_info_group {
    public:
        node_info_group(simple_graph& parent, size_t index, 
                const node_data_type& data = node_data_type());
        node_info_group(const node& other);
        node_info_group& operator =(const node_info_group& other); // NOT ALLOWED
        
        node m_node;
        adjacency_map m_forward_links;
        reverse_adjacency_map m_reverse_links;
    };
    
    node_container_type graph;
    free_container_type free_space;
};

}

#endif	/* MGL_SIMPLE_TOPOLOGY_DECL_H */

