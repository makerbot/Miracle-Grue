#include "pather_optimizer_fastgraph.h"
#include "pather.h"

namespace mgl {

int pather_optimizer_fastgraph::LabelTypeComparator::compare(
        const value_type& lhs, 
        const value_type& rhs) const {
    static const int OUTLINE_VALUE = 
            LayerPaths::Layer::ExtruderLayer::OUTLINE_LABEL_VALUE;
    const int INNERMOST_VALUE = 
            LayerPaths::Layer::ExtruderLayer::INSET_LABEL_VALUE + 
            grueCfg.get_nbOfShells() - 1;
    if(lhs.myValue == rhs.myValue)
            return SAME;
    return lhs.isInset() && !rhs.isInset() ? BETTER : 
        (rhs.isInset() && !lhs.isInset() ? WORSE : SAME);
    //fail outermost shell always
    if(lhs.myValue == OUTLINE_VALUE && rhs.myValue != OUTLINE_VALUE)
        return WORSE;
    if(rhs.myValue == OUTLINE_VALUE && lhs.myValue != OUTLINE_VALUE)
        return BETTER;
    //pass innermost shell always
    if(lhs.myValue == INNERMOST_VALUE && rhs.myValue != INNERMOST_VALUE)
        return BETTER;
    if(rhs.myValue == INNERMOST_VALUE && lhs.myValue != INNERMOST_VALUE)
        return WORSE;
    return SAME;
}
int pather_optimizer_fastgraph::LabelPriorityComparator::compare(
        const value_type& lhs, 
        const value_type& rhs) const {
    int diff = lhs.myValue - rhs.myValue;
    return (diff < 0 ? WORSE : 
        (diff > 0 ? BETTER : 
            SAME));
}
int pather_optimizer_fastgraph::NodeComparator::compare(const value_type& lhs, 
        const value_type& rhs) const {
    return m_labelCompare.compare(lhs.data().getLabel(), 
            rhs.data().getLabel());
}
int pather_optimizer_fastgraph::NodeConnectionComparator::compare(
        const value_type& lhs, const value_type& rhs) const {
    int nc = m_nodeCompare(*lhs.first, *rhs.first);
    if(nc)
        return nc;
    Scalar lunit = m_unit.dotProduct(lhs.second->normal());
    Scalar runit = m_unit.dotProduct(rhs.second->normal());
    return (lunit > runit ? 
        BETTER : (runit < lunit ? 
            WORSE : SAME));
}
int pather_optimizer_fastgraph::LoopHierarchyComparator::compare(
        const value_type& lhs, const value_type& rhs) const {
    int ret = m_compare(lhs.m_label, rhs.m_label);
    if(ret)
        return ret;
    Scalar lDist = std::numeric_limits<Scalar>::max();
    Scalar rDist = std::numeric_limits<Scalar>::max();
    for(bucket::LoopHierarchy::entryIndexVector::const_iterator iter = 
            lhs.m_entries.begin(); 
            iter != lhs.m_entries.end(); 
            ++iter) {
        Scalar distance = (m_entryPoint -
                m_graph[*iter].data().getPosition()).squaredMagnitude();
        if(distance < lDist)
            lDist = distance;
    }
    for(bucket::LoopHierarchy::entryIndexVector::const_iterator iter = 
            rhs.m_entries.begin(); 
            iter != rhs.m_entries.end(); 
            ++iter) {
        Scalar distance = (m_entryPoint -
                m_graph[*iter].data().getPosition()).squaredMagnitude();
        if(distance < lDist)
            rDist = distance;
    }
    return lDist < rDist ? BETTER : 
        (rDist < lDist ? WORSE : SAME);
}
bool pather_optimizer_fastgraph::connectionComparator::operator ()(
        const node::connection& lhs, const node::connection& rhs) const {
    if(lhs.second->myValue != rhs.second->myValue)
        return lhs.second->myValue < rhs.second->myValue;
    if(lhs.second->isValid() && rhs.second->isValid()) {
        return m_unit.dotProduct(lhs.second->normal()) < 
                m_unit.dotProduct(rhs.second->normal());
    }
    return lhs.second->distance() > rhs.second->distance();
}
bool pather_optimizer_fastgraph::nodeComparator::operator ()(const node& lhs, 
        const node& rhs) const {
    int nc = m_nodeCompare.compare(lhs, rhs);
    if(nc)
        return nc == BETTER;
    return (lhs.data().getPosition() - m_position).squaredMagnitude() < 
            (rhs.data().getPosition() - m_position).squaredMagnitude();
}
bool pather_optimizer_fastgraph::nodeComparator::operator ()(node_index lhs, 
        node_index rhs) const {
    return operator ()(m_graph[lhs], m_graph[rhs]);
}
bool pather_optimizer_fastgraph::probeCompare::operator ()(
        const probe_link_type& lhs, 
        const probe_link_type& rhs) {
    int nc = m_nodeCompare.compare(m_graph[lhs.first], m_graph[rhs.first]);
    if(nc)
        return nc == BETTER;
    Scalar distDifference = lhs.second - rhs.second;
    return distDifference < 0;
}

}



