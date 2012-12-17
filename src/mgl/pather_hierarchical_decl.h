/* 
 * File:   pather_hierarchical_decl.h
 * Author: Dev
 *
 * Created on December 14, 2012, 12:06 PM
 */

#ifndef MGL_PATHER_HIERARCHICAL_DECL_H
#define	MGL_PATHER_HIERARCHICAL_DECL_H

#include "ContainmentTree.h"
#include "spacial_data.h"
#include "pather_optimizer.h"
#include "spacial_graph.h"

namespace mgl {

class HierarchyException : public Exception {
public:
    template <typename T>
    HierarchyException(const T& arg) : Exception(arg) {}
};

class pather_hierarchical : public abstract_optimizer{
public:
    void addPath(const OpenPath& path, const PathLabel& label);
    void addPath(const Loop& loop, const PathLabel& label);
    void addBoundary(const OpenPath& path);
    void addBoundary(const Loop& loop);
    void clearPaths();
    void clearBoundaries();
protected:
    void optimizeInternal(LabeledOpenPaths& result);
private:
    /**
     @brief InsetTree describes the lowest type of node in the hierarchy. 
     
     A node in the inset tree consists of the loop and label describing 
     that particular inset, along with a spacial graph that contains 
     any non-inset objects that fall inside this inset, but not inside 
     any of its children. 
     
     Insideness considered geometrically only. For normal external insets, 
     the innermost insets will be geometrically inside the the outermost ones. 
     For insets of holes, the innermost insets will be geometrically 
     outside of outermost ones. This must be considered when traversing 
     the inset tree.
     
     Currently, this is the lowest element in the hierarchy, but this will not 
     always be so. In the future, I plan to keep track of solid and exposed 
     regions of a layer. These will be loops that are neither extruded nor 
     boundaries (tentatively) that represent things like exposed roofs
     and allow them to be pathed contiguously. These hierarchies will 
     be eventually contained in each inset tree node.
     */
    class InsetTree : public basic_containment_tree<InsetTree> {
    public:
        /// a convenience typedef to initialize the CRTP parent
        typedef basic_containment_tree<InsetTree> parent_class;
        
        /// construct a root inset tree
        InsetTree();
        /// construct a valid node in the inset tree
        InsetTree(const Loop& loop, const PathLabel& label);
        /**
         @brief insert a labeled path into the graph contained at this node
         @param path the path to insert
         @param label the label of the path
         
         This will store the path with its label into the SpacialGraph 
         object at this inset tree node. SpacialGraph handles 
         the specifics of maintaining its graph and r-tree, so we simply 
         forward the data to it.
         */
        void insert(const OpenPath& path, const PathLabel& label);
        /**
         @brief insert a labeled loop into the graph contained at this node
         @param loop the path to insert
         @param label the label of the loop
         
         DO NOT USE FOR INSETS! For inserting insets, instead call
         insert(InsetTree(loop, label));
         
         This will store the loop with its label into the SpacialGraph 
         object at this inset tree node. SpacialGraph handles 
         the specifics of maintaining its graph and r-tree, so we simply 
         forward the data to it.
         */
        void insert(const Loop& loop, const PathLabel& label);
        /**
         @brief Insert an inset into the InsetTree.
         @param other the node to insert. Will be destroyed by the process.
         @return reference to the node that holds the data from @a other
         
         This function forwards to basic_containment_tree's implementation. 
         I don't know why I need to do this since we're already a subclass.
         */
        InsetTree& insert(InsetTree& other);
        /**
         @brief Swap the contents of this object with @a other
         @param other The object with which to swap data.
         
         This function will swap the labels, invoke SpacialGraph's optimized 
         swap implementation, then call basic_containment_tree's swap
         to transfer over all its data.
         
         No new memory is allocated from this call. Biggest cost is updating 
         the pointers inside SpacialGraph
         */
        void swap(InsetTree& other);
        /**
         @brief Traverse this tree and its children, prioritizing according 
         to @a LABEL_COMPARE and respecting @a BOUNDARY_TEST, and optimizing 
         all contained graphs accordingly.
         @param LABEL_COMPARE A predicate on PathLabel
         @param BOUNDARY_TEST A functor on Segment2Type. 
         BOUNDARY_TEST::operator ()(const Segment2Type&) const will return 
         true if the line represents a valid extrusion path (i.e. does not 
         cross any boundaries), and false otherwise
         @param result Here will be placed the result of the traversal
         @param entryPoint Indicates where best to start traversal. After 
         function returns, holds the position of the last traversed point. 
         @param labeler instance of LABEL_COMPARE that dictates how best to 
         order labels
         @param bounder instance of BOUNDARY_TEST that dictates where 
         we can make new connections
         
         This function is destructive! It will cause all children to be 
         erased, and all graphs to be emptied.
         */
        template <typename LABEL_COMPARE, typename BOUNDARY_TEST>
        void traverse(LabeledOpenPaths& result, Point2Type& entryPoint, 
                const LABEL_COMPARE& labeler = LABEL_COMPARE(), 
                const BOUNDARY_TEST& bounder = BOUNDARY_TEST());
    private:
        /**
         @brief Select the best choice from my children that respects
         priorities imposed by labeler and restrictions imposed by 
         bounder.
         @param LABEL_COMPARE
         @param BOUNDARY_TEST
         @param entryPoint point used for distance testing
         @param labeler instance of object to compare labels
         @param bounder instance of object to test for new connections
         */
        template <typename LABEL_COMPARE, typename BOUNDARY_TEST>
        parent_class::iterator selectBestChild(Point2Type& entryPoint, 
                const LABEL_COMPARE& labeler, 
                const BOUNDARY_TEST& bounder);
        /**
         @brief Traverse over the data contained inside this node only!
         This is called within traverse. Traverse decides in what order 
         to do the children, and at which point to traverse the current node. 
         This function traverses the current node.
         
         All parameters play the same role as in InsetTree::traverse(...)
         
         @param LABEL_COMPARE
         @param BOUNDARY_TEST
         @param result
         @param entryPoint
         @param labeler
         @param bounder
         */
        template <typename LABEL_COMPARE, typename BOUNDARY_TEST>
        void traverseInternal(LabeledOpenPaths& result, 
                Point2Type& entryPoint, 
                const LABEL_COMPARE& labeler, 
                const BOUNDARY_TEST& bounder);
        //loop is stored by containment tree
        PathLabel m_label;
        SpacialGraph m_graph;
    };
    /**
     @brief OutlineTree describes the highest type of node in the hierarchy.
     
     As InsetTree describes a hierarchy of insets, OutlineTree describes 
     a hierarchy of outlines. This allows us to both group sibling 
     outlines and their contents together, and minimize the number of 
     boundaries that need to be considered for the optimization of 
     any single item.
     
     Topologically, these are laid out exactly as InsetTree. Each node in the 
     OutlineTree contains a spacial data structure of boundaries, 
     an InsetTree, and a SpacialGraph of items to be optimized separately 
     from the insets.
     */
    class OutlineTree : public basic_containment_tree<OutlineTree> {
    public:
        /// a convenience typedef to initialize the CRTP parent
        typedef basic_containment_tree<OutlineTree> parent_class;
        
        /// construct a root outline tree
        OutlineTree();
        /// construct a valid node in the outline tree
        OutlineTree(const Loop& loop);
        /**
         @brief insert a labeled path into the graph contained at this node
         @param path the path to insert
         @param label the label of the path
         
         This will store the path with its label into the SpacialGraph 
         object at this tree node, or the correct InsetTree node depending on 
         the label. SpacialGraph handles the specifics of maintaining its 
         graph and r-tree, so we simply forward the data to it.
         */
        void insert(const OpenPath& path, const PathLabel& label);
        /**
         @brief insert a labeled loop into the graph contained at this node
         @param loop the path to insert
         @param label the label of the loop
         
         DO NOT USE FOR OUTLINES! For inserting outlines, instead call
         insert(OutlineTree(loop));
         
         This will store the loop with its label into the SpacialGraph 
         object at this tree node, or the correct InsetTree node depending on 
         the label. SpacialGraph handles the specifics of maintaining its 
         graph and r-tree, so we simply forward the data to it.
         If label.isInset() returns true, we will add an entry to the 
         InsetTree at this node instead of adding it to a graph.
         */
        void insert(const Loop& loop, const PathLabel& label);
        /**
         Why do I need to write this? Is CRTP broken?
         */
        OutlineTree& insert(OutlineTree& other);
        /**
         @brief Swap the contents of this object with @a other
         @param other The object with which to swap data.
         
         This function will swap the InsetTrees, invoke SpacialGraph's optimized 
         swap implementation, then call basic_containment_tree's swap
         to transfer over all its data.
         
         No new memory is allocated from this call. Biggest cost is updating 
         the pointers inside SpacialGraph
         */
        void swap(OutlineTree& other);
        /**
         @brief Optimize myself and my children.
         
         In the most optimal order, optimize myself and my children. 
         Select from children based on distance.
         
         @param LABEL_COMPARE the type of object used to prioritize based on 
         labels.
         @param result here will be placed outcome of optimization
         @param entryPoint Indicates from where to start optimizing. 
         When function returns, holds the position of last object to be 
         optimized.
         @param labeler instance of label comparison object. Passed to 
         InsetTree and SpacialGraph.
         
         This function is DESTRUCTIVE! It will cause all my children to 
         be erased and all my data to be emptied.
         */
        template <typename LABEL_COMPARE>
        void traverse(LabeledOpenPaths& result, Point2Type& entryPoint, 
                const LABEL_COMPARE& labeler = LABEL_COMPARE());
    private:
        /**
         @brief based on entryPoint, select best child
         @param entryPoint nearest to this point select child
         @return nearest child or end() if none exist
         
         Simply select the closest child to @a entryPoint based on the 
         distance from it to the child's boundary loop
         */
        iterator selectBestChild(Point2Type& entryPoint);
        /**
         @brief Construct a collection of boundaries based on my and my 
         children's outlines.
         @param SPACIAL_CONTAINER the type of spacial container to be used. 
         Valid examples are rtree, local_rtree, quadtree, boxlist of 
         Segment2Tye.
         @param boundaries a properly initialized instance of spacial 
         container of the type specified. Boundaries will be inserted 
         into this container.
         
         We don't build up a collection of boundaries until we start 
         optimizing the contents of this node in the OutlineTree. 
         Calling this function will build up a collection of boundaries 
         relevant to this node.
         It's unnecessary to consider all boundaries. Looking at this node
         and its children is sufficient.
         */
        template <typename SPACIAL_CONTAINER>
        void constructBoundaries(SPACIAL_CONTAINER& boundaries) const;
        
        
        InsetTree m_insets;
        SpacialGraph m_graph;
        
    };
    
    
    OutlineTree m_root;
    Point2Type m_historyPoint;
};

}

#endif	/* MGL_PATHER_HIERARCHICAL_DECL_H */

