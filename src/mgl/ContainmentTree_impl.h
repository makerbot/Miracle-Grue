/* 
 * File:   ContainmentTree.cc
 * Author: FILIPP
 * 
 * Created on October 29, 2012, 12:16 PM
 */

#ifndef MGL_CONTAINMENT_TREE_IMPL_H
#define MGL_CONTAINMENT_TREE_IMPL_H

#include "ContainmentTree_decl.h"
#include "Exception.h"

namespace mgl {

#define CONTAINMENT_TEMPLATE template <typename T>
#define CONTAINMENT_TYPE ContainmentTree<T>

class ContainmentException : public Exception {
public:
    template <typename T>
    ContainmentException(const T& arg) : Exception(arg) {}
};
CONTAINMENT_TEMPLATE
CONTAINMENT_TYPE::ContainmentTree() {}
CONTAINMENT_TEMPLATE
CONTAINMENT_TYPE::ContainmentTree(const Loop& loop) 
        : m_loop(loop) {
    if(m_loop.empty())
        throw ContainmentException("Attempted to construct ContainmentTree from empty loop!");
}
CONTAINMENT_TEMPLATE
bool CONTAINMENT_TYPE::contains(const ContainmentTree& other) const {
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
CONTAINMENT_TEMPLATE
bool CONTAINMENT_TYPE::contains(const Point2Type& point) const {
    if(!isValid())
        return true;
    else {
        return m_loop.windingContains(point);
    }
}
CONTAINMENT_TEMPLATE
bool CONTAINMENT_TYPE::isValid() const {
    return !m_loop.empty();
}
CONTAINMENT_TEMPLATE
CONTAINMENT_TYPE& CONTAINMENT_TYPE::select(const Point2Type& point) {
    for(typename containment_list::iterator childIter = m_children.begin(); 
            childIter != m_children.end(); 
            ++childIter) {
        if(childIter->contains(point))
            return childIter->select(point);
    }
    return *this;
}
CONTAINMENT_TEMPLATE
const CONTAINMENT_TYPE& CONTAINMENT_TYPE::select(const Point2Type& point) const {
    for(typename containment_list::const_iterator childIter = m_children.begin(); 
            childIter != m_children.end(); 
            ++childIter) {
        if(childIter->contains(point))
            return childIter->select(point);
    }
    return *this;
}
CONTAINMENT_TEMPLATE
CONTAINMENT_TYPE& CONTAINMENT_TYPE::insert(ContainmentTree& other) {
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
        typedef std::list<typename containment_list::iterator> move_list;
        move_list thingsToMove;
        /*
         Some of my children might end up inside other. Now is the
         time to transfer them.
         */
        for(typename containment_list::iterator childIter = m_children.begin(); 
                childIter != m_children.end(); 
                ++childIter) {
            if(other.contains(*childIter))
                thingsToMove.push_back(childIter);
        }
        for(typename move_list::iterator moveIter = thingsToMove.begin(); 
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
CONTAINMENT_TEMPLATE
typename CONTAINMENT_TYPE::value_type& CONTAINMENT_TYPE::value() {
    return m_value;
}
CONTAINMENT_TEMPLATE
const typename CONTAINMENT_TYPE::value_type& CONTAINMENT_TYPE::value() const {
    return m_value;
}
CONTAINMENT_TEMPLATE
const Loop& CONTAINMENT_TYPE::boundary() const {
    return m_loop;
}
CONTAINMENT_TEMPLATE
void CONTAINMENT_TYPE::swap(ContainmentTree& other) {
    m_loop.swap(other.m_loop);
    m_children.swap(other.m_children);
}


}

namespace std {

CONTAINMENT_TEMPLATE
void swap(mgl::CONTAINMENT_TYPE& lhs, mgl::CONTAINMENT_TYPE& rhs) {
    lhs.swap(rhs);
}

}

#undef CONTAINMENT_TYPE
#undef CONTAINMENT_TEMPLATE

#endif /* MGL_CONTAINMENT_TREE_IMPL_H */

