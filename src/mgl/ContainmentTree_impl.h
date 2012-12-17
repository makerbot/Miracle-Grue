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

#define CONTAINMENT_TEMPLATE template <typename DERIVED_T>
#define CONTAINMENT_TYPE basic_containment_tree<DERIVED_T>

class ContainmentException : public Exception {
public:
    template <typename T>
    ContainmentException(const T& arg) : Exception(arg) {}
};
CONTAINMENT_TEMPLATE
CONTAINMENT_TYPE::basic_containment_tree() {}
CONTAINMENT_TEMPLATE
CONTAINMENT_TYPE::basic_containment_tree(const Loop& loop) 
        : m_loop(loop) {
    if(m_loop.empty())
        throw ContainmentException("Attempted to construct ContainmentTree from empty loop!");
}
CONTAINMENT_TEMPLATE
bool CONTAINMENT_TYPE::contains(const child_type& other) const {
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
typename CONTAINMENT_TYPE::child_type& CONTAINMENT_TYPE::select(const Point2Type& point) {
    child_type* this_cast = static_cast<child_type*>(this);
    for(typename containment_list::iterator childIter = m_children.begin(); 
            childIter != m_children.end(); 
            ++childIter) {
        if(childIter->contains(point))
            return childIter->select(point);
    }
    return *this_cast;
}
CONTAINMENT_TEMPLATE
const typename CONTAINMENT_TYPE::child_type& CONTAINMENT_TYPE::select(
        const Point2Type& point) const {
    const child_type* this_cast = static_cast<const child_type*>(this);
    for(typename containment_list::const_iterator childIter = m_children.begin(); 
            childIter != m_children.end(); 
            ++childIter) {
        if(childIter->contains(point))
            return childIter->select(point);
    }
    return *this_cast;
}
CONTAINMENT_TEMPLATE
typename CONTAINMENT_TYPE::child_type& CONTAINMENT_TYPE::insert(child_type& other) {
    child_type* this_cast = static_cast<child_type*>(this);
    if(other.contains(*this_cast)) {
        /*
         This node will be placed inside other node. Since only other node
         is ok to invalidate and doing other.insert(*this) will invalidate
         this, we swap first.
         */
        this_cast->swap(other);
        insert(other);
        return *this_cast;
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
        m_children.push_back(child_type());
        m_children.back().swap(other);
        return m_children.back();
    }
}
CONTAINMENT_TEMPLATE
const Loop& CONTAINMENT_TYPE::boundary() const {
    return m_loop;
}
CONTAINMENT_TEMPLATE
void CONTAINMENT_TYPE::swap(child_type& other) {
    m_loop.swap(other.m_loop);
    m_children.swap(other.m_children);
}
CONTAINMENT_TEMPLATE
typename CONTAINMENT_TYPE::iterator CONTAINMENT_TYPE::begin() {
    return m_children.begin();
}
CONTAINMENT_TEMPLATE
typename CONTAINMENT_TYPE::const_iterator CONTAINMENT_TYPE::begin() const {
    return m_children.begin();
}
CONTAINMENT_TEMPLATE
typename CONTAINMENT_TYPE::iterator CONTAINMENT_TYPE::end() {
    return m_children.end();
}
CONTAINMENT_TEMPLATE
typename CONTAINMENT_TYPE::const_iterator CONTAINMENT_TYPE::end() const {
    return m_children.end();
}
CONTAINMENT_TEMPLATE
bool CONTAINMENT_TYPE::empty() const {
    return m_children.empty();
}
CONTAINMENT_TEMPLATE
size_t CONTAINMENT_TYPE::size() const {
    return m_children.size();
}
CONTAINMENT_TEMPLATE
typename CONTAINMENT_TYPE::iterator CONTAINMENT_TYPE::erase(iterator position) {
    return m_children.erase(position);
}
CONTAINMENT_TEMPLATE
typename CONTAINMENT_TYPE::iterator CONTAINMENT_TYPE::erase(iterator from, iterator to) {
    return m_children.erase(from, to);
}

#define EXAMPLE_TEMPLATE template <typename T>
#define EXAMPLE_TYPE ContainmentTree<T>

EXAMPLE_TEMPLATE
EXAMPLE_TYPE::ContainmentTree() {}
EXAMPLE_TEMPLATE
EXAMPLE_TYPE::ContainmentTree(const Loop& loop) : parent_type(loop) {}
EXAMPLE_TEMPLATE
EXAMPLE_TYPE::ContainmentTree(const Loop& loop, const T& value) 
        : parent_type(loop), m_value(value) {}
EXAMPLE_TEMPLATE
typename EXAMPLE_TYPE::value_type& EXAMPLE_TYPE::value() { 
    return m_value; 
}
EXAMPLE_TEMPLATE
const typename EXAMPLE_TYPE::value_type& EXAMPLE_TYPE::value() const { 
    return m_value; 
}
EXAMPLE_TEMPLATE
void EXAMPLE_TYPE::swap(ContainmentTree& other) {
    std::swap(m_value, other.m_value);
    parent_type::swap(other);
}


}

namespace std {

CONTAINMENT_TEMPLATE
void swap(mgl::CONTAINMENT_TYPE& lhs, mgl::CONTAINMENT_TYPE& rhs) {
    static_cast<typename mgl::CONTAINMENT_TYPE::child_type&>(lhs).swap(
            static_cast<typename mgl::CONTAINMENT_TYPE::child_type&>(rhs));
}

}

#undef EXAMPLE_TYPE
#undef EXAMPLE_TEMPLATE

#undef CONTAINMENT_TYPE
#undef CONTAINMENT_TEMPLATE

#endif /* MGL_CONTAINMENT_TREE_IMPL_H */

