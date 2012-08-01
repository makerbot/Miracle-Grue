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

namespace mgl {

class pather_optimizer_graph : public abstract_optimizer {
public:
	typedef PathLabel CostType;
	typedef topo::node_template<CostType, PointType> node;
	typedef topo::link_template<CostType, PointType> link;
	
	class link_comparator{
	public:
		bool operator ()(const link& lhs, const link& rhs) const {
			return myCompare(lhs.get_cost(), rhs.get_cost());
		}
	private:
		basic_labelvalue_comparator<std::greater<int> > myCompare;
	};
};

}



#endif	/* PATHER_OPTIMIZER_GRAPH_H */

