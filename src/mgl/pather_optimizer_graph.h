/* 
 * File:   pather_optimizer_graph.h
 * Author: Dev
 *
 * Created on August 1, 2012, 10:28 AM
 */

#ifndef PATHER_OPTIMIZER_GRAPH_H
#define	PATHER_OPTIMIZER_GRAPH_H

#include "pather_optimizer.h"
#include "topology.h"
#include <set>
#include <vector>

namespace mgl {

class pather_optimizer_graph : public abstract_optimizer {
public:
	
	typedef PathLabel CostType;
	typedef topo::node_template<CostType, PointType> node;
	typedef topo::link_template<CostType, PointType> link;
	
	typedef std::pair<node*, node*> nodePair;
	
	virtual ~pather_optimizer_graph();

	void addPath(const OpenPath& path, 
			const PathLabel& label = 
			PathLabel(PathLabel::TYP_INSET, PathLabel::OWN_MODEL, 0));
	void addPath(const Loop& loop, 
			const PathLabel& label = 
			PathLabel(PathLabel::TYP_INSET, PathLabel::OWN_MODEL, 0));
	void addBoundary(const OpenPath& path);
	void addBoundary(const Loop& loop);
	void clearBoundaries() ;
	void clearPaths();
	
protected:
	
	void optimizeInternal(LabeledOpenPaths& labeledpaths);
	
private:
	
	class link_value_comparator{
	public:
		bool operator ()(const link& lhs, const link& rhs) const;
	private:
		basic_labelvalue_comparator<std::greater<int> > myCompare;
	};
	
	class link_undirected_comparator{
	public:
		bool operator ()(const link& lhs, const link& rhs) const;
		bool operator ()(const link* lhs, const link* rhs) const;
		static bool match(const link& lhs, const link& rhs);
		static bool match(const link* lhs, const link* rhs);
	};
	
	typedef std::set<link*, link_undirected_comparator> LinkSetType;
	typedef std::set<node*> NodeSetType;
	typedef std::vector<libthing::LineSegment2> BoundaryListType;
	
//	void removeLink(link* l);
	void tryRemoveNode(node* n);
	
	bool crossesBoundaries(const libthing::LineSegment2& seg);
	void connectEntry(node* n); //connects to other entries, you insert yourself
	
	node* bruteForceNearestRequired(node* current) const;
	
	bool isBetter(link* current, link* alternate) const;
	
	int highestValue(node* n) const;
	
	void bulkLineCrossings(std::list<std::pair<node*, node*> >& inputs, 
			std::list<nodePair>& notcrossOut, 
			std::list<nodePair>& yescrossOut);
	
	
//	LinkSetType linkSet;
//	LinkSetType requiredLinkSet;
	NodeSetType nodeSet;
	NodeSetType entryNodeSet;
	BoundaryListType boundaries;
	std::list<nodePair> entryConnects;
	
};

}



#endif	/* PATHER_OPTIMIZER_GRAPH_H */

