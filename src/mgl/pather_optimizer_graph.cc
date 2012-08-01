#include <set>
#include <list>
#include <vector>

#include "pather_optimizer_graph.h"

namespace mgl {

pather_optimizer_graph::~pather_optimizer_graph() {
	clearBoundaries();
	clearPaths();
}

void pather_optimizer_graph::addPath(const OpenPath& path, 
		const PathLabel& label) {
	if(path.size() < 2) {
		Exception mixup("Attempted to add degenerate path to optimizer");
		throw mixup;
	}
	//create nodes, connect them
	std::list<node*> nodes;
	for(OpenPath::const_iterator iter = path.fromStart(); 
			iter != path.end(); 
			++iter) {
		node* currentNode = new node(*iter);
		nodeSet.insert(currentNode);
		//connect to previous node if it exists
		if(!nodes.empty()) {
			//link* a = ;
			currentNode->connect(nodes.back(), label);
			//link* b = ;
			nodes.back()->connect(currentNode, label);
//			linkSet.insert(a);
//			linkSet.insert(b);
//			requiredLinkSet.insert(a);
//			requiredLinkSet.insert(b);
		}
		nodes.push_back(currentNode);
	}
	//connect entry points
	connectEntry(nodes.front());
	connectEntry(nodes.back());
	entryNodeSet.insert(nodes.front());
	entryNodeSet.insert(nodes.back());
}

void pather_optimizer_graph::addPath(const Loop& loop, const PathLabel& label) {
	if(loop.size() < 3) {
		Exception mixup("Attempted to add degenerate loop to optimizer");
		throw mixup;
	}
	//create nodes, connect them
	std::list<node*> nodes;
	for(Loop::const_finite_cw_iterator iter = loop.clockwiseFinite(); 
			iter != loop.clockwiseEnd(); 
			++iter) {
		node* currentNode = new node(*iter);
		nodeSet.insert(currentNode);
		//connect to previous node if it exists
		if(!nodes.empty()) {
			currentNode->connect(nodes.back(), label);
			nodes.back()->connect(currentNode, label);
		}
		nodes.push_back(currentNode);
	}
	nodes.front()->connect(nodes.back(), label);
	nodes.back()->connect(nodes.front(), label);
	
	//connect them all to entry points
	for(std::list<node*>::iterator iter = nodes.begin(); 
			iter != nodes.end(); 
			++iter) {
		connectEntry(*iter);
	}
	
	//add them all to entry points
	for(std::list<node*>::iterator iter = nodes.begin(); 
			iter != nodes.end(); 
			++iter) {
		entryNodeSet.insert(*iter);
	}
}

void pather_optimizer_graph::addBoundary(const OpenPath& path) {
	//boundaries are broken down into linesegments
	if(path.size() > 1) {
		for(OpenPath::const_iterator iter = path.fromStart(); 
				iter != path.end(); 
				++iter) {
			boundaries.push_back(path.segmentAfterPoint(iter));
		}
	} else {
		Exception mixup("Attempted to add degenerate path to optimizer boundary");
		throw mixup;
	}
}

void pather_optimizer_graph::addBoundary(const Loop& loop) {
		//boundaries are broken down into linesegments
	if(loop.size() > 2) {
		for(Loop::const_finite_cw_iterator iter = loop.clockwiseFinite(); 
				iter != loop.clockwiseEnd(); 
				++iter) {
			boundaries.push_back(loop.segmentAfterPoint(iter));
		}
	} else {
		Exception mixup("Attempted to add degenerate loop to optimizer boundary");
		throw mixup;
	}
}

void pather_optimizer_graph::clearBoundaries() {
	boundaries.clear();
}

void pather_optimizer_graph::clearPaths() {
	for(NodeSetType::iterator iter = nodeSet.begin(); 
			iter != nodeSet.end(); 
			++iter) {
		delete *iter;
	}
}

void pather_optimizer_graph::optimizeInternal(abstract_optimizer::LabeledOpenPaths& 
		labeledpaths) {
	if(entryNodeSet.empty())
		return;
	node* currentNode = *(entryNodeSet.begin());
	currentNode = bruteForceNearestRequired(currentNode);
//	std::cout << "Current Number of required extrusions: " 
//				<< requiredLinkSet.size() << std::endl;
	std::cout << "Current Number of total nodes: " 
			<< nodeSet.size() << std::endl;
	size_t degreeAccum = 0;
	for(NodeSetType::const_iterator iter = nodeSet.begin(); 
			iter != nodeSet.end(); 
			++iter) {
		degreeAccum = std::max(degreeAccum, (*iter)->outlinks_size());
	}
	std::cout << "Maximum degree of the graph: "
			<< degreeAccum << std::endl;
	std::cout << "Average degree of the graph: "
			<< double(degreeAccum) / nodeSet.size() << std::endl;
	while(!nodeSet.empty()) {
		if(currentNode->outlinks_begin() == currentNode->outlinks_end()) {
			node* nextNode = bruteForceNearestRequired(currentNode);
			tryRemoveNode(currentNode);
			currentNode = nextNode;
			if(!nextNode)
				return;
//			Log::severe() << "ERROR: Pather painted himself into a corner" 
//					<< std::endl;
		}
		link* currentChoice = *(currentNode->outlinks_begin());
		for(node::iterator linkIter = currentNode->outlinks_begin(); 
				linkIter != currentNode->outlinks_end(); 
				++linkIter) {
			if(isBetter(currentChoice, *linkIter))
				currentChoice = *linkIter;
		}
		if(currentChoice->get_cost().isValid()) {
			LabeledOpenPath lp;
			lp.myLabel = currentChoice->get_cost();
			lp.myPath.appendPoint(currentChoice->get_from()->get_position());
			lp.myPath.appendPoint(currentChoice->get_to()->get_position());
			labeledpaths.push_back(lp);
		}
		node* nextNode = currentChoice->get_to();
		currentNode->disconnect(nextNode);
		nextNode->disconnect(currentNode);
		tryRemoveNode(currentNode);
		currentNode = nextNode;
	}
}

//void pather_optimizer_graph::removeLink(link* l) {
//	//find a link like this in our set
//	LinkSetType::iterator found = linkSet.find(l);
//	if(found != linkSet.end()) {
//		//remove all that "match" it
//		LinkSetType::iterator lower = found;
//		LinkSetType::iterator upper = found;
//		while(upper != linkSet.end() && 
//				link_undirected_comparator::match(l, *upper))
//			++upper;
//		for(;lower != linkSet.begin(); --lower)
//			if(!link_undirected_comparator::match(l, *lower))
//				break;
//		if(!link_undirected_comparator::match(l, *lower))
//				++lower;
//		linkSet.erase(lower, upper);
//	}
//	found = requiredLinkSet.find(l);
//	if(found != requiredLinkSet.end()) {
//		//remove all that "match" it
//		LinkSetType::iterator lower = found;
//		LinkSetType::iterator upper = found;
//		while(upper != requiredLinkSet.end() && 
//				link_undirected_comparator::match(l, *upper))
//			++upper;
//		for(;lower != requiredLinkSet.begin(); --lower)
//			if(!link_undirected_comparator::match(l, *lower))
//				break;
//		if(!link_undirected_comparator::match(l, *lower))
//				++lower;
//		requiredLinkSet.erase(lower, upper);
//	}
//	//eradicate the connectivity from the underlying graph
//	node* end1 = l->get_from();
//	node* end2 = l->get_to();
//	end1->disconnect(end2);
//	end2->disconnect(end1);
//}

void pather_optimizer_graph::tryRemoveNode(node* n) {
	for(node::iterator iter = n->outlinks_begin(); 
			iter != n->outlinks_end(); 
			++iter) {
		if((*iter)->get_cost().isRequired())
			return;
	}
	for(node::iterator iter = n->inlinks_begin(); 
			iter != n->inlinks_begin(); 
			++iter) {
		if((*iter)->get_cost().isRequired())
			return;
	}
//	while(n->outlinks_begin() != n->outlinks_end())
//		removeLink(*(n->outlinks_begin()));
//	while(n->inlinks_begin() != n->inlinks_begin())
//		removeLink(*(n->inlinks_begin()));
	nodeSet.erase(n);
	entryNodeSet.erase(n);
	delete n;
}

bool pather_optimizer_graph::crossesBoundaries(const libthing::LineSegment2& seg) {
	//test if this linesegment crosses any boundaries
	for(BoundaryListType::const_iterator iter = 
			boundaries.begin(); 
			iter != boundaries.end(); 
			++iter) {
		const libthing::LineSegment2& currentBoundary = *iter;
		if(seg.intersects(currentBoundary))
			return true;
	}
	return false;
}

void pather_optimizer_graph::connectEntry(node* n) {
	for(NodeSetType::const_iterator entryIter = entryNodeSet.begin(); 
			entryIter != entryNodeSet.end(); 
			++entryIter) {
		libthing::LineSegment2 segment(n->get_position(), 
				(*entryIter)->get_position());
		CostType cost;
		if(boundaries.empty() || crossesBoundaries(segment)) {
			//make it invalid, meaning dry move
			cost.myOwner = CostType::OWN_INVALID;
			cost.myType = CostType::TYP_INVALID;
			cost.myValue = -1;
		} else {
			//valid connectivity
			cost.myOwner = CostType::OWN_MODEL;
			cost.myType = CostType::TYP_CONNECTION;
			cost.myValue = 0;
			n->connect(*entryIter, cost);
			(*entryIter)->connect(n, cost);
		}
	}
}

pather_optimizer_graph::node* 
		pather_optimizer_graph::bruteForceNearestRequired(
		node* current) const {
	if(nodeSet.empty())
		return NULL;
	node* closest = (*nodeSet.begin());
	Scalar closestDist = (closest->get_position() - 
			current->get_position()).magnitude();
	int closestVal = highestValue(closest);
	for(NodeSetType::const_iterator iter = nodeSet.begin(); 
		iter != nodeSet.end(); 
		++iter){
		Scalar dist = ((*iter)->get_position() - 
				current->get_position()).magnitude();
		int val = highestValue(*iter);
		if(closest == current || 
				val > closestVal ||
				( val == closestVal && 
				dist < closestDist)) {
			closestDist = dist;
			closest = (*iter);
			closestVal = val;
		}
	}
	if(closest == current)
		return NULL;
	return closest;
}

bool pather_optimizer_graph::isBetter(link* current, link* alternate) const {
	Scalar curDist = (current->get_from()->get_position() - 
			current->get_to()->get_position()).magnitude();
	Scalar altDist = (alternate->get_from()->get_position() - 
			alternate->get_to()->get_position()).magnitude();
	CostType curCost = current->get_cost();
	CostType altCost = alternate->get_cost();
	
	if(curCost.isInvalid()) {
		if(altCost.isValid())
			return true;
		else
			return altDist < curDist;
	} else {
		if(altCost.isInvalid())
			return false;
	}
	//we're valid, as is the other guy
	if(curCost.isInset()) {
		if(!altCost.isInset())
			return false;
		else
			return altCost.myValue > curCost.myValue;
	}
	//we're not inset
	if(curCost.isInfill()) {
		if(altCost.isInset())
			return true;
		else
			return altCost.isInfill();
	}
	//we're not infill
	if(curCost.isConnection()) {
		if(altCost.isValid() && !altCost.isConnection())
			return true;
		else if(altCost.isConnection())
			return altDist < curDist;
		else 
			return false;
	}
	else return altDist < curDist;
}

int pather_optimizer_graph::highestValue(node* n) const {
	int hv = -1;
	for(node::const_iterator iter = n->outlinks_begin(); 
			iter != n->outlinks_end();
			++iter) {
		hv = std::max(hv, (*iter)->get_cost().myValue);
	}
	return hv;
}

bool pather_optimizer_graph::link_value_comparator::operator ()(
		const link& lhs, const link& rhs) const {
	return myCompare(lhs.get_cost(), rhs.get_cost());
}

bool pather_optimizer_graph::link_undirected_comparator::operator ()(
		const link& lhs, const link& rhs) const {
	node* lmin = std::min(lhs.get_from(), lhs.get_to());
	node* lmax = std::min(lhs.get_from(), lhs.get_to());
	node* rmin = std::min(rhs.get_from(), rhs.get_to());
	node* rmax = std::min(rhs.get_from(), rhs.get_to());
	return lmin == rmin ? lmax < rmax : lmin < rmin;
}

bool pather_optimizer_graph::link_undirected_comparator::operator ()(const link* lhs, const link* rhs) const {
	return this->operator ()(*lhs, *rhs);
}

bool pather_optimizer_graph::link_undirected_comparator::match(
		const link& lhs, const link& rhs) {
	node* lmin = std::min(lhs.get_from(), lhs.get_to());
	node* lmax = std::min(lhs.get_from(), lhs.get_to());
	node* rmin = std::min(rhs.get_from(), rhs.get_to());
	node* rmax = std::min(rhs.get_from(), rhs.get_to());
	return lmin == rmin && lmax == rmax;
}

bool pather_optimizer_graph::link_undirected_comparator::match(
		const link* lhs, const link* rhs) {
	return match(*lhs, *rhs);
}

}



