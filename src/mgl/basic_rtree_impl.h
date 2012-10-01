/* 
 * File:   basic_rtree_impl.h
 * Author: Dev
 *
 * Created on October 1, 2012, 9:59 AM
 */

#ifndef MGL_BASIC_RTREE_IMPL_H
#define	MGL_BASIC_RTREE_IMPL_H

#include "basic_rtree_decl.h"

namespace mgl {

template <typename T, size_t C>
basic_rtree<T, C>::basic_rtree () 
        : myChildrenCount(0), myData(NULL) {
    for(size_t i = 0; i != CAPACITY; ++i)
        myChildren[i] = NULL;
}
template <typename T, size_t C>
basic_rtree<T, C>::~basic_rtree() {
    for(size_t i = 0; i != CAPACITY; ++i) {
        myTreeAllocator.destroy(myChildren[i]);
        myTreeAllocator.deallocate(myChildren[i]);
    }
    myChildAllocator.destroy(myData);
    myChildAllocator.deallocate(myData);
}
template <typename T, size_t C>
basic_rtree<T, C>::iterator basic_rtree<T, C>::insert(
        const basic_rtree<T, C>::value_type& value) {
    return insert(value, to_bbox<value_type>::bound(value));
}
template <typename T, size_t C>
basic_rtree<T, C>::iterator basic_rtree<T, C>::insert(
        const basic_rtree<T, C>::value_type& value, const AABBox& bound) {
    return iterator();
}

}


#endif	/* BASIC_RTREE_IMPL_H */

