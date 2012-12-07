/* 
 * File:   basic_local_rtree.h
 * Author: Dev
 *
 * Created on December 6, 2012, 11:26 AM
 */

#ifndef MGL_BASIC_LOCAL_RTREE_DECL_H
#define	MGL_BASIC_LOCAL_RTREE_DECL_H

#include <utility>
#include <list>
#include <vector>
#include <iostream>
#include "spacial_data.h"

namespace mgl {

static const size_t LRTREE_DEFAULT_BRANCH = 5;

class LocalTreeException : public Exception {
public:
    template <typename T>
    LocalTreeException(const T& arg) : Exception(arg) {}
};

/*
 basic_rtree implements the interface for a spacial index.
 A spacial index is a class that contains two-dimensional objects and 
 allows queries for these objects based on their location and geometry.
 
 Internally, spacial data structures work on AABBox (axis aligned 
 bounding box). They must be able to construct a bounding box for 
 the type that they contain. This is done by calling the static function
 AABBox boundingBox = to_bbox<value_type>::bound(value);
 
 YOU MUST PROVIDE A TEMPLATE SPECIALIZATION FOR to_bbox<>::bound(...);
 following this form:
 template <>
 struct to_bbox<YourType> {
    static AABBox bound(const YourType&) {
        //see spacial_data.h for interface to AABBox
    }
 }
 
 Spacial indexes are safe to copy construct and assign. They store copies 
 of all things inserted into them and fill search results with copies of 
 their contents. You may have an index of pointers if you implement 
 the correct specialization of to_bbox.
 
 Spacial data structures may be queried by calling search(collection, filter)
 collection is any object that supports push_back(const value_type&). This 
 would usually be a list or vector of the same type as the spacial index.
 filter is any object with a function bool object::filter(const AABBox&) const; 
 This function should return true for bounding boxes that meet the criteria of 
 what you wish to search for, and false otherwise.
 
 Removal from spacial indexes is not universally implemented yet.
 
 Interface methods:
 
 void insert(const value_type& value);  //store a copy of value in this index
 template <typename CONTAINER, typename FILTER>
 void search(CONTAINER& result, const FILTER& filt); //query container
 void swap(spacial_index& other);   //fast swap implementation
 
 */

template <typename T, size_t C = LRTREE_DEFAULT_BRANCH>
class basic_local_rtree {
    
public:
    ///@brief the type of thing this tree contains
    typedef T value_type;
    
    ///@brief a value and its bounding box are stored together in a pair
    typedef std::pair<AABBox, value_type> bound_value;
    typedef std::list<bound_value> data_container;
    
    typedef typename data_container::iterator data_iterator;
    typedef typename data_container::const_iterator data_const_iterator;
    
    class iterator{
    public:
        iterator() {}
        template <typename U>
        iterator(const U&) {  }
    };
    typedef iterator const_iterator;
    
    ///@brief these constructors behave as expected
    basic_local_rtree();
    ///@brief these constructors behave as expected
    basic_local_rtree(const basic_local_rtree& other);
    ///@brief these constructors behave as expected
    basic_local_rtree& operator=(const basic_local_rtree& other);
    
    /*!Insert a value into the spacial index
     @value: a const reference of what should be inserted.
     a copy of this will be stored.
     @return: an iterator to what you just inserted (not implemented)*/
    iterator insert(const value_type& value);
    /*!Not implemented, do not use!*/
    void erase(iterator iter);
    /*!Search for values that meet criteria of filt.filter(AABBox)
     @result: Object supporting push_back(...) where output is placed
     @filter: object supporting filter(...) that defines the criteria
     Contents of index are not modified, copies of values that pass 
     the filter are placed in result.*/
    template <typename COLLECTION, typename FILTER>
    void search(COLLECTION& result, const FILTER& filt) const;
    /*!Swap contents of this object with that of another
     @other: The object with which to swap contents
     In general, this should be a constant time swap that involves 
     no copying of data elements*/
    void swap(basic_local_rtree& other);
    /*!Not implemented, do not use!*/
    iterator begin() { return iterator(); }
    /*!Not implemented, do not use!*/
    iterator end() { return iterator(); }
    /*!Not implemented, do not use!*/
    const_iterator begin() const { return const_iterator(); }
    /*!Not implemented, do not use!*/
    const_iterator end() const { return const_iterator(); }
    
    void repr_svg(std::ostream& out) const;
    void repr_svg(std::ostream& out, size_t recursionLevel, size_t index) const;
private:
    
    static const size_t CAPACITY = C;
    
    /**
     @brief A class for managing the hierarchical topology of this rtree.
     
     This is a POD class that keeps track of relationships between nodes 
     in the rtree. Data is stored in the basic_local_rtree parent, while 
     nodes store iterators and indexes.
     
     They should be small and cheap to copy.
     */
    class node {
    public:
        template <typename BASE>
        class basic_iterator {
        public:
            friend class node;
            basic_iterator() : m_owner(NULL), m_index(0) {}
            basic_iterator& operator ++() { ++m_index; return *this; } //pre
            basic_iterator operator ++(int) { //post
                basic_iterator clone = *this; ++*this; return clone; 
            }
            BASE& operator *() { 
                return m_owner->m_parent->
                        dereferenceNode(m_owner->m_children[m_index]); 
            }
            BASE* operator ->() { return &**this; }
            bool operator ==(const basic_iterator& other) const {
                return m_owner == other.m_owner && 
                        m_index == other.m_index;
            }
            bool operator !=(const basic_iterator& other) const
                    { return !(*this==other); }
        private:
            basic_iterator(BASE* owner, size_t index) 
                    : m_owner(owner), m_index(index) {}
            
            BASE* m_owner;
            size_t m_index;
        };
        
        typedef basic_iterator<node> iterator;
        typedef basic_iterator<const node> const_iterator;
        
        /**
         @brief construct a node owned by @a parent with @a index
         @param parent
         @param index
         */
        node(basic_local_rtree& parent, size_t index);
        /**
         @brief construct a node owned by @a parent with @a index and 
         containing @a data
         @param parent
         @param index
         @param data iterator to data reference by this node. Must not be 
         end(). For end, use constructor above.
         */
        node(basic_local_rtree& parent, size_t index, 
                data_const_iterator data);
        ///@return true if this node references valid data, else false
        bool hasData() const;
        ///@return true if this node has children, else false
        bool hasChildren() const;
        ///@return true if this node has a reason to continue existing
        bool hasPurpose() const;
        ///@return true if full and need splitting, else false
        bool isFull() const;
        ///@return data element referenced by this node
        const bound_value& data() const;
        ///@return this node's bounding box
        const AABBox& bound() const;
        ///@return this node's index in its parent
        size_t index() const;
        ///@return above's index or default child ptr if none
        size_t above() const;
        /**
         @brief Causes this node to adopt the data and children of @a surrogate
         @param surrogate the node from which livelihood is stolen
         
         Often we will construct empty nodes with valid indexes into our 
         collections, and other nodes with bogus parent/index information, 
         but useful data and children. This function will let the current 
         node receive the surrogate node's goodies while maintaining 
         its identification
         */
        void adoptFrom(node& surrogate);
        /**
         @brief select best node for inserting @a child
         @param child
         @return index of child node to receive child, or 
         own index if leaf.
         */
        size_t selectCandidate(node& child);
        /**
         @brief Directly insert @a child under this node, used when 
         we found where to put child, and it is here
         @param child any node
         */
        void insert(node& child);
        /**
         @brief adjust my above's bounds to include mine, recurse upward
         */
        void readjustBounds();
        /**
         @brief when this node is overloaded, it can offload some work 
         to the idle @a sibling
         @param sibling node that will receive part of this one's children
         */
        void shareWith(node& sibling);
        
        ///@brief iterate over the children
        iterator begin() { return iterator(this, 0); }
        ///@brief iterate over the children
        iterator end() { return iterator(this, m_childrenCount); }
        ///@brief iterate over the children
        const_iterator begin() const { return const_iterator(this, 0); }
        ///@brief iterate over the children
        const_iterator end() const { return const_iterator(this, m_childrenCount); }
        
    private:
        
        void clearChildren();
        basic_local_rtree* m_parent;
        size_t m_index;
        size_t m_above;
        data_const_iterator m_data;
        size_t m_children[CAPACITY];
        size_t m_childrenCount;
        AABBox m_bounds;
    };
    
    /**
     @brief Get a clean unused node. INVALIDATES EXISTING node&
     @return A newly created node reference or an unoccupied old node
     */
    node& acquireNode();
    /**
     @brief Get the node at @a index
     @param index
     @return node at index
     */
    node& dereferenceNode(size_t index);
    /**
     @brief Get the node at @a index
     @param index
     @return node at index
     */
    const node& dereferenceNode(size_t index) const;
    /**
     @brief perform all necessary steps for insertion, including 
     recursion and splitting
     @param destination_index Start with m_root, function will recurse with 
     the correct child and handle all other things
     @param child_index index of thing you're inserting
     */
    void insertPrivate(size_t destination_index, size_t child_index);
    
    typedef std::vector<node> node_container;
    typedef std::vector<size_t> node_vacancy_container;
    
    static size_t DEFAULT_CHILD_PTR() { return -1; }
    data_const_iterator DEFAULT_DATA_PTR() const { return m_data.end(); }
    
    data_container m_data;
    node_container m_nodes;
    node_vacancy_container m_freenodes;
    size_t m_root;
};

}

#endif	/* MGL_BASIC_LOCAL_RTREE_DECL_H */

