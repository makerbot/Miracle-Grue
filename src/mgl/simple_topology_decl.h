#ifndef MGL_SIMPLE_TOPOLOGY_DECL_H
#define	MGL_SIMPLE_TOPOLOGY_DECL_H

#include <cstdlib>
#include <utility>
#include <memory>
#include <vector>
#include <map>

namespace topo {

/**
 @brief A graph representation focused on memory locality
 @param _NODE_DATA_T    the data element to store at each node. Ideally this 
                        should be a POD that's fast to copy.
 @param _COST_T         the cost representation to store at each link. 
                        Ideally this should be a POD that's fast to copy.
 
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
    
    ///uniquely identifies a node
    typedef size_t node_index;
    ///uniquely identifies a cost for a link
    typedef size_t cost_index;
    ///maps outgoing links (from destionation to cost)
    typedef std::map<node_index, cost_index> adjacency_map;
    ///maps incoming links (from origin to cost)
    typedef std::map<node_index, cost_index> reverse_adjacency_map;
    ///contains all nodes, node indeces point into this
    typedef std::vector<node_info_group> node_container_type;
    ///maps all costs, cost indeces point into here
    typedef std::vector<cost_type> cost_container_type;
    ///these are used for recovering freed space in node and cost containers
    typedef std::vector<node_index> free_node_container_type;
    typedef std::vector<cost_index> free_cost_container_type;
    ///allocators!
    typedef std::allocator<node> node_allocator_type;
    typedef std::allocator<cost_type> cost_allocator_type;
    
    /**
     @brief The node stores the node data item, provides an interface to
     retrieve its incoming and outgoing links, and allows convenient 
     connecting and disconnecting to/from other nodes. 
     Nodes must belong to the same graph for these operations to be valid.
     */
    class node {
    public:
        friend class simple_graph;
        
        typedef std::pair<node*, const cost_type*> connection;
        
        /**
         @brief A template iterator for a node's outgoing and incoming links.
         Use forward_link_iterator and reverse_link_iterator specializations 
         instead of this template. Do not use this template directly.
         
         Iterators are NOT random access and NOT bidirectional!
         */
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
        
        /**
         @brief A forward iterator for outgoing links
         */
        typedef link_iterator<adjacency_map::iterator> 
                forward_link_iterator;
        /**
         @brief A forward iterator for incoming links
         */
        typedef link_iterator<reverse_adjacency_map::iterator> 
                reverse_link_iterator;
        
        /**
         @brief Do not use directly. Call simple_graph::createNode(data) 
         instead!
         */
        node(simple_graph& parent, size_t index, 
                const node_data_type& data = node_data_type());
        /**
         @brief Connect this node to node @a other with a link having the 
         cost @a cost.
         @param other the node to which this will be connected. Must belong 
         to the same simple_graph object as this.
         @param cost the cost object which will be stored in the link
         Create a directed link from this to other with cost @a cost. This 
         node will contain a new outgoing link with @a cost and other node 
         will contain a new incoming link with @a cost.
         Nodes can be connected to themselves. Between any two nodes, only 
         one connection can exist in each direction.
         
         <code>
         // A and B are nodes in the same simple_graph
         // cost_type is an integer in this example
         A.connect(B, 1);   //creates a link from A to B with cost 1
         B.connect(A, 3);   //creates a link from B to A with cost 3
                            //there is now a link from A to B and another from 
                            //B to A
         A.connect(B, 2);   //overwrites the link from A to B, there is still
                            //one link from A to B, but it now has cost 2
         </code>
         */
        void connect(const node& other, const cost_type& cost);
        /**
         @brief disconnect other from this
         @param other the node from which this will be disconnected
         If there exists a link from this node to node @a other, 
         it will be erased. Otherwise, this function has no effect.
         */
        void disconnect(const node& other);
        /**
         @brief access the data item contained at this node
         @return the data item contained at this node
         */
        const node_data_type& data() const { return m_data; }
        
        forward_link_iterator forwardBegin();
        forward_link_iterator forwardEnd();
        reverse_link_iterator reverseBegin();
        reverse_link_iterator reverseEnd();
        
        /// test if there are no outgoing links
        bool forwardEmpty() const;
        /// test if there are no incoming links
        bool reverseEmpty() const;
        /// get this node's unique identifier, use for querying simple_graph
        inline const node_index& getIndex() const { return m_index; }
        
        bool operator ==(const node& other) { return m_index == other.m_index; }
        bool operator !=(const node& other) { return !(*this==other); } 
        
    private:
        
        simple_graph* m_parent;
        size_t m_index;
        node_data_type m_data;
    };
    
    /**
     @brief connect node a to node b using a cost generating function
     @param COST_GEN any function-like object that can generate a cost from 
     node a and node b
     @param a node from which the connection will be made
     @param b node to which the connection will be made
     @param costGenerator object used to generate the cost
     */
    template <typename COST_GEN>
    void connect(const node& a, const node& b, const COST_GEN& costGenerator = COST_GEN());
    /**
     @brief connect node a to node b with cost
     @param a node from which the connection will be made
     @param b node to which the connection will be made
     @param cost the cost to be stored in the created link
     node::connect(other, cost) invokes this function on the node's parent 
     simple_graph.
     */
    void connect(const node& a, const node& b, const cost_type& cost);
    /**
     @brief disconnect node b from node a
     @param a node from which the connection will be removed
     @param b node to which the connection will be removed
     node::disconnect(other) invokes this function on the node's parent
     simple_graph.
     */
    void disconnect(const node& a, const node& b);
    
    /**
     @brief retrieve a node using a unique node_index
     @param i a unique node_index as returned by node::getIndex()
     @return node reference to node uniquely identified by @a i
     @throws std::out_of_range
     The returned reference will be invalidated by future calls to 
     createNode(data)
     */
    node& operator [](node_index i);
    /**
     @brief retrieve a node using a unique node_index
     @param i a unique node_index as returned by node::getIndex()
     @return node const reference to node uniquely identified by @a i
     @throws std::out_of_range
     The returned reference will be invalidated by future calls to 
     createNode(data)
     */
    const node& operator [](node_index i) const;
    
    /**
     @brief fast, efficient swap implementation
     @param other the simple_graph with which to swap contents
     Does not cause memory to be allocated or deallocated. 
     Cost is proportional to the number of nodes in this and other.
     This is because we need to update the parent graph pointer.
     */
    void swap(simple_graph& other);
    
    /**
     @brief construct a node containing @a data
     @param data the data the new node will contain
     @return reference to the created node.
     The returned reference will be invalidated by future calls to 
     createNode(data). Instead use node.getIndex() to store the unique 
     identifier, then query simple graph using operator [](node_index);
     */
    node& createNode(const node_data_type& data = node_data_type());
    /**
     @brief cleanly remove a node
     @param a a node reference that belongs to this simple_Graph
     This function will cleanly remove all connections to @a a and then 
     invalidate it.
     */
    void destroyNode(node& a);
    /**
     @brief remove all nodes and links from this graph
     */
    void clear();
    /// test if there are no nodes contained in this graph
    bool empty() const { return free_nodes.size() >= nodes.size(); }
    /// get the number of nodes in this graph
    size_t count() const { return nodes.size() - free_nodes.size(); }
    
    /// Class used internally to keep track of nodes and links. Not part of API.
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
    
    /// use for iterating over nodes contained in this graph
    forward_node_iterator begin();
    /// use for iterating over nodes contained in this graph
    forward_node_iterator end();
    /// use for iterating over nodes contained in this graph
    reverse_node_iterator rbegin();
    /// use for iterating over nodes contained in this graph
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

namespace std {

/// specialization of std::swap for simple_graph
template <typename _NODE_DATA_T, typename _COST_T>
void swap(topo::simple_graph<_NODE_DATA_T, _COST_T>& lhs, 
        topo::simple_graph<_NODE_DATA_T, _COST_T>& rhs) {
    lhs.swap(rhs);
}

}

#endif	/* MGL_SIMPLE_TOPOLOGY_DECL_H */

