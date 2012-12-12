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

static const size_t LRTREE_DEFAULT_BRANCH = 4;

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
    class data_element {
    public:
        data_element(const value_type& value);
        data_element(const value_type& value, size_t index);
        /// value and its bounding box
        bound_value m_value;
        /// index of node containing this data, or default child ptr
        size_t m_index;
    };
    typedef std::list<data_element> data_container;
    
    typedef typename data_container::iterator data_iterator;
    typedef typename data_container::const_iterator data_const_iterator;
    
    template <typename BASE>
    class basic_iterator {
    public:
        friend class basic_local_rtree;
        template <typename OBASE>
        friend class basic_iterator;
        
        basic_iterator() {}
        template <typename OBASE>
        basic_iterator(const basic_iterator<OBASE>& other)
                : m_base(other.basic_iterator<OBASE>::m_base) {}
        template <typename OBASE>
        basic_iterator& operator =(const basic_iterator<OBASE>& other) {
            m_base = other.basic_iterator<OBASE>::m_base;
            return *this;
        }
        basic_iterator& operator ++() { ++m_base; return *this; } //pre
        basic_iterator operator ++(int) { //post
            basic_iterator clone = *this; ++*this; return clone; 
        }
        const value_type& operator *() const { 
            return m_base->m_value.second; 
        }
        const value_type* operator ->() { return &**this; }
        bool operator ==(const basic_iterator& other) const {
            return m_base == other.m_base;
        }
        bool operator !=(const basic_iterator& other) const
                { return !(*this==other); }
    private:
        template <typename OBASE>
        basic_iterator(OBASE base) 
                : m_base(base) {}

        BASE m_base;
    };
    
    typedef basic_iterator<data_iterator> iterator;
    typedef basic_iterator<data_const_iterator> const_iterator;
    
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
    /**
     @brief remove the data element pointed to by @aiter from the rtree
     @param iter the position of the data to remove
     This function will cleanly remove the data element at iter, 
     as well as any nodes that become empty as a result. 
     Existing iterators continue to be valid.
     Time complexity is usually constant, but can go to log(N) where 
     N is the size of the tree (as erasures propagate upward).
     */
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
    /// use for iterating through the data elements
    iterator begin() { return iterator(m_data.begin()); }
    /// use for iterating through the data elements
    iterator end() { return iterator(m_data.end()); }
    /// use for iterating through the data elements
    const_iterator begin() const { return const_iterator(m_data.begin()); }
    /// use for iterating through the data elements
    const_iterator end() const { return const_iterator(m_data.end()); }
    
    void repr(std::ostream& out) const;
    void repr(std::ostream& out, size_t recursionLevel, size_t index) const;
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
        
        static const size_t DEFAULT_HEIGHT = -1;
        
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
        ///@return this node's height, 0 for leaves, +1 for every layer above
        size_t height() const;
        /**
         @brief change the parent pointer
         @param parent
         */
        void setParent(basic_local_rtree* parent);
        /**
         @brief change which data element this node references
         @param data
         */
        void setData(data_const_iterator data);
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
         @brief recursively reinsert leaves below me, and clean up this node. 
         Assumes this is detached from parent
         */
        void reinsertLeaves();
        /**
         @brief select N children that lie furthest from the center and 
         reinsert them
         */
        void reinsertOutliers(size_t n);
        /**
         @brief readjust bounds tightly to children, recurse upward
         */
        void readjustBounds();
        /**
         @brieaf adjust my bounds to contain node at limit_index
         @param limit_index index of node that we must contain
         This is simple, quick, constant time, no recursion
         */
        void readjustBounds(size_t limit_index);
        /**
         @brief when this node is overloaded, it can offload some work 
         to the idle @a sibling
         @param sibling node that will receive part of this one's children
         */
        void shareWith(node& sibling);
        /**
         @brief remove child with @a child_index from my list of children
         @param child_index which child to attempt removing
         @return true if found and removed, else false
         */
        bool unlinkChild(size_t child_index);
        
        ///@brief iterate over the children
        iterator begin() { return iterator(this, 0); }
        ///@brief iterate over the children
        iterator end() { return iterator(this, m_childrenCount); }
        ///@brief iterate over the children
        const_iterator begin() const { return const_iterator(this, 0); }
        ///@brief iterate over the children
        const_iterator end() const { return const_iterator(this, m_childrenCount); }
        ///@brief make this node as though it was just created
        void clear();
    private:
        
        void clearChildren();
        ///@brief pointer to the basic_local_rtree that owns this node
        basic_local_rtree* m_parent;
        ///@brief this node's index in its parent basic_local_rtree
        size_t m_index;
        ///@brief the index of this node of which this is a child, or invalid
        size_t m_above;
        ///@brief the height of this node. 0 for leaves
        size_t m_height;
        ///@data element if leaf, otherwise end
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
    inline node& dereferenceNode(size_t index) { return m_nodes[index]; }
    /**
     @brief Get the node at @a index
     @param index
     @return node at index
     */
    inline const node& dereferenceNode(size_t index) const { return m_nodes[index]; }
    /**
     @brief perform all necessary steps for insertion, including 
     recursion and splitting
     @param destination_index Start with m_root, function will recurse with 
     the correct child and handle all other things
     @param child_index index of thing you're inserting
     @param can_reinsert allow reinsertions. True for actual insertion calls, 
     false for reinsertion calls to prevent infinite reinsertions
     */
    void insertPrivate(size_t destination_index, size_t child_index, 
            bool can_reinsert = false);
    /**
     @brief internal recursive implementation of search
     */
    template <typename COLLECTION, typename FILTER>
    void searchPrivate(COLLECTION& result, const FILTER& filt, size_t base) const;
    /**
     @brief cleanly erase node at index @a index, assuming its data element 
     has already been removed
     This will recurse upward, cleaning up empty nodes and maintaining 
     correct references as it goes
     */
    void erasePrivate(size_t index);
    
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

namespace std {

/**
 @brief overload of std::swap for local rtrees. Invokes lhs.swap(rhs)
 @param T the type template parameter for the rtree
 @param C the branching factor template parameter for the rtree
 @param lhs an rtree to be swapped, becomes @a rhs when done
 @param rhs an rtree to be swapped, becomes @a lhs when done
 */
template <typename T, size_t C>
void swap(mgl::basic_local_rtree<T, C>& lhs, mgl::basic_local_rtree<T, C>& rhs) {
    lhs.swap(rhs);
}

}

#endif	/* MGL_BASIC_LOCAL_RTREE_DECL_H */

