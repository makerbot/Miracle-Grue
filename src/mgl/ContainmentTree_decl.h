/* 
 * File:   ContainmentTree.h
 * Author: FILIPP
 *
 * Created on October 29, 2012, 12:16 PM
 */

#ifndef MGL_CONTAINMENTTREE_DECL_H
#define	MGL_CONTAINMENTTREE_DECL_H

#include <list>
#include "loop_path.h"
#include "mgl.h"

namespace mgl {


/**
 @brief A tree that represents objects containing other objects
 @param DERIVED_T The CRTP class that inherits from this tree
 
 A tree represents a single region (outline loop), and those 
 regions that fall inside this region. A tree contains its children 
 both in the OOP sense and in the spacial sense.
 
 @a DERIVED_T must implement implicit construction from const Loop& 
 and swap(DERIVED_T& other) for proper working of ContainmentTree
 
 It is invalid to have a ContainmentTree<DERIVED_T> instance. All objects 
 must be of type DERIVED_T that inherits from ContainmentTree<DERIVED_T>
 */
template <typename DERIVED_T>
class basic_containment_tree {
public:
    /**
     @brief a convenience typedef for things we contain
     */
    typedef DERIVED_T child_type;
    /**
     @brief Construct a root tree
     */
    basic_containment_tree();
    /**
     @brief Construct a normal tree
     @param loop a valid non-empty loop
     */
    basic_containment_tree(const Loop& loop);
    /**
     @brief Test if this tree contains another hierarchy
     @param other does this contain other?
     @return whether this contains other
     
     Roots return true for all normal trees, false for other roots
     Normal trees use winding test for other normal trees, false for roots
     */
    bool contains(const child_type& other) const;
    /**
     @brief Test if this tree contains a this point
     @param point does this contain point?
     @return whether this contains @a point
     
     Normal trees use the winding test, invalid trees return true always.
     */
    bool contains(const Point2Type& point) const;
    /**
     @brief Test if this is a valid tree?
     
     Normal trees have a loop and other stuff. Invalid trees are roots
     @return true for normal tree, false for roots
     */
    bool isValid() const;
    /**
     @brief Return reference to deepest child that contains point, 
     or to this tree if there are no such children.
     @param point the point to test for containment
     @return reference to deepest tree that contains @a point
     */
    child_type& select(const Point2Type& point);
    /**
     @brief Return reference to deepest child that contains point, 
     or to this tree if there are no such children.
     @param point the point to test for containment
     @return reference to deepest tree that contains @a point
     */
    const child_type& select(const Point2Type& point) const;
    /**
     @brief Insert tree @a other into this tree
     @param other the tree to be inserted. This variable WILL be 
     invalidated after insertion. The returned reference will point 
     to the new tree which holds the contents of @a other
     @return reference to the tree with the contents of @a other
     
     Insert @a other into this tree. @a other will be invalidated by 
     this process, but a reference to a tree with the contents of 
     @a other is returned. This function will correctly handle cases 
     where @a other contains this or the children of this.
     
     Cost is proportional to the depth at which @a other is placed in 
     this tree times the branching factor of each tree traversed 
     while reaching that depth times the cost of each winding test.
     */
    child_type& insert(child_type& other);
    /**
     @brief Get the loop that represents the extents of this tree
     @return The loop that represents the extents of this tree
     */
    const Loop& boundary() const;
    /**
     @brief Swap the contents of this tree with other.
     @param other The tree with which to swap contents
     
     Swap the contents of this tree with other. This involves swapping 
     each data member of this tree with the corresponding member in other. 
     Where possible, the fast, constant time implementation of swap is 
     used, so this function should run in constant time.
     
     This function will only swap the basic_containment_tree contents. 
     The derived type must provide an override to swap its own contents that 
     will also invoke this implementation of swap
     */
    void swap(child_type& other);
private:
    typedef std::list<child_type> containment_list;
    
    Loop m_loop;
    containment_list m_children;
};

/**
 @brief this is an example implementation of a class that would derive from 
 basic_containment_tree. This one simply stores an element of some type and 
 has no information about its surrounding topology.
 @param T the type of value this node stores
 
 
 */
template <typename T>
class ContainmentTree : public basic_containment_tree<ContainmentTree<T> > {
public:
    /// convenience typedef for type of data stored
    typedef T value_type;
    /// convenience typedef of parent class for invoking its implementations
    typedef basic_containment_tree<ContainmentTree<value_type> > parent_type;
    ContainmentTree();
    /**
     @brief construct this object from a loop
     @param loop from which this object is constructed
     The variable @a loop is passed to the parent constructor,
     m_value is default constructed
     */
    ContainmentTree(const Loop& loop);
    /**
     @brief construct this object from a loop and a value
     @param loop from which this object is constructed
     The variable @a loop is passed to the parent constructor,
     @param value what m_value is initialized with
     */
    ContainmentTree(const Loop& loop, const value_type& value);
    /// Accessor for m_value
    value_type& value();
    /// Accessor for m_value
    const value_type& value() const;
    /**
     @brief swap the contents of this object with @a other
     @param other the object with whic to swap contents
     This function will swap m_value, then invoke 
     parent_type::swap(other). All overrides MUST do this for the 
     layout of the basic_containment_tree to remain valid
     */
    void swap(ContainmentTree& other);
private:
    value_type m_value;
};

}

namespace std {

/**
 @brief This is the ContainmentTree specialization of std::swap()
 It will static_cast both lhs and rhs to DERIVED_T and invoke 
 lhs_cast_to_derived_t.swap(rhs_cast_to_derived_t);
 The derived type must provide an override that also invokes the basic 
 containment_tree's swap
 */
template <typename DERIVED_T>
void swap(mgl::basic_containment_tree<DERIVED_T>& lhs, 
        mgl::basic_containment_tree<DERIVED_T>& rhs);

}

#endif	/* MGL_CONTAINMENTTREE_DECL_H */

