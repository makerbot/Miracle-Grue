/* 
 * File:   basic_rtree_impl.h
 * Author: Dev
 *
 * Created on October 1, 2012, 9:59 AM
 */

#ifndef MGL_BASIC_RTREE_IMPL_H
#define	MGL_BASIC_RTREE_IMPL_H

#include "basic_rtree_decl.h"
#include <string>
#include <limits>

namespace mgl {

template <typename T, size_t C>
basic_rtree<T, C>::basic_rtree() 
        : myChildrenCount(0), myData(NULL) {
    for(size_t i = 0; i != CAPACITY; ++i)
        myChildren[i] = NULL;
}
template <typename T, size_t C>
basic_rtree<T, C>::basic_rtree(const basic_rtree& other) 
        : myChildrenCount(other.myChildren), myData(NULL) {
    for(size_t i = 0; i != CAPACITY; ++i) {
        myChildren[i] = NULL;
        if(other.myChildren[i]) {
            myChildren[i] = myTreeAllocator.allocate(1, this);
            myTreeAllocator.construct(myChildren[i], *other.myChildren[i]);
        }
    }
    if(other.myData) {
        myData = myDataAllocator.allocate(1, this);
        myDataAllocator.construct(myData, *other.myData);
    }
    return *this;
}
template <typename T, size_t C>
basic_rtree<T, C>& basic_rtree<T, C>::operator =(const basic_rtree& other) {
    if(this == &other)
        return *this;
    tree_alloc_t tmpAllocator;
    //HACK!!!
    basic_rtree tmpStorage(other); //in case other is child of this
    tmpAllocator.destroy(this);
    tmpAllocator.construct(this, tmpStorage);
}
template <typename T, size_t C>
basic_rtree<T, C>::~basic_rtree() {
    for(size_t i = 0; i != CAPACITY; ++i) {
        myTreeAllocator.destroy(myChildren[i]);
        myTreeAllocator.deallocate(myChildren[i]);
    }
    myDataAllocator.destroy(myData);
    myDataAllocator.deallocate(myData);
}
template <typename T, size_t C>
basic_rtree<T, C>::iterator basic_rtree<T, C>::insert(
        const basic_rtree::value_type& value) {
    return insert(value, to_bbox<value_type>::bound(value));
}
template <typename T, size_t C>
void basic_rtree<T, C>::erase(iterator iter) {
    iter;
}
template <typename T, size_t C>
template <typename COLLECTION, typename FILTER>
void basic_rtree<T, C>::search(COLLECTION& result, const FILTER& filt) {
    if(!filt.filter(myBounds))
        return;
    if(isLeaf()) {
        result.push_back(*myData);
    } else {
        for(size_t i = 0; i < size(); ++i) {
            myChildren[i]->search(result, filt);
        }
    }
}
template <typename T, size_t C>
void basic_rtree<T, C>::repr(std::ostream& out, unsigned int recursionLevel) {
    std::string tabs(recursionLevel, '\t');
    out << tabs << myBounds.m_min << " - " << myBounds.m_max;
    if(isLeaf())
        out << " - leaf";
    out << std::endl;
    for(size_t i = 0; i < size(); ++i) {
        myChildren[i]->repr(out, recursionLevel + 1);
    }
}
template <typename T, size_t C>
basic_rtree<T, C>::iterator basic_rtree<T, C>::insert(
        const basic_rtree::value_type& value, const AABBox& bound) {
    if(!isLeaf() && !size()) {
        //root
        myBounds = bound;
        myData = myDataAllocator.allocate(1, this);
        myDataAllocator.construct(myData, value);
        return iterator(this);
    } else if(isLeaf()) {
        basic_rtree* newborn = myTreeAllocator.allocate(1, this);
        myTreeAllocator.construct(newborn, basic_rtree());
        iterator ret = newborn->insert(value, bound);  //he stores the new data
        basic_rtree* adopted = myTreeAllocator.allocate(1, this);
        myTreeAllocator.construct(adopted, basic_rtree());
        adopted->adopt(this); //he steals all my data
        //i own them both
        insert(newborn);
        insert(adopted);
        return ret;
    } else {
        if(full()) {
            throw TreeException("Overfilled R Tree node!");
        }
        
        /*
         Here we select a child, insert into the child, 
         split if necessary, return correct iterator
         */
        //pick least populated
        size_t minsize = std::numeric_limits<size_t>::max();
        size_t minind = 0;
        for(size_t i = 0; i < size(); ++i) {
            if(myChildren[i]->size() < minsize) {
                minind = i;
                minsize = myChildren[i]->size();
            }
        }
        return myChildren[minind]->insert(value, bound);
    }
}
template <typename T, size_t C>
void basic_rtree<T, C>::insert(basic_rtree* child) {
    if(full()) {
        throw TreeException("Overfilled R Tree node!");
    }
    myChildren[myChildrenCount++] = child;
}
template <typename T, size_t C>
void basic_rtree<T, C>::split(basic_rtree* child) {
    if(child->isLeaf())
        throw TreeException("Attempted to split a leaf!");
    if(full())
        throw TreeException("Can't split child of full node");
    basic_rtree* contents[CAPACITY];
    for(size_t i = 0; i < CAPACITY; ++i) {
        contents[i] = child->myChildren[i];
        child->myChildren[i] = NULL;
    }
    child->myChildrenCount = 0;
    child->myBounds.reset();
    basic_rtree* clone = myTreeAllocator.allocate(1, child);
    myTreeAllocator.construct(clone, basic_rtree());
    //distribute even/odd
    for(size_t i = 0; i < CAPACITY; ++i) {
        if([contents[i]]) {
            basic_rtree* curthing = (i & 1 ? child : clone);
            curthing->insert(contents[i]);
        }
    }
    myBounds.growTo(child->myBounds);
    insert(clone);
    
}
template <typename T, size_t C>
void basic_rtree<T, C>::adopt(basic_rtree* from) {
    *this = basic_rtree();
    for(size_t i = 0; i < CAPACITY; ++i) {
        myChildren[i] = from->myChildren[i];
        from->myChildren[i] = NULL;
    }
    myChildrenCount = from->myChildrenCount;
    from->myChildrenCount = 0;
    
    myData = from->myData;
    from->myData = NULL;
    
    myBounds = from->myBounds;
    from->myBounds.reset();
    
}

}


#endif	/* BASIC_RTREE_IMPL_H */

