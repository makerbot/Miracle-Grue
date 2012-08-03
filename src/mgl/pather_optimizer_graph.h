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
#include "loop_utils.h"
#include <set>
#include <map>
#include <vector>

namespace mgl {

class pather_optimizer_graph : public abstract_optimizer {
public:
	
	pather_optimizer_graph() : boundariesSorted(false) {}
	
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
	
	class link_value_comparator {
	public:
		bool operator ()(const link& lhs, const link& rhs) const;
	private:
		basic_labelvalue_comparator<std::greater<int> > myCompare;
	};
	
	class link_undirected_comparator {
	public:
		bool operator ()(const link& lhs, const link& rhs) const;
		bool operator ()(const link* lhs, const link* rhs) const;
		static bool match(const link& lhs, const link& rhs);
		static bool match(const link* lhs, const link* rhs);
	};
	
	class node_position_comparator {
	public:
		bool operator ()(const node& lhs, const node& rhs) const;
		bool operator ()(const node* lhs, const node* rhs) const;
	private:
		basic_axisfunctor<> myCompare;
	};
	
	typedef std::set<link*, link_undirected_comparator> LinkSet;
	typedef std::set<node*> NodeSet;
	typedef std::map<PointType, node*, basic_axisfunctor<> > NodePositionMap;
	typedef std::vector<libthing::LineSegment2> BoundaryListType;
	

	void appendMove(link* l, LabeledOpenPaths& labeledpaths);
	
	node* tryCreateNode(const PointType& pos);
	void tryRemoveNode(node* n);
	
	bool crossesBoundaries(const libthing::LineSegment2& seg);
	void connectEntry(node* n, std::list<nodePair>& entries);
	
	node* bruteForceNearestRequired(node* current) const;
	
	bool isBetter(link* current, link* alternate) const;
	
	int highestValue(node* n) const;
	
	void bulkLineCrossings(std::list<std::pair<node*, node*> >& inputs, 
			std::list<nodePair>& notcrossOut, 
			std::list<nodePair>& yescrossOut);
	
	
//	LinkSetType linkSet;
//	LinkSetType requiredLinkSet;
	NodeSet nodeSet;
	NodeSet entryNodeSet;
	NodePositionMap nodePositions;
	BoundaryListType boundaries;
	bool boundariesSorted;
};

}



#endif	/* PATHER_OPTIMIZER_GRAPH_H */

