/* 
 * File:   ContainmentTree.cc
 * Author: FILIPP
 * 
 * Created on October 29, 2012, 12:16 PM
 */

#include "ContainmentTree.h"
#include "Exception.h"

namespace mgl {

class ContainmentException : public Exception {
public:
    template <typename T>
    ContainmentException(const T& arg) : Exception(arg) {}
};
ContainmentTree::ContainmentTree() {}
ContainmentTree::ContainmentTree(const Loop& loop) 
        : m_loop(loop) {
    if(m_loop.empty())
        throw ContainmentException("Attempted to construct ContainmentTree from empty loop!");
}
bool ContainmentTree::contains(const ContainmentTree& other) const {
    if(isValid()) {
        if(other.isValid()) {
            return contains(*other.m_loop.clockwise());
        } else {
            return false;
        }
    } else {
        return other.isValid();
    }
}
bool ContainmentTree::contains(const Point2Type& point) const {
    if(!isValid())
        return true;
    else {
        return m_loop.windingContains(point);
    }
}
bool ContainmentTree::isValid() const {
    return !m_loop.empty();
}
ContainmentTree& ContainmentTree::select(const Point2Type& point) {
    for(containment_list::iterator childIter = m_children.begin(); 
            childIter != m_children.end(); 
            ++childIter) {
        if(childIter->contains(point))
            return childIter->select(point);
    }
    return *this;
}
const ContainmentTree& ContainmentTree::select(const Point2Type& point) const {
    for(containment_list::const_iterator childIter = m_children.begin(); 
            childIter != m_children.end(); 
            ++childIter) {
        if(childIter->contains(point))
            return childIter->select(point);
    }
    return *this;
}
void ContainmentTree::swap(ContainmentTree& other) {
    m_loop.swap(other.m_loop);
    m_children.swap(other.m_children);
}


}

namespace std {

void swap(mgl::ContainmentTree& lhs, mgl::ContainmentTree& rhs) {
    lhs.swap(rhs);
}

}

