#ifndef MGL_BASIC_QUADTREE_IMPL_H
#define	MGL_BASIC_QUADTREE_IMPL_H

#include "basic_quadtree_decl.h"
#include "intersection_index.h"

namespace mgl {

template <typename T>
basic_quadtree<T>::basic_quadtree(AABBox bb) 
        : myBounds(bb), childrenExist(false) {
    for(size_t i = 0; i < CAPACITY; ++i) {
        myChildren[i] = DEFAULT_CHILD_PTR();
    }
}
template <typename T>
basic_quadtree<T>::basic_quadtree(const basic_quadtree& other)
        : myBounds(other.myBounds), 
        childrenExist(other.childrenExist) {
    for(size_t i = 0; i < CAPACITY; ++i) {
        myChildren[i] = DEFAULT_CHILD_PTR();
        if(other.childrenExist) {
            myChildren[i] = myTreeAllocator.allocate(1, this);
            myTreeAllocator.construct(myChildren[i], *other.myChildren[i]);
        }
    }
    for(typename data_container::const_iterator iter = other.myData.begin(); 
            iter != other.myData.end(); 
            ++iter) {
        myData.push_back(bounded_value(iter->first, 
                myDataAllocator.allocate(1)));
        myDataAllocator.construct(myData.back().second, *(iter->second));
    }
}
template <typename T>
basic_quadtree<T>& basic_quadtree<T>::operator =(const basic_quadtree& other) {
    tree_alloc_t tmpalloc;
    basic_quadtree tmpother(other);
    tmpalloc.destroy(this);
    tmpalloc.construct(this, tmpother);
    return *this;
}
template <typename T>
basic_quadtree<T>::~basic_quadtree() {
    while(!myData.empty()) {
        myDataAllocator.destroy(myData.back().second);
        myDataAllocator.deallocate(myData.back().second, 1);
        myData.pop_back();
    }
    if(hasChildren()) {
        for(size_t i = 0; i < CAPACITY; ++i) {
            myTreeAllocator.destroy(myChildren[i]);
            myTreeAllocator.deallocate(myChildren[i], 1);
        }
    }
}
template <typename T>
typename basic_quadtree<T>::iterator basic_quadtree<T>::insert(const value_type& value) {
    bounded_value bounded(to_bbox<value_type>::bound(value), 
            myDataAllocator.allocate(1));
    myDataAllocator.construct(bounded.second, value);
    insert(bounded);
    return iterator(*bounded.second);
}
template <typename T>
template <typename COLLECTION, typename FILTER>
void basic_quadtree<T>::search(COLLECTION& result, const FILTER& filt) const {
    if(!filt.filter(myBounds))
        return;
    for(typename data_container::const_iterator iter = myData.begin(); 
            iter != myData.end(); 
            ++iter) {
        if(filt.filter(iter->first))
            result.push_back(*(iter->second));
    }
    if(hasChildren())
        for(size_t i = 0; i < CAPACITY; ++i) {
            myChildren[i]->search(result, filt);
        }
}
template <typename T>
void basic_quadtree<T>::repr(std::ostream& out, unsigned int recursionLevel) {
    std::string tabs(recursionLevel, '|');
    out << tabs << 'N';//myBounds.m_min << " - " << myBounds.m_max;
    if(isLeaf())
        out << "-L";
    out << std::endl;
    if(hasChildren()) {
        for(size_t i = 0; i < CAPACITY; ++i) {
            myChildren[i]->repr(out, recursionLevel + 1);
        }
    }
}
template <typename T>
void basic_quadtree<T>::repr_svg(std::ostream& out, unsigned int recursionLevel) {
    if(!recursionLevel) {
        out << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"no\"?>" << std::endl;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">" << std::endl;
        repr_svg(out, 1);
        out << "</svg>" << std::endl;
        return;
    }
        
    unsigned int rgbcolor = (255 << (recursionLevel*8)) | 
            (255 >> ((1+recursionLevel)*8));
    Scalar factor = 0.0 * recursionLevel;
    AABBox bounds = myBounds.adjusted(Point2Type(factor, factor), Point2Type(-factor, -factor));
    out << "<rect x=\"" << bounds.left() << "\" y=\"" << bounds.bottom() << 
            "\" width=\"" << bounds.size_x() << "\" height=\"" << 
            bounds.size_y() << "\" " << 
            "style=\"fill-opacity:0.0;stroke:rgb(" << 
            (255 & (rgbcolor >> 16)) << ',' << 
            (255 & (rgbcolor >> 0)) << ',' << 
            (255 & (rgbcolor >> 8))
            << ");stroke-width:" << 
            2.0 / (1.0 + recursionLevel) << ";\"/>" << std::endl;
    if(hasChildren()) {
        for(size_t i = 0; i < CAPACITY; ++i) {
            myChildren[i]->repr_svg(out, recursionLevel + 1);
        }
    }
}
template <typename T>
void basic_quadtree<T>::insert(const bounded_value& bounded) {
    if(!hasData() && !hasChildren()) {
        myData.push_back(bounded);
    } else if(hasChildren()) {
        if(!tryPropagateInsert(bounded))
            myData.push_back(bounded);
    } else {
        myData.push_back(bounded);
        split_and_redistribute();
    }
}
template <typename T>
bool basic_quadtree<T>::tryPropagateInsert(const bounded_value& bounded) {
    for(size_t i = 0; i < CAPACITY; ++i) {
        if(myChildren[i]->myBounds.contains(bounded.first)) {
            myChildren[i]->insert(bounded);
            return true;
        }
    }
    return false;
}
template <typename T>
void basic_quadtree<T>::split() {
    for(size_t i = 0; i < CAPACITY; ++i)
        myChildren[i] = myTreeAllocator.allocate(1, this);
    myTreeAllocator.construct(myChildren[QUAD_BL], basic_quadtree(
            AABBox(myBounds.bottom_left(), myBounds.center())));
    myTreeAllocator.construct(myChildren[QUAD_BR], basic_quadtree(
            AABBox(myBounds.bottom_center(), myBounds.right_center())));
    myTreeAllocator.construct(myChildren[QUAD_TL], basic_quadtree(
            AABBox(myBounds.left_center(), myBounds.top_center())));
    myTreeAllocator.construct(myChildren[QUAD_TR], basic_quadtree(
            AABBox(myBounds.center(), myBounds.top_right())));
    childrenExist = true;
}
template <typename T>
void basic_quadtree<T>::split_and_redistribute() {
    split();
    data_container nonpropagated;
    for(typename data_container::iterator iter = myData.begin(); 
            iter != myData.end();
            ++iter) {
        if(!tryPropagateInsert(*iter)) {
            nonpropagated.push_back(*iter);
        }
    }
    myData.swap(nonpropagated);
}


}

#endif	/* MGL_BASIC_QUADTREE_IMPL_H */

