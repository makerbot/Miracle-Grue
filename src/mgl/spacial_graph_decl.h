/* 
 * File:   spacial_graph_decl.h
 * Author: Dev
 *
 * Created on December 12, 2012, 2:23 PM
 */

#ifndef MGL_SPACIAL_GRAPH_DECL_H
#define	MGL_SPACIAL_GRAPH_DECL_H

#include "simple_topology.h"
#include "basic_local_rtree.h"
#include "labeled_path.h"
#include "loop_path.h"
#include "predicate.h"

namespace mgl {

/**
 @brief A class that combines a graph and an rtree of its nodes, that is used 
 for quickly finding nearest entry points when optimizing. 
 This class contains only a simple_graph, and an rtree of the nodes in this 
 simple graph. It keeps the two consistent, which allows for much faster than 
 linear time searches of things like nearest entry points. This meant to be 
 fast to swap, reasonably fast to copy.
 The motivation was to avoid having static private functions to which I 
 pass graph references.
 */
class SpacialGraph {
public:
    class NodeData;
    class CostData;
    
    typedef topo::simple_graph<NodeData, CostData> graph_type;
    typedef std::pair<graph_type*, graph_type::node_index> graph_node_reference;
    typedef basic_local_rtree<graph_node_reference> tree_type;
    typedef graph_type::node node;
    
    typedef std::list<LabeledOpenPath> LabeledOpenPaths;
    
    class NodeData {
    public:
        NodeData(Point2Type position, PathLabel label, bool entry)
                : m_position(position), m_label(label), m_isEntry(entry) {}
        NodeData() : m_isEntry(false) {}
        const Point2Type& position() const { return m_position; }
        const PathLabel& label() const { return m_label; }
        bool isEntry() const { return m_isEntry; }
        /**
         @brief stores the iterator that points to this node's index in a 
         parent object's r-tree. Used for when we want to erase nodes from 
         the graph and the r-tree.
         mutable so it can be updated after the node is created
         */
        mutable tree_type::iterator m_treeIterator;
    private:
        /// This node's position in 2D space
        Point2Type m_position;
        /// The label of the path with which this node is associated
        PathLabel m_label;
        /// is this a valid entry node or no?
        bool m_isEntry;
    };
    
    class CostData : public PathLabel {
    public:
        CostData(const PathLabel& label = PathLabel(), 
                Scalar distanceSquared = 0, 
                Point2Type normal = Point2Type())
                : PathLabel(label), m_squaredDistance(distanceSquared), 
                m_normal(normal) {}
        Scalar squaredDistance() const { return m_squaredDistance; }
        const Point2Type& normal() const { return m_normal; }
    private:
        Scalar m_squaredDistance;
        Point2Type m_normal;
    };
    
    /**
     @brief A filter iterator that allows convenient access to only those 
     nodes in a graph that are entry points.
     */
    class entry_iterator {
    public:
        
        friend class SpacialGraph;
        
        entry_iterator() {}
            
        entry_iterator& operator ++() /*pre*/ { 
            do{ ++m_base; } while(m_base != m_end && !m_base->data().isEntry());
            return *this; }
        entry_iterator operator ++(int) /*post*/ 
                { entry_iterator dup = *this; ++*this; return dup; }
        node& operator *() { return *m_base; }
        node* operator ->() { return &**this; }
        bool operator ==(const entry_iterator& other) const 
                { return m_base == other.m_base; }
        bool operator !=(const entry_iterator& other) const
                { return !(*this==other); }
        
    private:
        explicit entry_iterator(graph_type::forward_node_iterator base, 
                graph_type::forward_node_iterator end) 
                : m_base(base), m_end(end) {}
        graph_type::forward_node_iterator m_base;
        graph_type::forward_node_iterator m_end;
    };
    
    /**
     @brief BASE is a predicate on 
     PathLabels. BASE will select which label is the better one, and this 
     class will break ties based on information
     */
    template <typename BASE>
    class cost_predicate : public abstract_predicate<CostData> {
    public:
        typedef CostData value_type;
        template <typename T>
        cost_predicate(const T& arg) : m_base(arg) {}
        int compare(const value_type& lhs, const value_type& rhs) const;
    protected:
        BASE m_base;
    };
    
    SpacialGraph();
    SpacialGraph(const SpacialGraph& other);
    SpacialGraph& operator =(const SpacialGraph& other);
    
    /**
     @brief Insert @a path with @a label into the graph, creating correct 
     nodes, links, and marking correct entries
     @param path the OpenPath to insert
     @param label the label of @a path
     */
    void insertPath(const OpenPath& path, const PathLabel& label);
    /**
     @brief Insert @a loop with @a label into the graph, creating correct 
     nodes, links, and marking correct entries
     @param loop the Loop to insert
     @param label the label of @a loop
     */
    void insertPath(const Loop& loop, const PathLabel& label);
    /**
     @brief Insert a LabeledPath or LabeledLoop into the graph by invoking 
     the correct overload of insertPath declared above
     @param PATH_TYPE template parameter for the type: loop or path
     @param labeledPath a LabeledLoop or LabeledOpenPath to insert
     */
    template <typename PATH_TYPE>
    void insertPath(const basic_labeled_path<PATH_TYPE>& labeledPath);
    /**
     @brief insert an stl collection of compatible objects into the graph
     @param COLLECTION the type of collection we're inserting
     @param collection the instance of the collection we're inserting. This 
     should be any collection that supports const_iterator, begin(), end(), 
     and const_iterator::operator *() should return a type of object 
     accepted the insertPath(basic_labeled_path...) template
     */
    template <typename COLLECTION>
    void insertPaths(const COLLECTION& collection);
    /**
     @brief insert an stl collection of compatible objects into the graph
     @param COLLECTION the type of collection we're inserting
     @param collection the instance of the collection we're inserting. This 
     should be any collection that supports const_iterator, begin(), end(), 
     and const_iterator::operator *() should return a loop or an open path. 
     @param label the label to be applied to all the objects inserted
     */
    template <typename COLLECTION>
    void insertPaths(const COLLECTION& collection, const PathLabel& label);
    
    ///conveniently iterate over entry points
    entry_iterator entryBegin();
    ///conveniently iterate over entry points
    entry_iterator entryEnd();
    
    ///@return true if no nodes remain in the graph, else false
    bool empty() const;
    ///Does this really need any explanation?
    void clear();
    
    void repr(std::ostream& out);
    void repr_svg(std::ostream& out);
    
    /**
     @brief Optimized the contents of this graph using an arbitrary
     method of comparing labels and testing for boundary intersections
     @param LABEL_PREDICATE the type of predicate to compare labels
     @param BOUNDARY_TEST the type of functor to test for boundary conditions, 
     BOUNDARY_TEST::operator(const Segment2Type&) const will return true if the 
     line segment is a valid path to extrude over, otherwise false
     @param result here will be placed reults of optimization
     @param entryPoint holds the position where the last thing was printed, 
     so we know where best to start. After this function returns, 
     this variable will hold the last position we traversed.
     @param labeler instance of object used to compare labels
     @param bounder instance of object to determine valid connections, 
     ideally a simple class that tests the input segment against a spacial 
     data structure of boundaries, but we don't really care how its implemented
     
     This function is destructive! It will leave the contained graph empty.
     */
    template <typename LABEL_PREDICATE, typename BOUNDARY_TEST>
    void optimize(LabeledOpenPaths& result, 
            Point2Type& entryPoint, 
            const LABEL_PREDICATE& labeler = LABEL_PREDICATE(), 
            const BOUNDARY_TEST& bounder = BOUNDARY_TEST());
    
private:
    
    /**
     @brief From the node at @a index, select or create the best possible 
     link according to @a labeler that satisfied @a bounder, possibly 
     using @a entryPoint as a hint
     @param LABEL_PREDICATE see LABEL_PREDICATE in SpacialGraph::optimize
     @param BOUNDARY_TEST see BOUNDARY_TEST in SpacialGraph::optimize
     @param index the index of the node from which to select a forward link
     @param labeler SpacialGraph::optimize will construct a predicate and 
     pass it to this function, used to prioritize certain labels
     @param bounder Functor on Segment2Type that determines where new 
     connections can be made.
     @param entryPoint the position of the end of the last movement, 
     may be used to hint at best next movement.
     */
    template <typename LABEL_PREDICATE, typename BOUNDARY_TEST>
    node::forward_link_iterator selectBestLink(graph_type::node_index index, 
            const cost_predicate<LABEL_PREDICATE>& labeler, 
            const BOUNDARY_TEST& bounder, 
            const Point2Type& entryPoint);
    
    /**
     @brief Generate node with @a data in the graph, and place it in the tree
     @param data what is stored in the node
     @return index of newly created node
     */
    graph_type::node_index createNode(const NodeData& data);
    /**
     @brief Remove node with @a index from tree and graph
     @param index identifier of node to remove
     */
    void destroyNode(graph_type::node_index index);
    
    graph_type m_graph;
    tree_type m_tree;
    
};

/**
 @brief A convenience class for turning a boundary crossing test
 from a spacial container of boundary segments into a simple functor
 for use with SpacialGraph's optimize function
 @param SPACIAL_CONTAINER the type of container that holds the boundaries, 
 can be boxlist, rtree, local rtree, or quadtree of Segment2Type
 */
template <typename SPACIAL_CONTAINER>
class basic_boundary_test {
public:
    /// the type of container this functor uses
    typedef SPACIAL_CONTAINER container_type;
    /// construct a functor with a container reference
    basic_boundary_test(const container_type& container);
    ///@return true if testSegment avoids all boundaries, else false
    bool operator ()(const Segment2Type& testSegment) const;
private:
    const container_type& m_container;
};

}

#endif	/* SPACIAL_GRAPH_DECL_H */

