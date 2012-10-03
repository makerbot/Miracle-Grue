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
#include <set>
#include <iostream>
#include <algorithm>

namespace mgl {

template <typename T, size_t C>
basic_rtree<T, C>::basic_rtree() {
    myTreeAllocator.construct(this, basic_rtree(true));
}
template <typename T, size_t C>
basic_rtree<T, C>::basic_rtree(const basic_rtree& other) 
        : splitMyself(other.splitMyself), myBounds(other.myBounds), 
        myChildrenCount(other.myChildrenCount), myData(NULL) {
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
    return *this;
}
template <typename T, size_t C>
basic_rtree<T, C>::~basic_rtree() {
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
template <typename T, size_t C>
basic_rtree<T, C>::basic_rtree(const value_type& value) {
    myTreeAllocator.construct(this, basic_rtree(false));
    myData = myDataAllocator.allocate(1, this);
    myDataAllocator.construct(myData, value);
    myBounds = to_bbox<value_type>::bound(value);
}
template <typename T, size_t C>
basic_rtree<T, C>::basic_rtree(bool canReproduce) 
        : splitMyself(canReproduce), myChildrenCount(0), myData(NULL) {
    for(size_t i = 0; i != CAPACITY; ++i)
        myChildren[i] = NULL;
}
/* End Private constructors */
/* Public insert */
template <typename T, size_t C>
typename basic_rtree<T, C>::iterator basic_rtree<T, C>::insert(
        const basic_rtree::value_type& value) {
    basic_rtree* child = myTreeAllocator.allocate(1, this);
    myTreeAllocator.construct(child, basic_rtree(value));
    insert(child);
    return iterator(child);
}
/* End Public insert */
template <typename T, size_t C>
void basic_rtree<T, C>::erase(iterator) {}
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
//    std::string tabs(recursionLevel, '|');
//    out << tabs << 'N';//myBounds.m_min << " - " << myBounds.m_max;
//    if(isLeaf())
//        out << "-L";
//    if(splitMyself)
//        out << "-R";
//    out << std::endl;
    if(!recursionLevel) {
        out << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"no\"?>" << std::endl;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">" << std::endl;
        repr(out, 1);
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
        myChildren[i]->repr(out, recursionLevel + 1);
    }
}
template <typename T, size_t C>
void basic_rtree<T, C>::insert(basic_rtree* child) {
    if(isFull()) {
        throw TreeException("Overfilled R Tree node!");
    }
    if(isEmpty()) {
        insertDumb(child);
    } else if(isLeaf()) {
        basic_rtree* otherchild = myTreeAllocator.allocate(1, this);
        myTreeAllocator.construct(otherchild, basic_rtree(false));
        otherchild->adopt(this);
        insert(otherchild);
        insert(child);
    } else {
        //not leaf, but not empty
        basic_rtree* firstborn = myChildren[0];
        if(firstborn->isLeaf()) {
            //we're one level above leaves
            //make a sibling
            if(child->isLeaf()) {
                insertDumb(child);
            } else {
                throw TreeException("Leaf level mismatch!");
            }
        } else {
            //use brainpower to pick where to insert and do it
            insertIntelligently(child);
        }
        if(splitMyself && isFull()) {
            growTree();
        }
    }
    regrowBounds();
}
template <typename T, size_t C>
void basic_rtree<T, C>::insertDumb(basic_rtree* child) {
    myChildren[myChildrenCount++] = child;
    if(myChildrenCount == 1)
        myBounds = child->myBounds;
    else
        myBounds.expandTo(child->myBounds);
}
template <typename T, size_t C>
void basic_rtree<T, C>::insertIntelligently(basic_rtree* child) {
    typedef std::set<basic_rtree*> cset;
    typedef typename cset::iterator iterator;
    cset candidates;
    for(size_t i = 0; i < size(); ++i) {
        candidates.insert(myChildren[i]);
    }
    while(candidates.size() > 1) {
        iterator curworst = candidates.begin();
        Scalar curarea = 0;
        for(iterator iter = candidates.begin(); 
                iter != candidates.end(); 
                ++iter) {
            basic_rtree* iterchild = *iter;
            Scalar area = iterchild->myBounds.expandedTo(child->myBounds).area() - 
                    iterchild->myBounds.area();
            if(area > curarea) {
                curarea = area;
                curworst = iter;
            }
        }
        candidates.erase(curworst);
    }
    basic_rtree* winner = *candidates.begin();
    winner->insert(child);
    myBounds.expandTo(winner->myBounds);
    split(winner);
    regrowBounds();
}
template <typename T, size_t C>
void basic_rtree<T, C>::split(basic_rtree* child) {
    if(child->isLeaf())
        throw TreeException("Attempted to split a leaf!");
    if(isFull())
        throw TreeException("Can't split child of full node");
    if(!child->isFull())
        return;
    basic_rtree* contents[CAPACITY];
    for(size_t i = 0; i < CAPACITY; ++i) {
        contents[i] = child->myChildren[i];
        child->myChildren[i] = NULL;
    }
    
    child->myChildrenCount = 0;
    child->myBounds.reset();
    basic_rtree* clone = myTreeAllocator.allocate(1, child);
    myTreeAllocator.construct(clone, basic_rtree(false));

    //distribute even/odd
        
    struct intersection_area {
        size_t a, b;
        Scalar area;
        static bool compare(const intersection_area& lhs, 
                const intersection_area& rhs) {
            return lhs.area < rhs.area;
        }
    };
    
    intersection_area worst;
    worst.a = 0;
    worst.b = 1;
    worst.area = 0;
    for(size_t i = 2; i < CAPACITY; ++i) {
        if(!contents[i])
            continue;
        for(size_t j = i + 1; j < CAPACITY; ++j) {
            if(!contents[j])
                continue;
            Scalar area = contents[i]->myBounds.expandedTo(
                    contents[j]->myBounds).area() - 
                    contents[i]->myBounds.area() - 
                    contents[j]->myBounds.area();
            if(area > worst.area) {
                worst.a = i;
                worst.b = j;
                worst.area = area;
            }
        }
    }
    child->insertDumb(contents[worst.a]);
    clone->insertDumb(contents[worst.b]);
    std::swap(contents[worst.a], contents[0]);
    std::swap(contents[worst.b], contents[1]);
    std::vector<basic_rtree*> theRest;
    for(size_t i = 2; i < CAPACITY; ++i) {
        theRest.push_back(contents[i]);
    }
    bool picker = false;
    while(!theRest.empty()) {
        basic_rtree* current = picker ? child : clone;
        basic_rtree* other = !picker ? child : clone;
        typename std::vector<basic_rtree*>::iterator worstmatch = 
                std::max_element(theRest.begin(), theRest.end(), 
                        basic_rtree_intersect_comparator<value_type, CAPACITY>(other));
        current->insertDumb(*worstmatch);
        theRest.erase(worstmatch);
        picker = !picker;
    }
    insertDumb(clone);
    regrowBounds();
    if(splitMyself && isFull()) {
        growTree();
    }
    
}
template <typename T, size_t C>
void basic_rtree<T, C>::adopt(basic_rtree* from) {
    *this = basic_rtree(false);
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
template <typename T, size_t C>
void basic_rtree<T, C>::growTree() {
    basic_rtree* childling = myTreeAllocator.allocate(1, this);
    myTreeAllocator.construct(childling, basic_rtree(false));
    childling->adopt(this);
    insert(childling);
    split(childling);
    regrowBounds();
}

template <typename T, size_t C>
void basic_rtree<T, C>::regrowBounds() {
    if(!isEmpty())
        myBounds = myChildren[0]->myBounds;
    for(size_t i = 0; i < size(); ++i) {
        myBounds.expandTo(myChildren[i]->myBounds);
    }
}

}


#endif	/* BASIC_RTREE_IMPL_H */

