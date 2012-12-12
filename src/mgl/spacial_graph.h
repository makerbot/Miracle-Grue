/* 
 * File:   spacial_graph.h
 * Author: Dev
 *
 * Created on December 12, 2012, 9:17 AM
 */

#ifndef MGL_SPACIAL_GRAPH_H
#define	MGL_SPACIAL_GRAPH_H

#include "simple_topology.h"
#include "basic_local_rtree.h"
#include "labeled_path.h"
#include "loop_path.h"

namespace mgl {

class SpacialGraph {
public:
    class NodeData;
    class CostData;
    typedef topo::simple_graph<NodeData, CostData> graph_type;
    typedef std::pair<graph_type*, graph_type::node_index> graph_node_reference;
    typedef basic_local_rtree<graph_node_reference> tree_type;
    typedef graph_type::node node;
    
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
    
    void insertPath(const OpenPath& path, const PathLabel& label);
    void insertPath(const Loop& loop, const PathLabel& label);
    template <typename PATH_TYPE>
    void insertPath(const basic_labeled_path<PATH_TYPE>& labeledPath) {
        insertPath(labeledPath.myPath, labeledPath.myPath);
    }
    template <typename COLLECTION>
    void insertPaths(const COLLECTION& collection) {
        for(typename COLLECTION::const_iterator iter = collection.begin(); 
                iter != collection.end(); 
                ++iter) {
            insertPath(*iter);
        }
    }
    template <typename COLLECTION>
    void insertPaths(const COLLECTION& collection, const PathLabel& label) {
        for(typename COLLECTION::const_iterator iter = collection.begin(); 
                iter != collection.end(); 
                ++iter) {
            insertPath(*iter, label);
        }
    }
    
    entry_iterator entryBegin();
    entry_iterator entryEnd();
    
    void repr(std::ostream& out);
    void repr_svg(std::ostream& out);
    
private:
    
    graph_type::node_index createNode(const NodeData& data);
    void destroyNode(graph_type::node_index index);
    
    graph_type m_graph;
    tree_type m_tree;
    
};

}

#endif	/* MGL_SPACIAL_GRAPH_H */

