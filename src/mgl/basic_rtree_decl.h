/* 
 * File:   basic_rtree_decl.h
 * Author: Dev
 *
 * Created on October 1, 2012, 10:00 AM
 */

#ifndef MGL_BASIC_RTREE_DECL_H
#define	MGL_BASIC_RTREE_DECL_H

#include "mgl.h"
#include "spacial_data.h"

namespace mgl {

template <typename T, size_t C = 10>
class basic_rtree {
public:
    typedef T value_type;
    typedef std::allocator<basic_rtree> tree_alloc_t;
    typedef tree_alloc_t::rebind<value_type>::other child_alloc_t;
    
    class iterator{};
    class const_iterator{};
    
    basic_rtree();
    ~basic_rtree();
    
    iterator insert(const value_type& value);
    void erase(iterator iter);
    template <typename COLLECTION, typename FILTER>
    void search(COLLECTION& result, const FILTER& filt);
    
private:
    static const size_t CAPACITY = C;
    
    iterator insert(const value_type& value, const AABBox& bound);
    void split(basic_rtree* child);   //redistribute my children
    bool isLeaf() const { return myData; }
    
    AABBox myBounds;
    basic_rtree* myChildren[CAPACITY];
    size_t myChildrenCount;
    
    value_type* myData;
    
    tree_alloc_t myTreeAllocator;
    child_alloc_t myChildAllocator;
        
};

}

#endif	/* BASIC_RTREE_DECL_H */

