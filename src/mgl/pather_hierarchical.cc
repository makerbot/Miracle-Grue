/* 
 * File:   pather_hierarchical.cpp
 * Author: Dev
 * 
 * Created on December 14, 2012, 11:25 AM
 */

#include <limits>
#include <iostream>

#include "pather_hierarchical.h"
#include "dump_restore.h"
#include <json/value.h>
#include <json/writer.h>

namespace mgl {


void pather_hierarchical::addPath(const OpenPath& path, 
        const PathLabel& label) {
    m_root.insert(path, label);
}
void pather_hierarchical::addPath(const Loop& loop, 
        const PathLabel& label) {
    m_root.insert(loop, label);
}
void pather_hierarchical::addBoundary(const OpenPath&) {
    throw HierarchyException("Insertion of path boundaries not implemented!");
}
void pather_hierarchical::addBoundary(const Loop& loop) {
    OutlineTree createdBoundary(loop);
    m_root.insert(createdBoundary);
}
void pather_hierarchical::clearPaths() {
    m_root.erase(m_root.begin(), m_root.end());
}
void pather_hierarchical::clearBoundaries() {
    clearPaths();
}
class LabelCompare : public abstract_predicate<PathLabel> {
public:
    typedef PathLabel value_type;
    int compare(const value_type& lhs, const value_type& rhs) const {
        return (lhs.myValue > rhs.myValue) ? BETTER : 
                ((lhs.myValue < rhs.myValue) ? WORSE : SAME);
    }
};
void pather_hierarchical::optimizeInternal(LabeledOpenPaths& result) {
    Json::FastWriter writer;
    Json::Value value;
    m_root.repr(value);
    std::cerr << writer.write(value);
    m_root.repr(std::cout);
    LabelCompare lc;
    m_root.traverse(result, m_historyPoint, lc);
}

pather_hierarchical::InsetTree::InsetTree() {}
pather_hierarchical::InsetTree::InsetTree(const Loop& loop, 
        const PathLabel& label) : parent_class(loop), 
        m_label(label) {}
void pather_hierarchical::InsetTree::insert(const OpenPath& path, 
        const PathLabel& label) {
    m_graph.insertPath(path, label);
}
void pather_hierarchical::InsetTree::insert(const Loop& loop, 
        const PathLabel& label) {
    m_graph.insertPath(loop, label);
}
pather_hierarchical::InsetTree& pather_hierarchical::InsetTree::insert(
        InsetTree& other) {
    return parent_class::insert(other);
}
void pather_hierarchical::InsetTree::swap(InsetTree& other) {
    std::swap(m_label, other.m_label);
    m_graph.swap(other.m_graph);
    parent_class::swap(other);
}
void pather_hierarchical::InsetTree::repr(std::ostream& out) const {
    repr(out, 0);
}
void pather_hierarchical::InsetTree::repr(std::ostream& out, size_t level) const {
    out << std::string(level, '|');
    out << 'N';
    if(isValid()) {
        out << '-' << m_label.myValue;
    }
    out << std::endl;
    for(const_iterator iter = begin(); iter != end(); ++iter) {
        iter->repr(out, level + 1);
    }
}
pather_hierarchical::OutlineTree::OutlineTree() {}
pather_hierarchical::OutlineTree::OutlineTree(const Loop& loop) 
        : parent_class(loop) {}
void pather_hierarchical::OutlineTree::insert(const Loop& loop, 
        const PathLabel& label) {
    Point2Type testPoint = *loop.clockwise();
    if(label.isInset()) {
        InsetTree createdNode(loop, label);
        select(testPoint).m_insets.select(testPoint).insert(createdNode);
    } else {
        select(testPoint).m_graph.insertPath(loop, label);
    }
}
void pather_hierarchical::OutlineTree::insert(const OpenPath& path, 
        const PathLabel& label) {
    Point2Type testPoint = *path.fromStart();
    if(label.isInset()) {
        select(testPoint).m_insets.select(testPoint).insert(path, label);
    } else {
        select(testPoint).m_graph.insertPath(path, label);
    }
}
pather_hierarchical::OutlineTree& 
        pather_hierarchical::OutlineTree::insert(OutlineTree& other) {
    return parent_class::insert(other);
}
void pather_hierarchical::OutlineTree::swap(OutlineTree& other) {
    m_insets.swap(other.m_insets);
    m_graph.swap(other.m_graph);
    parent_class::swap(other);
}
void pather_hierarchical::OutlineTree::repr(std::ostream& out) const {
    repr(out, 0);
}
void pather_hierarchical::OutlineTree::repr(Json::Value& out) const {
    out["type"] = "OutlineNode";
    if(isValid()) {
        Json::Value loopJson;
        dumpLoop(boundary(), loopJson);
        out["loop"] = loopJson;
    }
    Json::Value children;
    for(const_iterator iter = begin(); iter != end(); ++iter) {
        Json::Value child;
        iter->repr(child);
        children.append(child);
    }
    out["children"] = children;
}
void pather_hierarchical::OutlineTree::repr(std::ostream& out, 
        size_t level) const {
    out << std::string(level, '|') << "-L" << std::endl;
    for(const_iterator iter = begin(); 
            iter != end(); 
            ++iter) {
        iter->repr(out, level + 1);
    }
}
pather_hierarchical::OutlineTree::iterator 
        pather_hierarchical::OutlineTree::selectBestChild(
        Point2Type& entryPoint) {
    iterator bestIter = end();
    Scalar bestSquaredMagnitude = std::numeric_limits<Scalar>::max();
    Point2Type bestPoint;
    for(iterator iter = begin(); iter != end(); ++iter) {
        Scalar currentSquaredMagnitude = std::numeric_limits<Scalar>::max();
        Point2Type currentPoint;
        for(Loop::const_finite_cw_iterator loopIter = 
                iter->boundary().clockwiseFinite(); 
                loopIter != iter->boundary().clockwiseEnd(); 
                ++loopIter) {
            Scalar squaredMagnitude = (Point2Type(*loopIter) - 
                    entryPoint).squaredMagnitude();
            if(squaredMagnitude < currentSquaredMagnitude) {
                currentSquaredMagnitude = squaredMagnitude;
                currentPoint = *loopIter;
            }
        }
        if(currentSquaredMagnitude < bestSquaredMagnitude) {
            bestSquaredMagnitude = currentSquaredMagnitude;
            bestIter = iter;
            bestPoint = currentPoint;
        }
    }
    entryPoint = bestPoint;
    return bestIter;
}

}

