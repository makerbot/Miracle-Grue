/**
 @file pather_optimizer_fastgraph.h
 @version 0
 @author Filipp Gelman <filipp@makerbot.com>
 @summary An incredibly convoluted recursive tree hierarchy based approach to 
 approximately solving the a pseudo-traveling-salesman problem 
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

/** 
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
            historyPoint(grueConf.get_startingX(), grueConf.get_startingY()) {}
    //addPath builds up the correct interior graph (the correct bucket)
    void addPath(const OpenPath& path, const PathLabel& label);
    void addPath(const Loop& loop, const PathLabel& label);
    //Do not cross this path! TODO: Not supported by buckets
    void addBoundary(const OpenPath& path);
    //Creates a new bucket. Things inside of this loop will be added to this bucket
	void addBoundary(const Loop& loop);
    void clearBoundaries();
	void clearPaths();
    //debugging: Make a nice svg of this graph
    void repr_svg(std::ostream& out);
    
    
    // HACK FOR UNIT TESTS TO TOUCH PRIVATES
#ifdef FASTGRAPH_FRIENDS_LIST
    #define FASTGRAPH_PRIVATE public
#else
    #define FASTGRAPH_PRIVATE private
#endif
    // END HACK
protected:
    /**
     @brief Starting point for all optimization
     Whenever you call any_type_of_optimizer.optimize(anyContainer), inernally 
     it will call optimizeInternal then copy results out of @a labeledpaths
     @param labeledpaths where result is placed
     This function is DESTRUCTIVE. 
     */
    void optimizeInternal(LabeledOpenPaths& labeledpaths);
FASTGRAPH_PRIVATE:

#undef FASTGRAPH_PRIVATE

    typedef std::list<LabeledOpenPaths> multipath_type;
    
    class bucket;
    typedef std::list<bucket> bucket_list;
    //format is void foo(output, [input]);
    //pick the best bucket to start optimizing, optimize it, repeat until done
    /**
     @brief optimize the best bucket then erase it. Repeat until empty
     @param output a list of lists of labeledopenpaths. Each entry in the top 
     list represents the paths of a single top level bucket. Each entry in the 
     bottom list represents a single path to traverse.
     @param entryPoint A Point2Type reference to the most recently traversed 
     point. This is persistent for the entire slice. It is used to determine 
     which object to optimize next, and which node to optimize within objects. 
     All optimizations will update this point. It is also what allows us to 
     start a new layer where the previous layer ended.
     */
    void optimizeBuckets(multipath_type& output, Point2Type& entryPoint);
    ///Run v-opt on results of above function, not used currently
    bool optimizeIterative(LabeledOpenPaths& labeledopenpaths, 
            LabeledOpenPaths& intermediate);
    /**
     @brief a PathLabel that also stores precomputed distance and normal.
     */
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
    /**
     @brief Data to store at each node in our graphs
     @param position where in the layer this node is located
     @param label the label of the path to which this node belongs
     @param entry is this node a valid entry point
     */
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
    
    class LoopHierarchyBaseComparator;
    
    /**
     @brief a description of the extents of a region and all regions 
     contained within it.
     
     An outline loop defines a bucket. If there is a hole loop inside this 
     outline loop, the outline loop will contain the hole loop.
     
     Each bucket contains its extents for testing containment, but also
     contains the borders of itself and all its children for not crossing 
     when making connections.
     In addition, each bucket contains a loop hierarchy and a graph of things 
     to optimize.
     
     TODO: Replace both bucket and loop hierarchy with specializations of 
     Containment_tree
     
     */
    class bucket {
    public:
        
        typedef Loop::const_finite_cw_iterator edge_iterator;
        
        class LoopHierarchy;
        typedef std::list<LoopHierarchy> hierarchy_list;
        
        /**
         @brief a LoopHierarchy is a bucket for insets. All same things apply.
         LoopHierarchies have code that will traverse loops always 
         innermost to outermost, but will also group adjacent loops 
         together.
         This is simply a depth-first tree traversal with head or tail 
         recursion dictated by a comparator.
         */
        class LoopHierarchy {
        public:
            /// Construct a hierarchy parent
            LoopHierarchy();
            /// construct a normal hierarchy node
            LoopHierarchy(const LabeledLoop& loop);
            /// construct a normal hierarchy node
            LoopHierarchy(const Loop& loop, const PathLabel& label);
            typedef std::vector<graph_type::node_index> entryIndexVector;
            /**
             @brief convenience function, invokes insert(loop, label);
             */
            LoopHierarchy& insert(const LabeledLoop& loop);
            /**
             @brief insert this loop and its label into the hierarchy
             @param loop the loop to insert. Must not intersect any loops 
             already in the hierarchy.
             @param label the label for @a loop
             @return reference to the element that contains the loop you 
             are trying to insert
             
             This will properly handle the case where @a loop contains this 
             hierarchy or some of its children. It will may also cause 
             a complete reorganization of the tree, invalidating all old 
             references.
             
             LoopHierarchy will NOT handle non-manifold or intersecting loops.
             */
            LoopHierarchy& insert(const Loop& loop, const PathLabel& label);
            /// test if we contain the point
            bool contains(Point2Type point) const;
            /// test if we contain other
            bool contains(const LoopHierarchy& other) const;
            /**
             @brief the entry point for optimizing a loop hierarchy
             Selects the best valid child and optimizes it until no more remain
             */
            void optimize(LabeledOpenPaths& output, Point2Type& entryPoint, 
                    graph_type& graph, boundary_container& bounds, 
                    const GrueConfig& grueConf);
            /**
             @brief called by a parent hierarchy on its valid children
             @param from dummy unused parameter to differentiate overloads
             */
            void optimize(LabeledOpenPaths& output, Point2Type& entryPoint, 
                    graph_type& graph, graph_type::node_index& from, 
                    boundary_container& bounds, 
                    const GrueConfig& grueConf);
            void swap(LoopHierarchy& other);
            void repr(std::ostream& out, size_t level = 0);
            PathLabel m_label;
            hierarchy_list m_children;
            Point2Type m_testPoint;
            Loop m_loop;
            
        private:
            bool isValid() const;
            hierarchy_list::iterator bestChild(
                    const LoopHierarchyBaseComparator& compare);
            LoopHierarchy& insert(LoopHierarchy& constructed);
        };
        
        /// construct a bucket parent
        bucket(Point2Type testPoint = Point2Type());
        /// construct a normal bucket
        bucket(const Loop& loop);
        /// test for containment
        bool contains(Point2Type point) const;
        /// test for containment
        bool contains(const bucket& other) const;
        /// invokes insertBucket with a boundary constructed from the loop
        void insertBoundary(const Loop& loop);
        /// things optimized in this bucket should not cross this loop
        void insertNoCross(const Loop& loop);
        /**
         @brief insert the bucket into this one. 
         
         As in LoopHierarchy, it handles cases where constructed contains 
         this bucket or some of its children. Invalidates old references, 
         and does not handle non-manifoldness
         
         @param constructed the bucket you try to insert into this one. This 
         will be invalidated after insertion.
         */
        void insertBucket(bucket& constructed);
        /**
         @brief select the deepest child that contains point, or this bucket. 
         @param point the point to test
         @return the deepest child that contains point, or this bucket if no 
         child contains
         */
        bucket& select(Point2Type point);
        /**
         @brief optimize any loop hierarchies in this bucket followed by 
         all things stored in this bucket's graph
         */
        void optimize(LabeledOpenPaths& output, Point2Type& entryPoint, 
                const GrueConfig& grueConf);
        /// Fast swap implementation, no memory allocation or deallocation
        void swap(bucket& other);
        /// use for iterating over the extents of this bucket
        edge_iterator edgeBegin() const;
        /// use for iterating over the extents of this bucket
        edge_iterator edgeEnd() const;
        
        /**
         @brief Convenience function that invokes pickBestChild below, but 
         omits bestDistanceOutput
         */
        static bucket_list::iterator pickBestChild(
                bucket_list::iterator begin, 
                bucket_list::iterator end, 
                const Point2Type& entryPoint);
        /**
         @brief Based on @a entryPoint, pick from the range [begin,end] the 
         best bucket for optimizing.
         @param begin start of the range to consider
         @param end past-the-end of the range to consider
         @param entryPoint find bucket nearest to this point
         @param bestDistanceOutput distance from @a entryPoint to the 
         nearest point in the returned bucket
         @return iterator to the nearest bucket or @a end
         */
        static bucket_list::iterator pickBestChild(
                bucket_list::iterator begin, 
                bucket_list::iterator end, 
                const Point2Type& entryPoint, 
                Scalar& bestDistanceOutput);
        
        boundary_container m_noCrossing;
        graph_type m_graph;
        Point2Type m_testPoint;
        bool m_empty;
        bucket_list m_children;
        Loop m_loop;
        LoopHierarchy m_hierarchy;
    private:
        void insertNoCross(const Segment2Type& line);
    };
    
    
    typedef graph_type::forward_node_iterator node_iterator;
    
    
    //below here are many many comparators, should be cleaned up.
    
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
    
    class LoopHierarchyBaseComparator : public abstract_predicate<bucket::LoopHierarchy> {
    public:
        typedef abstract_predicate<bucket::LoopHierarchy>::value_type value_type;
        LoopHierarchyBaseComparator(Point2Type& entryPoint, 
                const graph_type& graph, 
                const GrueConfig& grueConf) 
                : m_entryPoint(entryPoint), 
                m_graph(graph), 
                m_compare(grueConf) {}
        int compare(const value_type& lhs, const value_type& rhs) const;
    protected:
        Point2Type& m_entryPoint;
        const graph_type& m_graph;
        LabelPriorityComparator m_compare;
    };
    
    class LoopHierarchyStrictComparator : public LoopHierarchyBaseComparator {
    public:
        typedef LoopHierarchyBaseComparator::value_type value_type;
        LoopHierarchyStrictComparator(Point2Type& entryPoint, 
                const graph_type& graph, 
                const GrueConfig& grueConf) 
                : LoopHierarchyBaseComparator(entryPoint, graph, grueConf) {}
        int compare(const value_type& lhs, const value_type& rhs) const;
    };
    
    /**
     @brief A filter iterator that allows convenient access to only those 
     nodes in a graph that are entry points.
     */
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
    
    /// get the entry_iterator for graph
    static entry_iterator entryBegin(graph_type& graph);
    /// get the entry_iterator for graph
    static entry_iterator entryEnd(graph_type& graph);
    
    /**
     @brief adapt a predicate for use when building up links
     */
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
    
    /**
     @brief find or construct the best outgoing link from a node
     @param from a node reference
     @param graph the owner of @a from
     @param boundaries that which should not be crossed
     @param grueConf a const GrueConfig reference
     @param unit optionally provide a unit normal
     @return an iterator to the best link to follow, or from.forwardEnd() 
     if no such links can be constructed
     Considers all entry points in @a graph when building new links
     */
    static node::forward_link_iterator bestLink(node& from, graph_type& graph, 
            boundary_container& boundaries, const GrueConfig& grueConf, 
            Point2Type unit = Point2Type());
    /**
     @brief find or construct the best outgoing link from a node
     @param from a node reference
     @param graph the owner of @a from
     @param boundaries that which should not be crossed
     @param entries a vector of node_indexes to consider
     @param grueConf a const GrueConfig reference
     @param unit optionally provide a unit normal
     @return an iterator to the best link to follow, or from.forwardEnd() 
     if no such links can be constructed
     This operates just like the function above, but allows you to limit
     which entry points are considered when making links
     Considers only entry points in @a entries when building new links
     */
    static node::forward_link_iterator bestLink(node& from, graph_type& graph, 
            boundary_container& boundaries, 
            bucket::LoopHierarchy::entryIndexVector& entries, 
            const GrueConfig& grueConf, 
            Point2Type unit = Point2Type());
    /**
     @brief construct outgoing connection links from a node
     @param from node from which to construct links
     @param graph the owner of @a from
     @param boundaries that should not be crossed
     @param grueConf a const GrueConfig reference
     This function considers all valid entry points in @a graph, 
     and constructs the best connection from node @a from to one entry point
     if one can be constructed.
     */
    static void buildLinks(node& from, graph_type& graph, 
            boundary_container& boundaries, const GrueConfig& grueConf);
    /**
     @brief construct outgoing connection links from a node
     @param from node from which to construct links
     @param graph the owner of @a from
     @param boundaries that should not be crossed
     @param entries a vector of node_indexes to consider
     @param grueConf a const GrueConfig reference
     Like the above function, but considers only entries in @a entries.
     */
    static void buildLinks(node& from, graph_type& graph,
            boundary_container& boundaries, 
            bucket::LoopHierarchy::entryIndexVector& entries, 
            const GrueConfig& grueConf);
    /**
     @brief Select the best connection from candidates generated by the above.
     @param from node from which to construct links
     @param graph the owner of @a from
     @param boundaries that should not be crossed
     @param sortedProbes sorted vector of candidates from a buildLinks above
     @param grueConf a const GrueConfig reference
     The above overloads of buildLinks generate many connection candidates 
     and store them in a sorted vector. They then use this function to 
     test each candidate and build a link from it if it is valid
     */
    static void buildLinks(node& from, graph_type& graph, 
            boundary_container& boundaries, 
            std::vector<probe_link_type>& sortedProbes, 
            const GrueConfig& grueConf);
    
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
    
    /**
     @brief convenience function for testing if @a line crosses any 
     element of @boundaries
     @param line the line to test
     @param boundaries the spacial index to test against
     @return true if crossings exist, false otherwise
     */
    static bool crossesBounds(const Segment2Type& line, 
            boundary_container& boundaries);
    
    static void smartAppendPoint(Point2Type point, PathLabel label, 
            LabeledOpenPaths& labeledpaths, LabeledOpenPath& path, 
            Point2Type& entryPoint);
    static void smartAppendPath(LabeledOpenPaths& labeledpaths, LabeledOpenPath& path);
    
    Scalar splitPaths(multipath_type& destionation, const LabeledOpenPaths& source);
    bucket_list::iterator pickBucket(Point2Type point);
    
    
    const GrueConfig& grueCfg;
    
    bucket_list buckets;
    Point2Type historyPoint;
    
};

}

#endif	/* MGL_PATHER_OPTIMIZER_FASTGRAPH_H */

