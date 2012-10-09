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
    typedef basic_boxlist<libthing::LineSegment2> boundary_container;
    typedef topo::simple_graph<PointType, Cost> graph_type;
    
    boundary_container boundaries;
    graph_type graph;
    
};

}

#endif	/* MGL_PATHER_OPTIMIZER_FASTGRAPH_H */

