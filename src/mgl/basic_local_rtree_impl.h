/* 
 * File:   basic_local_rtree_impl.h
 * Author: Dev
 *
 * Created on December 6, 2012, 12:22 PM
 */

#ifndef MGL_BASIC_LOCAL_RTREE_IMPL_H
#define	MGL_BASIC_LOCAL_RTREE_IMPL_H

#include "basic_local_rtree_decl.h"
#include "intersection_index.h"


namespace mgl {

#define BLRT_TEMPLATE template <typename T, size_t C>
#define BLRT_TYPE basic_local_rtree<T, C>


BLRT_TEMPLATE
BLRT_TYPE::data_element::data_element(const value_type& value) 
        : m_value(to_bbox<value_type>::bound(value), value), 
        m_index(DEFAULT_CHILD_PTR()) {}
BLRT_TEMPLATE
BLRT_TYPE::data_element::data_element(const value_type& value, size_t index)
        : m_value(to_bbox<value_type>::bound(value), value), 
        m_index(index) {}
BLRT_TEMPLATE
BLRT_TYPE::basic_local_rtree() : m_root(DEFAULT_CHILD_PTR()) {}
BLRT_TEMPLATE
BLRT_TYPE::basic_local_rtree(const basic_local_rtree& other) 
        : m_root(DEFAULT_CHILD_PTR()) {
    for(data_const_iterator dataIter = other.m_data.begin(); 
            dataIter != other.m_data.end(); 
            ++dataIter) {
        insert(dataIter->second);
    }
}
BLRT_TEMPLATE
BLRT_TYPE& BLRT_TYPE::operator=(const basic_local_rtree& other) {
    if(&other == this)
        return *this;
    m_root = DEFAULT_CHILD_PTR();
    m_data.clear();
    m_nodes.clear();
    m_freenodes.clear();
    for(data_const_iterator dataIter = other.m_data.begin(); 
            dataIter != other.m_data.end(); 
            ++dataIter) {
        insert(dataIter->second);
    }
    return *this;
}
BLRT_TEMPLATE
typename BLRT_TYPE::iterator BLRT_TYPE::insert(const value_type& value) {
    node& child = acquireNode();
    data_iterator iter = m_data.insert(m_data.end(), 
            data_element(value, child.index()));
    child.setData(iter);
    if(m_root == DEFAULT_CHILD_PTR()) {
        m_root = child.index();
    } else {
        insertPrivate(m_root, child.index());
    }
    return iterator(iter);
}
BLRT_TEMPLATE
void BLRT_TYPE::erase(iterator iter) {
    size_t indexToErase = iter.m_base->m_index;
    m_data.erase(iter.m_base);
    erasePrivate(indexToErase);
}
BLRT_TEMPLATE
void BLRT_TYPE::clear() {
    m_data.clear();
    m_nodes.clear();
    m_freenodes.clear();
    m_root = DEFAULT_CHILD_PTR();
}
BLRT_TEMPLATE
template <typename COLLECTION, typename FILTER>
void BLRT_TYPE::search(COLLECTION& result, const FILTER& filt) const {
    if(m_root == DEFAULT_CHILD_PTR()) 
        return;
    searchPrivate(result, filt, m_root);
}
BLRT_TEMPLATE
void BLRT_TYPE::swap(basic_local_rtree& other) {
    m_data.swap(other.m_data);
    m_nodes.swap(other.m_nodes);
    m_freenodes.swap(other.m_freenodes);
    std::swap(m_root, other.m_root);
    for(typename node_container::iterator iter = m_nodes.begin(); 
            iter != m_nodes.end(); 
            ++iter) {
        iter->setParent(this);
    }
}
BLRT_TEMPLATE
void BLRT_TYPE::repr(std::ostream& out) const {
    if(m_root != DEFAULT_CHILD_PTR())
        repr(out, 0, m_root);
}
BLRT_TEMPLATE
void BLRT_TYPE::repr(std::ostream& out, size_t recursionLevel, 
        size_t index) const {
    for(size_t i = 0; i < recursionLevel; ++i) {
        out << '|';
    }
    out << 'N';
    if(m_nodes[index].hasData())
        out << "-L ";
    out << m_nodes[index].index();
    out << '\n';
    for(typename node::const_iterator iter = m_nodes[index].begin(); 
            iter != m_nodes[index].end(); 
            ++iter) {
        repr(out, recursionLevel + 1, iter->index());
    }
}
BLRT_TEMPLATE
void BLRT_TYPE::repr_svg(std::ostream& out) const {
    out << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"no\"?>" << std::endl;
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">" << std::endl;
    if(m_root != DEFAULT_CHILD_PTR())
        repr_svg(out, 1, m_root);
    out << "</svg>" << std::endl;
}
BLRT_TEMPLATE
void BLRT_TYPE::repr_svg(std::ostream& out, size_t recursionLevel, 
        size_t index) const {
    static const Scalar SCALE = 10.0;
    unsigned int rgbcolor = (255 << (recursionLevel*8)) | 
            (255 >> ((1+recursionLevel)*8));
    Scalar factor = (0.5 / SCALE) * recursionLevel;
    const node& curNode = dereferenceNode(index);
    AABBox bounds = curNode.bound().adjusted(Point2Type(factor, factor), 
            Point2Type(-factor, -factor));
    out << "<rect x=\"" << bounds.left() * SCALE << 
            "\" y=\"" << bounds.bottom() * SCALE << 
            "\" width=\"" << bounds.size_x() * SCALE << 
            "\" height=\"" << bounds.size_y() * SCALE << "\" " << 
            "style=\"fill-opacity:0.0;stroke:rgb(" << 
            (255 & (rgbcolor >> 16)) << ',' << 
            (255 & (rgbcolor >> 0)) << ',' << 
            (255 & (rgbcolor >> 8))
            << ");stroke-width:" << 
            2.0 / (1.0 + recursionLevel) << ";\"/>" << std::endl;
    for(typename node::const_iterator iter = curNode.begin(); 
            iter != curNode.end(); 
            ++iter) {
        repr_svg(out, recursionLevel + 1, iter->index());
    }
}
BLRT_TEMPLATE
typename BLRT_TYPE::node& BLRT_TYPE::acquireNode() {
    if(!m_freenodes.empty()) {
        node& ret = m_nodes[m_freenodes.back()];
        m_freenodes.pop_back();
        return ret;
    } else {
        m_nodes.push_back(node(*this, m_nodes.size()));
        return m_nodes.back();
    }
}
BLRT_TEMPLATE
void BLRT_TYPE::insertPrivate(size_t destination_index, size_t child_index, 
        bool can_reinsert) {
    size_t candidate = m_nodes[destination_index].selectCandidate(
            m_nodes[child_index]);
    if(candidate == destination_index || 
            m_nodes[candidate].height() == m_nodes[child_index].height()) {
        //handle the case of splitting a child
        if(m_nodes[destination_index].hasData()) {
            node& daughter = acquireNode();
            daughter.adoptFrom(m_nodes[destination_index]);
            m_nodes[destination_index].insert(daughter);
        }
        m_nodes[destination_index].insert(m_nodes[child_index]);
    } else {
        insertPrivate(candidate, child_index, can_reinsert);
        m_nodes[destination_index].readjustBounds(candidate);
    }
    //destination might be full, so split if necessary
    if(m_nodes[destination_index].isFull()) {
        size_t above_index = m_nodes[destination_index].above();
        if(can_reinsert && above_index != DEFAULT_CHILD_PTR()) {
            m_nodes[destination_index].reinsertOutliers(CAPACITY/2);
        } else {
            node& sibling = acquireNode();
            m_nodes[destination_index].shareWith(sibling);
            size_t sibling_index = sibling.index();
            if(above_index == DEFAULT_CHILD_PTR()) {
                //if no parent, add a stage
                node& successor = acquireNode();
                successor.adoptFrom(m_nodes[destination_index]);
                m_nodes[destination_index].insert(m_nodes[sibling_index]);
                m_nodes[destination_index].insert(successor);
            } else {
                m_nodes[above_index].insert(m_nodes[sibling_index]);
            }
        }
    }
}
BLRT_TEMPLATE
template <typename COLLECTION, typename FILTER>
void BLRT_TYPE::searchPrivate(COLLECTION& result, const FILTER& filt, 
        size_t base) const {
    const node& curNode = dereferenceNode(base);
    if(filt.filter(curNode.bound())) {
        if(curNode.hasData())
            result.push_back(curNode.data().second);
        for(typename node::const_iterator childIter = curNode.begin(); 
                childIter != curNode.end(); 
                ++childIter) {
            searchPrivate(result, filt, childIter->index());
        }
    }
}
BLRT_TEMPLATE
void BLRT_TYPE::erasePrivate(size_t index) {
    node& victim = m_nodes[index];
    if(victim.hasChildren()) {
        throw LocalTreeException("Attempted to erase non-empty node");
    }
    size_t above_index = victim.above();
    if(above_index != DEFAULT_CHILD_PTR()) {
        m_nodes[above_index].unlinkChild(index);
        if(!m_nodes[above_index].hasPurpose()) {
            erasePrivate(above_index);
        }
    }
    victim.clear();
    m_freenodes.push_back(index);
}


BLRT_TEMPLATE
BLRT_TYPE::node::node(basic_local_rtree& parent, size_t index) 
        : m_parent(&parent), m_index(index), 
        m_above(parent.DEFAULT_CHILD_PTR()), 
        m_height(DEFAULT_HEIGHT), 
        m_data(parent.DEFAULT_DATA_PTR()) {
    clearChildren();
}

BLRT_TEMPLATE
BLRT_TYPE::node::node(basic_local_rtree& parent, size_t index, 
        data_const_iterator data)
        : m_parent(&parent), m_index(index), 
        m_above(parent.DEFAULT_CHILD_PTR()), 
        m_height(0), 
        m_data(data), m_bounds(data->m_value.first) {
    clearChildren();
}

BLRT_TEMPLATE
bool BLRT_TYPE::node::hasData() const {
    return m_data != m_parent->DEFAULT_DATA_PTR();
}

BLRT_TEMPLATE
bool BLRT_TYPE::node::hasChildren() const {
    return m_childrenCount > 0;
}

BLRT_TEMPLATE
bool BLRT_TYPE::node::hasPurpose() const {
    return hasData() || hasChildren();
}

BLRT_TEMPLATE
bool BLRT_TYPE::node::isFull() const {
    return m_childrenCount >= CAPACITY;
}

BLRT_TEMPLATE
const typename BLRT_TYPE::bound_value& BLRT_TYPE::node::data() const {
    return m_data->m_value;
}

BLRT_TEMPLATE
const AABBox& BLRT_TYPE::node::bound() const {
    return m_bounds;
}

BLRT_TEMPLATE
size_t BLRT_TYPE::node::index() const {
    return m_index;
}

BLRT_TEMPLATE
size_t BLRT_TYPE::node::above() const {
    return m_above;
}

BLRT_TEMPLATE
size_t BLRT_TYPE::node::height() const {
    return m_height;
}

BLRT_TEMPLATE
void BLRT_TYPE::node::setParent(basic_local_rtree* parent) {
    m_parent = parent;
}

BLRT_TEMPLATE
void BLRT_TYPE::node::setData(data_const_iterator data) {
    m_data = data;
    m_bounds = m_data->m_value.first;
}

BLRT_TEMPLATE
void BLRT_TYPE::node::adoptFrom(node& surrogate) {
    std::swap(m_data, surrogate.m_data);
    if(hasData()) {
        std::swap(m_bounds, surrogate.m_bounds);
        std::swap(m_height, surrogate.m_height);
    }
    while(surrogate.m_childrenCount > 0) {
        size_t childIndex = surrogate.m_children[--surrogate.m_childrenCount];
        insert(m_parent->dereferenceNode(childIndex));
    }
}

BLRT_TEMPLATE
size_t BLRT_TYPE::node::selectCandidate(node& child) {
    if(!hasChildren())
        return m_index;
    if(m_parent->dereferenceNode(m_children[0]).hasData())
        return m_index;
    Scalar minGrowth = std::numeric_limits<Scalar>::max();
    size_t bestCandidate = 0;
    for(size_t i = 0; i < m_childrenCount; ++i) {
        node& current = m_parent->dereferenceNode(m_children[i]);
        Scalar oldP = current.m_bounds.perimeter();
        Scalar newP = current.m_bounds.expandedTo(child.m_bounds).perimeter();
        Scalar diff = newP - oldP;
        if(diff < minGrowth) {
            minGrowth = diff;
            bestCandidate = i;
        }
    }
    return m_children[bestCandidate];
}

BLRT_TEMPLATE
void BLRT_TYPE::node::insert(node& child) {
    if(m_childrenCount >= CAPACITY) {
        throw LocalTreeException("Overfilled node in insertDirectly");
    }
    m_children[m_childrenCount++] = child.m_index;
    child.m_above = m_index;
    if(m_childrenCount == 1) {
        m_bounds = child.m_bounds;
        m_height = child.m_height + 1;
    } else {
        if(m_height != child.m_height + 1) {
            throw LocalTreeException("Sibling node height mismatch");
        }
        m_bounds.expandTo(child.m_bounds);
    }
}

BLRT_TEMPLATE
void BLRT_TYPE::node::reinsertLeaves() {
    if(hasData()) {
        m_parent->insertPrivate(m_parent->m_root, m_index, false);
    } else {
        node tmpNode = *this;
        for(size_t i = 0; i < tmpNode.m_childrenCount; ++i) {
            tmpNode.m_parent->dereferenceNode(
                    tmpNode.m_children[i]).reinsertLeaves();
        }
        tmpNode.m_parent->dereferenceNode(tmpNode.m_index).clear();
        tmpNode.m_parent->m_freenodes.push_back(tmpNode.m_index);
    }
}

BLRT_TEMPLATE
void BLRT_TYPE::node::reinsertOutliers(size_t n) {
    if(n > 0) {
        Point2Type center = m_bounds.center();
        size_t outlierIndex = 0;
        Scalar outlierDistance = std::numeric_limits<Scalar>::min();
        for(size_t i = 0; i < m_childrenCount; ++i) {
            Scalar distance = (m_parent->dereferenceNode(
                    m_children[i]).bound().center() - 
                    center).squaredMagnitude();
            if(distance > outlierDistance) {
                outlierDistance = distance;
                outlierIndex = i;
            }
        }
        size_t outlier = m_children[outlierIndex];
        m_children[outlierIndex] = m_children[--m_childrenCount];
        m_bounds = m_parent->dereferenceNode(m_children[0]).bound();
        for(size_t i = 0; i < m_childrenCount; ++i) {
            m_bounds.expandTo(
                    m_parent->dereferenceNode(m_children[i]).m_bounds);
        }
        size_t myIndex = m_index;
        basic_local_rtree* myParent = m_parent;
        myParent->insertPrivate(m_parent->m_root, outlier, false);
        myParent->dereferenceNode(myIndex).reinsertOutliers(n - 1);
    }
}

BLRT_TEMPLATE
void BLRT_TYPE::node::readjustBounds() {
    if(hasChildren()) {
        m_bounds = m_parent->dereferenceNode(m_children[0]).m_bounds;
        for(iterator iter = begin(); iter != end(); ++iter) {
            m_bounds.expandTo(iter->m_bounds);
        }
    }
    if(m_above != m_parent->DEFAULT_CHILD_PTR()) {
        node& above = m_parent->dereferenceNode(m_above);
        above.readjustBounds();
    }
}

BLRT_TEMPLATE
void BLRT_TYPE::node::readjustBounds(unsigned int limit_index) {
    m_bounds.expandTo(m_parent->dereferenceNode(limit_index).m_bounds);
}

BLRT_TEMPLATE
void BLRT_TYPE::node::shareWith(node& sibling) {
    typedef std::pair<size_t, size_t> indexPair;
    
    static const size_t WORST_SIZE = CAPACITY/2;
    
    indexPair worstPairs[WORST_SIZE];
    size_t worstIndex = 0;
    
    
    if(m_childrenCount < CAPACITY) {
        throw LocalTreeException("Attempted to split non-full node");
    }
    //group children into pairs that would be worst to have in the same node
    while(m_childrenCount > 1) {
        indexPair worstPair(0,0);
        Scalar worstPerimeter = std::numeric_limits<Scalar>::min();
        for(size_t i = 0; i < m_childrenCount; ++i) {
            node& inode = m_parent->dereferenceNode(m_children[i]);
            for(size_t j = i + 1; j < m_childrenCount; ++j) {
                node& jnode = m_parent->dereferenceNode(m_children[j]);
                Scalar perimeter = inode.bound().expandedTo(
                        jnode.bound()).perimeter();
                if(perimeter > worstPerimeter) {
                    worstPerimeter = perimeter;
                    worstPair.first = i;
                    worstPair.second = j;
                }
            }
        }
        worstPairs[worstIndex++] = indexPair(m_children[worstPair.first], 
                m_children[worstPair.second]);
        m_children[worstPair.second] = m_children[--m_childrenCount];
        m_children[worstPair.first] = m_children[--m_childrenCount];
    }
    //if capacity is odd, there will be one child left in this node, 
    //this is fine, but readjust bounds
    if(m_childrenCount > 0) {
        m_bounds = m_parent->dereferenceNode(m_children[0]).m_bounds;
    }
    //distribute pairs such that worst combinations go in opposite nodes
    for(size_t i = 0; i < worstIndex; ++i) {
        Scalar aperim = m_bounds.perimeter();
        Scalar bperim = sibling.m_bounds.perimeter();
        //if we put first in this and second in other, how much growth
        Scalar a1g = m_bounds.expandedTo(
                m_parent->dereferenceNode(
                worstPairs[i].first).m_bounds).perimeter() - aperim;
        Scalar b1g = sibling.m_bounds.expandedTo(
                m_parent->dereferenceNode(
                worstPairs[i].second).m_bounds).perimeter() - bperim;
        //and the converse
        Scalar a2g = m_bounds.expandedTo(
                m_parent->dereferenceNode(
                worstPairs[i].second).m_bounds).perimeter() - aperim;
        Scalar b2g = sibling.m_bounds.expandedTo(
                m_parent->dereferenceNode(
                worstPairs[i].first).m_bounds).perimeter() - bperim;
        Scalar g1 = a1g + b1g;
        Scalar g2 = a2g + b2g;
        //now pick the best arrangement
        if(g1 < g2) {
            insert(m_parent->dereferenceNode(worstPairs[i].first));
            sibling.insert(m_parent->dereferenceNode(worstPairs[i].second));
        } else {
            insert(m_parent->dereferenceNode(worstPairs[i].second));
            sibling.insert(m_parent->dereferenceNode(worstPairs[i].first));
        }
    }
}

BLRT_TEMPLATE
bool BLRT_TYPE::node::unlinkChild(size_t child_index) {
    for(size_t i = 0; i < m_childrenCount; ++i) {
        if(m_children[i] == child_index) {
            m_children[i] = m_children[--m_childrenCount];
            return true;
        }
    }
    return false;
}

BLRT_TEMPLATE
void BLRT_TYPE::node::clear() {
    clearChildren();
    m_above = m_parent->DEFAULT_CHILD_PTR();
    m_height = DEFAULT_HEIGHT;
    m_data = m_parent->DEFAULT_DATA_PTR();
}

BLRT_TEMPLATE
void BLRT_TYPE::node::clearChildren() {
    for(size_t i = 0; i < CAPACITY; ++i) {
        m_children[i] = m_parent->DEFAULT_CHILD_PTR();
    }
    m_childrenCount = 0;
}

#undef BLRT_TYPE
#undef BLRT_TEMPLATE

}

#endif	/* MGL_BASIC_LOCAL_RTREE_IMPL_H */

