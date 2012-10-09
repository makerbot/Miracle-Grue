/* 
 * File:   pather_optimizer_fastgraph.h
 * Author: Dev
 *
 * Created on October 9, 2012, 11:28 AM
 */

#ifndef MGL_PATHER_OPTIMIZER_FASTGRAPH_H
#define	MGL_PATHER_OPTIMIZER_FASTGRAPH_H

#include "pather_optimizer.h"
#include "simple_topology.h"
#include "basic_boxlist.h"

namespace mgl {

class pather_optimizer_fastgraph : public abstract_optimizer {
public:
    void addPath(const OpenPath& path, const PathLabel& label);
    void addPath(const Loop& loop, const PathLabel& label);
    void addBoundary(const OpenPath& path);
	void addBoundary(const Loop& loop);
    void clearBoundaries();
	void clearPaths();
protected:
    void optimizeInternal(LabeledOpenPaths& labeledpaths);
private:
    class Cost : public PathLabel {
    public:
        Cost(const PathLabel& label = PathLabel(), 
                Scalar distance = 0, 
                PointType normal = PointType()) 
                : m_distance(distance), m_normal(normal) {}
        Scalar distance() const { return m_distance; }
        const PointType& normal() const { return m_normal; }
    private:
        const Scalar m_distance;
        const PointType m_normal;
    };
    class NodeData {
    public:
        NodeData(PointType position = PointType(), 
                int priority = 0, 
                bool entry = false) 
                : m_position(position), 
                m_priority(priority), 
                m_isentry(entry) {}
        const PointType& getPosition() const { return m_position; }
        int getPriority() const { return m_priority; }
        bool isEntry() const { return m_isentry; }
        PointType m_position;
        int m_priority;
        bool m_isentry;
    };
    typedef basic_boxlist<libthing::LineSegment2> boundary_container;
    typedef topo::simple_graph<NodeData, Cost> graph_type;
    typedef graph_type::node node;
    
    typedef graph_type::forward_node_iterator node_iterator;
    
    class entry_iterator {
    public:
        entry_iterator() {}
            
        entry_iterator& operator ++(); //pre
        entry_iterator operator ++(int); //post
        node& operator *();
        node* operator ->() { return &**this; }
        bool operator ==(const entry_iterator& other) const;
        
    private:
        explicit entry_iterator(node_iterator base, node_iterator end) 
                : m_base(base), m_end(end) {}
        node_iterator m_base;
        node_iterator m_end;
    };
    
    boundary_container boundaries;
    graph_type graph;
    
};

}

#endif	/* MGL_PATHER_OPTIMIZER_FASTGRAPH_H */

