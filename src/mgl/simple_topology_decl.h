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
    
    typedef size_t node_index;
    typedef size_t cost_index;
    
    typedef std::map<node_index, cost_index> adjacency_map;
    typedef std::map<node_index, cost_index> reverse_adjacency_map;
    
    typedef std::vector<node_info_group> node_container_type;
    typedef std::vector<cost_type> cost_container_type;
    typedef std::vector<node_index> free_node_container_type;
    typedef std::vector<cost_index> free_cost_container_type;
    
    class node {
    public:
        friend class simple_graph;
        node(simple_graph& parent, size_t index, 
                const node_data_type& data = node_data_type());
        
        void connect(const node& other, const cost_type& cost);
        void disconnect(const node& other);
        const node_data_type& data() const { return m_data; }
        
        class forward_link_iterator {
        public:
            friend class node;
            typedef std::pair<node*, const cost_type*> connection;
            forward_link_iterator& operator ++(); //pre
            forward_link_iterator operator ++(int); //post
            connection operator *();
            connection operator ->() { return this->operator *(); }
            bool operator ==(const forward_link_iterator& other) const;
        private:
            explicit forward_link_iterator(adjacency_map::iterator base) 
                    : m_base(base) {}
            adjacency_map::iterator m_base;
        };
        
    private:
        
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
    
private:
    
    cost_index createCost(const cost_type& cost);
    
    node_container_type nodes;
    cost_container_type costs;
    free_node_container_type free_nodes;
    free_cost_container_type free_costs;
};

}

#endif	/* MGL_SIMPLE_TOPOLOGY_DECL_H */

