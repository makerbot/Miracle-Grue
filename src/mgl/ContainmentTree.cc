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
ContainmentTree& ContainmentTree::insert(ContainmentTree& other) {
    if(other.contains(*this)) {
        /*
         This node will be placed inside other node. Since only other node
         is ok to invalidate and doing other.insert(*this) will invalidate
         this, we swap first.
         */
        swap(other);
        insert(other);
        return *this;
    } else {
        typedef std::list<containment_list::iterator> move_list;
        move_list thingsToMove;
        /*
         Some of my children might end up inside other. Now is the
         time to transfer them.
         */
        for(containment_list::iterator childIter = m_children.begin(); 
                childIter != m_children.end(); 
                ++childIter) {
            if(other.contains(*childIter))
                thingsToMove.push_back(childIter);
        }
        for(move_list::iterator moveIter = thingsToMove.begin(); 
                moveIter != thingsToMove.end(); 
                ++moveIter) {
            other.insert(**moveIter);
            m_children.erase(*moveIter);
        }
        // the hierarchy is in order.
        m_children.push_back(ContainmentTree());
        m_children.back().swap(other);
        return m_children.back();
    }
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

