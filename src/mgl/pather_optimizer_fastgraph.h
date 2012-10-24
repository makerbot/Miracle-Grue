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
#include "predicate.h"
#include "basic_boxlist.h"
#include "intersection_index.h"
#include "Exception.h"
#include "configuration.h"
#include "labeled_path.h"
#include "mgl.h"
#include <iostream>
#include <list>

namespace mgl {

class GraphException : public Exception {
public:
    template <typename T>
    GraphException(const T& arg) : Exception(arg) {}
};

class PathingException : public Exception {
public:
    template <typename T>
    PathingException(const T& arg) : Exception(arg) {}
};

/* 
 Implementation of interface put forth in abstract_optimizer
   One of the several different options for optimizing paths. 
    Usage:
    pather_optimizer_fastgraph optimus(grueCfg);
    optimus.addBoundary(loop1);
    optimus.addBoundary(loop2); //all boundaries must be added first
    optimus.addPaths(listOfLoops, somelabel);
    optimus.addPaths(listOfPaths, somedifferentlabel);
    optimus.optimize(destinationlistOfLabeledPaths);    //this is destructive
 */
class pather_optimizer_fastgraph : public abstract_optimizer {
public:
    pather_optimizer_fastgraph(const GrueConfig& grueConf)
            : grueCfg(grueConf),  
            historyPoint(std::numeric_limits<Scalar>::min(), 
            std::numeric_limits<Scalar>::min()) {}
    //addPath builds up the correct interior graph (the correct bucket)
    void addPath(const OpenPath& path, const PathLabel& label);
    void addPath(const Loop& loop, const PathLabel& label);
    //Do not cross this path!
    void addBoundary(const OpenPath& path);
    //Creates a new bucket. Things inside of this loop will be added to this bucket
	void addBoundary(const Loop& loop);
    void clearBoundaries();
	void clearPaths();
    //debugging: Make a nice svg of this graph
    void repr_svg(std::ostream& out);
protected:
    void optimizeInternal(LabeledOpenPaths& labeledpaths);
private:
    typedef std::list<LabeledOpenPaths> multipath_type;
    
    class bucket;
    typedef std::list<bucket> bucket_list;
    //format is void foo(output, [input]);
    //pick the best bucket to start optimizing, optimize it, repeat until done
    void optimize1(multipath_type& output, Point2Type& entryPoint);
    //optimize the bucket selected by above
    void optimize1Inner(LabeledOpenPaths& labeledpaths, bucket_list::iterator input, 
            Point2Type& entryPoint);
    //Run v-opt on results of above function
    bool optimize2(LabeledOpenPaths& labeledopenpaths, 
            LabeledOpenPaths& intermediate);
    class Cost : public PathLabel {
    public:
        Cost(const PathLabel& label = PathLabel(), 
                Scalar distance = 0, 
                Point2Type normal = Point2Type()) 
                : PathLabel(label), m_distance(distance), m_normal(normal) {}
        Cost(const Cost& other) : PathLabel(other), 
                m_distance(other.m_distance), m_normal(other.m_normal) {}
        Scalar distance() const { return m_distance; }
        const Point2Type& normal() const { return m_normal; }
    private:
        Scalar m_distance;
        Point2Type m_normal;
    };
    class NodeData {
    public:
        NodeData(Point2Type position, 
                PathLabel label, 
                bool entry) 
                : m_position(position), 
                m_label(label), 
                m_isentry(entry) {}
        NodeData() : m_isentry(false) {}
        const Point2Type& getPosition() const { return m_position; }
        const PathLabel& getLabel() const { return m_label; }
        bool isEntry() const { return m_isentry; }
    private:
        Point2Type m_position;
        PathLabel m_label;
        bool m_isentry;
    };
    
    typedef basic_boxlist<Segment2Type> boundary_container;
    typedef topo::simple_graph<NodeData, Cost> graph_type;
    typedef graph_type::node node;
    typedef graph_type::node_index node_index;
    typedef std::pair<node_index, Scalar> probe_link_type;
    
    class bucket {
    public:
        
        typedef Loop::const_finite_cw_iterator edge_iterator;
        
        bucket(Point2Type testPoint = Point2Type());
        bucket(const Loop& loop);
        bool contains(Point2Type point) const;
        bool contains(const bucket& other) const;
        void insertBoundary(const Loop& loop);
        void insertNoCross(const Loop& loop);
        bucket& select(Point2Type point);
        void optimize(LabeledOpenPaths& output, Point2Type& entryPoint, 
                const GrueConfig& grueConf);
        void swap(bucket& other);
        edge_iterator edgeBegin() const;
        edge_iterator edgeEnd() const;
        
        static bucket_list::iterator pickBestChild(
                bucket_list::iterator begin, 
                bucket_list::iterator end, 
                const Point2Type& entryPoint);
        
        boundary_container m_bounds;
        boundary_container m_noCrossing;
        AABBox m_limits;
        graph_type m_graph;
        Point2Type m_testPoint;
        Point2Type m_infinitePoint;
        bool m_empty;
        bucket_list m_children;
        Loop m_loop;
    private:
        void insertBoundary(const Segment2Type& line);
        void insertNoCross(const Segment2Type& line);
        void updateInfinity();
    };
    
    typedef graph_type::forward_node_iterator node_iterator;
    
    class AbstractLabelComparator : public abstract_predicate<PathLabel> {
    public:
        typedef abstract_predicate<PathLabel>::value_type value_type;
        AbstractLabelComparator(const GrueConfig& grueConf) 
                : grueCfg(grueConf) {}
    protected:
        const GrueConfig& grueCfg;
    };
    class LabelTypeComparator : public AbstractLabelComparator {
    public:
        LabelTypeComparator(const GrueConfig& grueConf) 
                : AbstractLabelComparator(grueConf) {}
        typedef AbstractLabelComparator::value_type value_type;
        int compare(const value_type& lhs, const value_type& rhs) const;
    };
    class LabelPriorityComparator : public AbstractLabelComparator {
    public:
        LabelPriorityComparator(const GrueConfig& grueConf) 
                : AbstractLabelComparator(grueConf) {}
        typedef AbstractLabelComparator::value_type value_type;
        int compare(const value_type& lhs, const value_type& rhs) const;
    };
    typedef composite_predicate<PathLabel, LabelTypeComparator, LabelPriorityComparator> 
            LabelComparator;
    class NodeComparator : public abstract_predicate<node> {
    public:
        NodeComparator(const GrueConfig& grueConf) 
                : m_labelCompare(LabelTypeComparator(grueConf), 
                        LabelPriorityComparator(grueConf)) {}
        typedef abstract_predicate<node>::value_type value_type;
        int compare(const value_type& lhs, const value_type& rhs) const;
    protected:
        LabelComparator m_labelCompare;
    };
    class NodeConnectionComparator : public abstract_predicate<node::connection> {
    public:
        NodeConnectionComparator(const GrueConfig& grueConf, 
                Point2Type unit = Point2Type()) 
                : m_nodeCompare(grueConf), m_unit(unit) {}
        typedef abstract_predicate<node::connection>::value_type value_type;
        int compare(const value_type& lhs, const value_type& rhs) const;
    protected:
        NodeComparator m_nodeCompare;
        Point2Type m_unit;
    };
    
    typedef NodeComparator LinkBuildingSortComparator;
    typedef LabelTypeComparator LinkBuildingConnectionCutoffComparator;
    
    
    class entry_iterator {
    public:
        
        friend class pather_optimizer_fastgraph;
        
        entry_iterator() {}
            
        entry_iterator& operator ++(); //pre
        entry_iterator operator ++(int); //post
        node& operator *();
        node* operator ->() { return &**this; }
        bool operator ==(const entry_iterator& other) const;
        bool operator !=(const entry_iterator& other) const
                { return !(*this==other); }
        
    private:
        explicit entry_iterator(node_iterator base, node_iterator end) 
                : m_base(base), m_end(end) {}
        node_iterator m_base;
        node_iterator m_end;
    };
    
    static entry_iterator entryBegin(graph_type& graph);
    static entry_iterator entryEnd(graph_type& graph);
    
    class probeCompare {
    public:
        probeCompare(node_index from, graph_type& basis, 
                const GrueConfig& grueConf) 
                : m_from(from), m_graph(basis), 
                m_nodeCompare(grueConf) {}
        bool operator ()(const probe_link_type& lhs, 
                const probe_link_type& rhs);
    private:
        node_index m_from;
        graph_type& m_graph;
        LinkBuildingSortComparator m_nodeCompare;
    };
    
    static node::forward_link_iterator bestLink(node& from, graph_type& graph, 
            boundary_container& boundaries, const GrueConfig& grueConf, 
            Point2Type unit = Point2Type()); //can return node::forwardEnd()
    static void buildLinks(node& from, graph_type& graph, 
            boundary_container& boundaries, const GrueConfig& grueConf);
    
    static bool comparePathLists(const LabeledOpenPaths& lhs, 
            const LabeledOpenPaths& rhs);
    static size_t countIntersections(Segment2Type& line, 
            const boundary_container& boundContainer);
    
    class connectionComparator {
    public:
        connectionComparator(Point2Type unit = Point2Type()) : m_unit(unit) {}
        bool operator ()(const node::connection& lhs, 
                const node::connection& rhs) const;
    private:
        Point2Type m_unit;
    };
    
    class nodeComparator {
    public:
        nodeComparator(const GrueConfig& grueConf, graph_type& graph, Point2Type point = 
                Point2Type(std::numeric_limits<Scalar>::min(), 
                std::numeric_limits<Scalar>::min()))
                : m_graph(graph), m_position(point), 
                m_nodeCompare(grueConf) {}
        bool operator ()(const node& lhs, const node& rhs) const;
        bool operator ()(node_index lhs, node_index rhs) const;
    private:
        graph_type& m_graph;
        Point2Type m_position;
        NodeComparator m_nodeCompare;
    };
    
    static bool crossesBounds(const Segment2Type& line, 
            boundary_container& boundaries);
    
    static void smartAppendPoint(Point2Type point, PathLabel label, 
            LabeledOpenPaths& labeledpaths, LabeledOpenPath& path, 
            Point2Type& entryPoint);
    static void smartAppendPath(LabeledOpenPaths& labeledpaths, LabeledOpenPath& path);
    
    Scalar splitPaths(multipath_type& destionation, const LabeledOpenPaths& source);
    bucket_list::iterator pickBucket(Point2Type point);
    
    
    const GrueConfig& grueCfg;
    
    boundary_container m_boundaries;
    bucket_list buckets;
    graph_type m_graph;
    AABBox boundaryLimits;
    Point2Type historyPoint;
    
};

}

#endif	/* MGL_PATHER_OPTIMIZER_FASTGRAPH_H */

