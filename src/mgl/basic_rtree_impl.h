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
#include <iostream>
#include <algorithm>

namespace mgl {

template <typename T, size_t C, typename DIAG>
basic_rtree<T, C, DIAG>::basic_rtree() 
        : splitMyself(true), myChildrenCount(0), myData(DEFAULT_DATA_PTR()){
    for(size_t i = 0; i != CAPACITY; ++i)
        myChildren[i] = DEFAULT_CHILD_PTR();
}
template <typename T, size_t C, typename DIAG>
basic_rtree<T, C, DIAG>::basic_rtree(const basic_rtree& other) 
        : splitMyself(other.splitMyself), myBounds(other.myBounds), 
        myChildrenCount(other.myChildrenCount), myData(DEFAULT_DATA_PTR()) {
    for(size_t i = 0; i != CAPACITY; ++i) {
        myChildren[i] = DEFAULT_CHILD_PTR();
        if(other.myChildren[i]) {
            myChildren[i] = myTreeAllocator.allocate(1, this);
            myTreeAllocator.construct(myChildren[i], *other.myChildren[i]);
        }
    }
    if(other.myData) {
        myData = myDataAllocator.allocate(1, this);
        myDataAllocator.construct(myData, *other.myData);
    }
}
template <typename T, size_t C, typename DIAG>
basic_rtree<T, C, DIAG>& basic_rtree<T, C, DIAG>::operator =(const basic_rtree& other) {
    if(this == &other)
        return *this;
    tree_alloc_t tmpAllocator;
    //HACK!!!
    basic_rtree tmpStorage(other); //in case other is child of this
    tmpAllocator.destroy(this);
    tmpAllocator.construct(this, tmpStorage);
    return *this;
}
template <typename T, size_t C, typename DIAG>
basic_rtree<T, C, DIAG>::~basic_rtree() {
    for(size_t i = 0; i != CAPACITY; ++i) {
        if(myChildren[i]) {
            myTreeAllocator.destroy(myChildren[i]);
            myTreeAllocator.deallocate(myChildren[i], 1);
        }
    }
    if(myData) {
        myDataAllocator.destroy(myData);
        myDataAllocator.deallocate(myData, 1);
    }
}
/* Private constructors */
template <typename T, size_t C, typename DIAG>
basic_rtree<T, C, DIAG>::basic_rtree(const value_type& value) {
    myTreeAllocator.construct(this, basic_rtree(false));
    myData = myDataAllocator.allocate(1, this);
    myDataAllocator.construct(myData, value);
    myBounds = to_bbox<value_type>::bound(value);
}
template <typename T, size_t C, typename DIAG>
basic_rtree<T, C, DIAG>::basic_rtree(bool canReproduce) 
        : splitMyself(canReproduce), myChildrenCount(0), myData(DEFAULT_DATA_PTR()) {
    for(size_t i = 0; i != CAPACITY; ++i)
        myChildren[i] = DEFAULT_CHILD_PTR();
}
/* End Private constructors */
/* Public insert */
template <typename T, size_t C, typename DIAG>
typename basic_rtree<T, C, DIAG>::iterator basic_rtree<T, C, DIAG>::insert(
        const basic_rtree::value_type& value) {
    basic_rtree* child = myTreeAllocator.allocate(1, this);
    myTreeAllocator.construct(child, basic_rtree(value));
    DIAG diag;
    insert(child, diag);
    return iterator(child);
    diag.displayOperations();
}
/* End Public insert */
template <typename T, size_t C, typename DIAG>
void basic_rtree<T, C, DIAG>::erase(iterator) {}
template <typename T, size_t C, typename DIAG>
template <typename COLLECTION, typename FILTER>
void basic_rtree<T, C, DIAG>::search(COLLECTION& result, const FILTER& filt) const {
    DIAG diag;
    searchPrivate(result, filt, diag);
    diag.displayOperations();
}
template <typename T, size_t C, typename DIAG>
template <typename COLLECTION, typename FILTER>
void basic_rtree<T, C, DIAG>::searchPrivate(COLLECTION& result, 
        const FILTER& filt, DIAG& diag) const {
    diag.addOperations(1);
    if(!filt.filter(myBounds))
        return;
    if(isLeaf()) {
        result.push_back(*myData);
    } else {
        for(size_t i = 0; i < size(); ++i) {
            myChildren[i]->searchPrivate(result, filt, diag);
        }
    }
}
template <typename T, size_t C, typename DIAG>
void basic_rtree<T, C, DIAG>::repr(std::ostream& out, unsigned int recursionLevel) {
    std::string tabs(recursionLevel, '|');
    out << tabs << 'N';//myBounds.m_min << " - " << myBounds.m_max;
    if(isLeaf())
        out << "-L";
    if(splitMyself)
        out << "-R";
    out << std::endl;
    for(size_t i = 0; i < size(); ++i) {
        myChildren[i]->repr(out, recursionLevel + 1);
    }
}
template <typename T, size_t C, typename DIAG>
void basic_rtree<T, C, DIAG>::repr_svg(std::ostream& out, unsigned int recursionLevel) {
    if(!recursionLevel) {
        out << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"no\"?>" << std::endl;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">" << std::endl;
        repr_svg(out, 1);
        out << "</svg>" << std::endl;
        return;
    }
        
    unsigned int rgbcolor = (255 << (recursionLevel*8)) | 
            (255 >> ((1+recursionLevel)*8));
    Scalar factor = -1.0 * recursionLevel;
    AABBox bounds = myBounds.adjusted(PointType(factor, factor), PointType(-factor, -factor));
    out << "<rect x=\"" << bounds.left() << "\" y=\"" << bounds.bottom() << 
            "\" width=\"" << bounds.size_x() << "\" height=\"" << 
            bounds.size_y() << "\" " << 
            "style=\"fill-opacity:0.0;stroke:rgb(" << 
            (255 & (rgbcolor >> 16)) << ',' << 
            (255 & (rgbcolor >> 0)) << ',' << 
            (255 & (rgbcolor >> 8))
            << ");stroke-width:" << 
            2.0 / (1.0 + recursionLevel) << ";\"/>" << std::endl;
    for(size_t i = 0; i < size(); ++i) {
        myChildren[i]->repr_svg(out, recursionLevel + 1);
    }
}
template <typename T, size_t C, typename DIAG>
void basic_rtree<T, C, DIAG>::insert(basic_rtree* child, DIAG& diag) {
    if(isFull()) {
        throw TreeException("Overfilled R Tree node!");
    }
    if(isEmpty()) {
        insertDumb(child);
        diag.addOperations(1);
    } else if(isLeaf()) {
        basic_rtree* otherchild = myTreeAllocator.allocate(1, this);
        myTreeAllocator.construct(otherchild, basic_rtree(false));
        otherchild->adopt(this);
        insert(otherchild, diag);
        insert(child, diag);
    } else {
        //not leaf, but not empty
        basic_rtree* firstborn = myChildren[0];
        if(firstborn->isLeaf()) {
            //we're one level above leaves
            //make a sibling
            if(child->isLeaf()) {
                insertDumb(child);
                diag.addOperations(1);
            } else {
                throw TreeException("Leaf level mismatch!");
            }
        } else {
            //use brainpower to pick where to insert and do it
            insertIntelligently(child, diag);
        }
        if(splitMyself && needSplitting()) {
            growTree(diag);
        }
    }
    regrowBounds();
}
template <typename T, size_t C, typename DIAG>
void basic_rtree<T, C, DIAG>::insertDumb(basic_rtree* child) {
    myChildren[myChildrenCount++] = child;
    if(myChildrenCount == 1)
        myBounds = child->myBounds;
    else
        myBounds.expandTo(child->myBounds);
}
template <typename T, size_t C, typename DIAG>
void basic_rtree<T, C, DIAG>::unlinkChild(size_t childIndex) {
    if(childIndex < myChildrenCount) {
        myChildren[childIndex] = myChildren[--myChildrenCount];
        myChildren[myChildrenCount] = DEFAULT_CHILD_PTR();
    }
}
template <typename T, size_t C, typename DIAG>
void basic_rtree<T, C, DIAG>::insertIntelligently(basic_rtree* child, DIAG& diag) {
    size_t best = 0;
    Scalar bestVal = std::numeric_limits<Scalar>::max();
    for(size_t i = 0; i < size(); ++i) {
        diag.addOperations(1);
        Scalar val = myChildren[i]->myBounds.expandedTo(child->myBounds).perimeter() - 
                myChildren[i]->myBounds.perimeter();
        if(val < bestVal) {
            bestVal = val;
            best = i;
        }
    }
    basic_rtree* winner = myChildren[best];
    winner->insert(child, diag);
    myBounds.expandTo(winner->myBounds);
    if(winner->needSplitting())
        split(winner, diag);
}
template <typename T, size_t C, typename DIAG>
void basic_rtree<T, C, DIAG>::split(basic_rtree* child, DIAG& diag) {
    using std::min;
    using std::max;
    if(child->isLeaf())
        throw TreeException("Attempted to split a leaf!");
    if(isFull())
        throw TreeException("Can't split child of full node");
    typedef std::vector<child_ptr_pair> child_vector;
    child_vector splittings;
    bool hasLeftover = false;
    basic_rtree* leftover = DEFAULT_CHILD_PTR();
    while(child->size() > 1) {
        diag.addOperations(child->size() * child->size());
        child_index_pair furthest = child->pick_furthest_children();
        splittings.push_back(child_ptr_pair(child->myChildren[furthest.first], 
                child->myChildren[furthest.second]));
        child->unlinkChild(max(furthest.first, furthest.second));
        child->unlinkChild(min(furthest.first, furthest.second));
    }
    if(child->size() > 1) {
        throw TreeException("This should not happen while splitting!");
    }
    if(child->size() == 1) {
        hasLeftover = true;
        leftover = child->myChildren[0];
        child->unlinkChild(0);
    }
    basic_rtree* clone = myTreeAllocator.allocate(1, child);
    myTreeAllocator.construct(clone, basic_rtree(false));
    child_ptr_pair destination(child, clone);
    for(typename child_vector::const_iterator iter = splittings.begin(); 
            iter != splittings.end(); 
            ++iter) {
        distributeChildPair(*iter, destination, diag);
    }
    if(hasLeftover) {
        distributeChild(leftover, destination, diag);
    }
    insertDumb(clone);
    diag.addOperations(1);
    if(splitMyself && isFull()) {
        growTree(diag);
    }
}
template <typename T, size_t C, typename DIAG>
typename basic_rtree<T, C, DIAG>::child_index_pair 
        basic_rtree<T, C, DIAG>::pick_furthest_children() const {
    child_index_pair ret;
    ret.first = 0;
    ret.second = 0;
    Scalar maxValue = std::numeric_limits<Scalar>::min();
    for(size_t i = 0; i < size(); ++i) {
        for(size_t j = i + 1; j < size(); ++j) {
            Scalar value = myChildren[i]->myBounds.expandedTo(
                    myChildren[j]->myBounds).perimeter();
            if(value > maxValue) {
                maxValue = value;
                ret.first = i;
                ret.second = j;
            }
        }
    }
    return ret;
}
template <typename T, size_t C, typename DIAG>
void basic_rtree<T, C, DIAG>::distributeChildPair(child_ptr_pair childs, 
        child_ptr_pair to, DIAG& diag) {
    if(!to.first->hasChildren() && !to.second->hasChildren()) {
        to.first->insert(childs.first, diag);
        to.second->insert(childs.second, diag);
    } else {
        Scalar firstVal = to.first->myBounds.perimeter();
        Scalar secondVal = to.second->myBounds.perimeter();
        Scalar growthA = to.first->myBounds.expandedTo(
                childs.first->myBounds).perimeter() - firstVal + 
                to.second->myBounds.expandedTo(
                childs.second->myBounds).perimeter() - secondVal;
        Scalar growthB = to.second->myBounds.expandedTo(
                childs.first->myBounds).perimeter() - secondVal + 
                to.first->myBounds.expandedTo(
                childs.second->myBounds).perimeter() - firstVal;
        if(growthA > growthB) {
            to.second->insertDumb(childs.first);
            to.first->insertDumb(childs.second);
        } else {
            to.first->insertDumb(childs.first);
            to.second->insertDumb(childs.second);
        }
        diag.addOperations(2);
    }
}
template <typename T, size_t C, typename DIAG>
void basic_rtree<T, C, DIAG>::distributeChild(basic_rtree* child, 
        child_ptr_pair to, DIAG& diag) {
    Scalar firstVal = to.first->myBounds.perimeter();
    Scalar secondVal = to.second->myBounds.perimeter();
    Scalar growthA = to.first->myBounds.expandedTo(
            child->myBounds).perimeter() - firstVal;
    Scalar growthB = to.second->myBounds.expandedTo(
            child->myBounds).perimeter() - secondVal;
    if(growthA > growthB) {
        to.second->insertDumb(child);
    } else {
        to.first->insertDumb(child);
    }
    diag.addOperations(1);
}
template <typename T, size_t C, typename DIAG>
void basic_rtree<T, C, DIAG>::adopt(basic_rtree* from) {
    *this = basic_rtree(false);
    for(size_t i = 0; i < CAPACITY; ++i) {
        myChildren[i] = from->myChildren[i];
        from->myChildren[i] = DEFAULT_CHILD_PTR();
    }
    myChildrenCount = from->myChildrenCount;
    from->myChildrenCount = 0;
    
    myData = from->myData;
    from->myData = DEFAULT_DATA_PTR();
    
    myBounds = from->myBounds;
    from->myBounds.reset();
    
}
template <typename T, size_t C, typename DIAG>
void basic_rtree<T, C, DIAG>::growTree(DIAG& diag) {
    basic_rtree* childling = myTreeAllocator.allocate(1, this);
    myTreeAllocator.construct(childling, basic_rtree(false));
    childling->adopt(this);
    insert(childling, diag);
    split(childling, diag);
    regrowBounds();
}

template <typename T, size_t C, typename DIAG>
void basic_rtree<T, C, DIAG>::regrowBounds() {
    if(!isEmpty())
        myBounds = myChildren[0]->myBounds;
    for(size_t i = 0; i < size(); ++i) {
        myBounds.expandTo(myChildren[i]->myBounds);
    }
}

}


#endif	/* BASIC_RTREE_IMPL_H */

