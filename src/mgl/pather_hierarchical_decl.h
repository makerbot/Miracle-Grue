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
#include "spacial_graph_decl.h"

namespace mgl {

class pather_hierarchical : public abstract_optimizer{
public:
protected:
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
};

}

#endif	/* MGL_PATHER_HIERARCHICAL_DECL_H */

