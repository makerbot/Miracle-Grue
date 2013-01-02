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
     @brief the type of container used for iterating over the children
     */
    typedef std::list<child_type> containment_list;
    /*
     @brief iterator used for iterating over children
     */
    typedef typename containment_list::iterator iterator;
    /*
     @brief const_iterator used for iterating over children
     */
    typedef typename containment_list::const_iterator const_iterator;
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
     @brief When removing a child from a parent, but wishing to preserve 
     some grandchildren, invoke parent.insert(grandchilditerator, child)
     @param donation an iterator to the grandchild that needs to be preserved
     @param donor the child that is currently a direct ancestor to the 
     grandchild indicated by @a donation
     @return a reference to the grandchild that was just donated.
     
     The end result of this function is that the current tree node has 
     @a donation as its direct descendent, and @a donor no longer has any 
     record of @a donation. To preserve the correct topology of the 
     containment tree, you MUST ERASE DONOR FROM THE CURRENT NODE after 
     all necessary grandchildren have been transferred.
     
     In the future, to ensure correct usage, this function might get replaced 
     by promote(iterator_to_child, functor_to_test_grandchildren) that will 
     automatically transfer all grandchildren for which the functor returns 
     true, then erase the child.
     */
    child_type& insert(iterator donation, child_type& donor);
    /**
     @brief When removing an child from a parent, but wishing to preserve some 
     granchildren, invoke parent.insert(fromgrandchild, tograndchild, child);
     @param from specifies the start of the range of grandchildren to transfer
     @param to specifies the end of the range of grandchildren to transfer, 
     not including the grandchild at to
     
     Operates exactly like insert(iterator, child_type&) but for a range of 
     grandchildren.
     
     You can insert all grandchildren by invoking 
     insert(donor.begin(), donor.end(), donor);
     */
    void insert(iterator from, iterator to, child_type& donor);
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
    /**
     @brief Get an iterator to the start of my children
     @return iterator to start of children
     
     Iterators are invalidated by insertions.
     */
    iterator begin();
    /**
     @brief Get a const iterator to the start of my children
     @return const_iterator to start of children
     
     Iterators are invalidated by insertions.
     */
    const_iterator begin() const;
    /**
     @brief Get an iterator to past-the-end of my children
     @return iterator to past-the-end of my children
     */
    iterator end();
    /**
     @brief Get a const iterator to past-the-end of my children
     @return const_iterator to past-the-end of my children
     */
    const_iterator end() const;
    /**
     @brief Test if there exist no children.
     @return true if no children exist, else false
     */
    bool empty() const;
    /**
     @brief Get the number of children
     @return number of children
     */
    size_t size() const;
    /**
     @brief erase my child at position specified
     @param position indicate from where to erase a child
     @return iterator pointing to the next element (or end()).
     
     This will not cause any rearrangement of other elements in the tree, 
     and should leave existing iterators valid (except the one pointing to 
     the element to be erased).
     */
    iterator erase(iterator position);
    /**
     @brief erase a range of children specified by from (inclusive) and 
     to (exclusive)
     @param from specifies start of the range to erase. This element is erased.
     @param to specifies one past the end of the range to erase. This element 
     is not erased.
     @return iterator to the element that was at @a to prior to erasing 
     (or end()).
     
     This will not cause any rearrangement of other elements in the tree, 
     and should leave existing iterators valid (except ones pointing to 
     the elements to be erased).
     */
    iterator erase(iterator from, iterator to);
private:
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

