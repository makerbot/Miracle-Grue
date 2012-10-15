#ifndef MGL_SIMPLE_TOPOLOGY_DECL_H
#define	MGL_SIMPLE_TOPOLOGY_DECL_H

#include <utility>
#include <memory>
#include <vector>
#include <map>

namespace topo {

/*
 This is a graph implementation focused on memory locality. Node data and cost
 types are arbitrary and never used internally except for copying.
 
 There is a vector of node info groups and a vector of costs
 A node info group contains a node, a map of outgoing (forward) links, 
 and a map of incoming (reverse) links. The maps map from node indeces 
 (the other node to which you querry connection) to cost indeces 
 (a vector of whatever cost type you have).
 A node contains a node data, its own index, and a pointer to the parent graph. 
 It provides convenience methods that invokes calls on the parent graph.
 
 Node references will become invalidated on node insertions or removals, 
 so use node indeces (node::getIndex()) and query the graph (graph[i]) if you 
 plan on inserting things into the graph
 
 */

template <typename _NODE_DATA_T, typename _COST_T>
class simple_graph {
public:
    
    class node;
    class node_info_group;
    
    typedef _NODE_DATA_T node_data_type;
    typedef _COST_T cost_type;
    
    //uniquely identifies a node
    typedef size_t node_index;
    //uniquely identifies a cost for a link
    typedef size_t cost_index;
    //maps outgoing links (from destionation to cost)
    typedef std::map<node_index, cost_index> adjacency_map;
    //maps incoming links (from origin to cost)
    typedef std::map<node_index, cost_index> reverse_adjacency_map;
    //contains all nodes, node indeces point into this
    typedef std::vector<node_info_group> node_container_type;
    //maps all costs, cost indeces point into here
    typedef std::vector<cost_type> cost_container_type;
    //these are used for recovering freed space in node and cost containers
    typedef std::vector<node_index> free_node_container_type;
    typedef std::vector<cost_index> free_cost_container_type;
    //allocators!
    typedef std::allocator<node> node_allocator_type;
    typedef std::allocator<cost_type> cost_allocator_type;
    
    class node {
    public:
        friend class simple_graph;
        
        typedef std::pair<node*, const cost_type*> connection;
        
        template <typename BASE>
        class link_iterator {
        public:
            
            typedef BASE base_iterator;
            friend class node;
            
            link_iterator() {}
            
            link_iterator& operator ++(); //pre
            link_iterator operator ++(int); //post
            connection operator *();
            connection operator ->() { return **this; }
            bool operator ==(const link_iterator& other) const;
            bool operator !=(const link_iterator& other) const
                    { return !(*this==other); }
        private:
            explicit link_iterator(base_iterator base, simple_graph* parent) 
                    : m_base(base), m_parent(parent) {}
            base_iterator m_base;
            simple_graph* m_parent;
        };
        
        /* All iterators are forward iterators only. forward and reverse 
         distinguishes between links that go FROM this node (forward) and
         links that go TO this node (reverse) */
        
        typedef link_iterator<adjacency_map::iterator> 
                forward_link_iterator;
        typedef link_iterator<reverse_adjacency_map::iterator> 
                reverse_link_iterator;
        
        node(simple_graph& parent, size_t index, 
                const node_data_type& data = node_data_type());
        
        void connect(const node& other, const cost_type& cost);
        void disconnect(const node& other);
        const node_data_type& data() const { return m_data; }
        
        forward_link_iterator forwardBegin();
        forward_link_iterator forwardEnd();
        reverse_link_iterator reverseBegin();
        reverse_link_iterator reverseEnd();
        
        //true if no outgoing or incoming links
        bool forwardEmpty() const;
        bool reverseEmpty() const;
        
        inline size_t getIndex() const { return m_index; }
        
        bool operator ==(const node& other) { return m_index == other.m_index; }
        bool operator !=(const node& other) { return !(*this==other); } 
        
    private:
        
        simple_graph* m_parent;
        size_t m_index;
        node_data_type m_data;
    };
    
    //cost_gen can be a functor that generates cost types
    template <typename COST_GEN>
    void connect(const node& a, const node& b, const COST_GEN& costGenerator = COST_GEN());
    void connect(const node& a, const node& b, const cost_type& cost);
    void disconnect(const node& a, const node& b);
    
    node& operator [](node_index i);
    
    //create node with this data contents
    node& createNode(const node_data_type& data = node_data_type());
    //clean up all this node's connections
    void destroyNode(node& a);
    void clear();
    bool empty() const { return free_nodes.size() >= nodes.size(); }
    size_t count() const { return nodes.size() - free_nodes.size(); }
    
    class node_info_group {
    public:
        node_info_group(simple_graph& parent, size_t index, 
                const node_data_type& data = node_data_type()) 
                : m_node(parent, index, data), m_valid(true) {}
        node_info_group(const node& other) 
                : m_node(other), m_valid(true) {}
        
        node m_node;
        adjacency_map m_forward_links;
        reverse_adjacency_map m_reverse_links;
        bool m_valid;
    };
    
    template <typename BASE>
    class node_iterator {
    public:
        typedef BASE base_iterator;
        
        friend class simple_graph;
        
        node_iterator() {}
            
        node_iterator& operator ++(); //pre
        node_iterator operator ++(int); //post
        node& operator *();
        node* operator ->() { return &**this; }
        bool operator ==(const node_iterator& other) const;
        bool operator !=(const node_iterator& other) const 
                { return !(*this==other); }
        
    private:
        explicit node_iterator(base_iterator base, base_iterator end) 
                : m_base(base), m_end(end) {}
        base_iterator m_base;
        base_iterator m_end;
    };
    
    typedef node_iterator<typename node_container_type::iterator> 
            forward_node_iterator;
    typedef node_iterator<typename node_container_type::reverse_iterator> 
            reverse_node_iterator;
    
    forward_node_iterator begin();
    forward_node_iterator end();
    reverse_node_iterator rbegin();
    reverse_node_iterator rend();
    
private:
    
    cost_index createCost(const cost_type& cost);
    
    node_container_type nodes;
    cost_container_type costs;
    free_node_container_type free_nodes;
    free_cost_container_type free_costs;
    
    node_allocator_type node_allocator;
    cost_allocator_type cost_allocator;
};

}

#endif	/* MGL_SIMPLE_TOPOLOGY_DECL_H */

