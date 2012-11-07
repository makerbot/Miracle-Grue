#ifndef MGL_BASIC_QUADTREE_DECL_H
#define	MGL_BASIC_QUADTREE_DECL_H

#include "mgl.h"
#include "spacial_data.h"
#include <sstream>
#include <memory>
#include <utility>
#include <vector>

namespace mgl {

class QuadTreeException : public Exception {
public:
    template <typename T>
    QuadTreeException(const T& arg) : Exception(arg) {}
};

/*
 basic_quadtree implements the interface for a spacial index.
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

template <typename T>
class basic_quadtree {
public:
    typedef T value_type;
    
    static value_type* DEFAULT_DATA_PTR() { return NULL; }
    static basic_quadtree* DEFAULT_CHILD_PTR() { return NULL; }
    
    typedef std::allocator<basic_quadtree> tree_alloc_t;
    typedef typename tree_alloc_t::template rebind<value_type>::other value_alloc_t;
    
    class iterator{
    public:
        iterator() {}
        template <typename U>
        iterator(const U&) {  }
    };
    typedef iterator const_iterator;
    
    basic_quadtree(AABBox bb);
    basic_quadtree(const basic_quadtree& other);
    basic_quadtree& operator =(const basic_quadtree& other);
    ~basic_quadtree();
    
    /*!Insert a value into the spacial index
     @value: a const reference of what should be inserted.
     a copy of this will be stored.
     @return: an iterator to what you just inserted (not implemented)*/
    iterator insert(const value_type& value);
    /*!Not implemented, do not use!*/
    void erase(iterator) {}
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
    void swap(basic_quadtree& other);
    
    /*!Not implemented, do not use!*/
    iterator begin() { return iterator(); }
    /*!Not implemented, do not use!*/
    iterator end() { return iterator(); }
    /*!Not implemented, do not use!*/
    const_iterator begin() const { return const_iterator(); }
    /*!Not implemented, do not use!*/
    const_iterator end() const { return const_iterator(); }
    
    void repr(std::ostream& out, size_t recursionLevel = 0);
    void repr_svg(std::ostream& out, size_t recursionLevel = 0);
    
private:
    
    static const size_t CAPACITY = 4;
    
    enum QUADRANT {
        QUAD_BL = 0,
        QUAD_BR = 1, 
        QUAD_TL = 2, 
        QUAD_TR = 3
    };
    
    typedef std::pair<AABBox, value_type*> bounded_value;
    typedef std::vector<bounded_value> data_container;
    
    void insert(const bounded_value& bounded);
    bool tryPropagateInsert(const bounded_value& bounded);
    
    bool isLeaf() const { return !childrenExist; }
    bool hasChildren() const { return childrenExist; }
    bool hasData() const { return !myData.empty(); }
    
    void split();
    void split_and_redistribute();
    
    AABBox myBounds;
    basic_quadtree* myChildren[CAPACITY];
    
    data_container myData;
    
    bool childrenExist;
    
    tree_alloc_t myTreeAllocator;
    value_alloc_t myDataAllocator;
    
    
};

}

#endif	/* MGL_BASIC_QUADTREE_DECL_H */

