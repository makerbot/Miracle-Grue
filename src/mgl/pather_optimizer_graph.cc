#include <set>
#include <list>
#include <vector>
#include <limits>
#include <map>

#include "pather_optimizer_graph.h"
#include "loop_utils.h"

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
		node* currentNode = tryCreateNode(*iter);
		//connect to previous node if it exists
		if(!nodes.empty() && currentNode != nodes.back()) {
			currentNode->connect(nodes.back(), label);
			nodes.back()->connect(currentNode, label);
		}
		nodes.push_back(currentNode);
	}
	tryMarkEntry(nodes.front());
	tryMarkEntry(nodes.back());
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
		node* currentNode = tryCreateNode(*iter);
		//connect to previous node if it exists
		if(!nodes.empty()  && currentNode != nodes.back()) {
			currentNode->connect(nodes.back(), label);
			nodes.back()->connect(currentNode, label);
		}
		nodes.push_back(currentNode);
	}
	nodes.front()->connect(nodes.back(), label);
	nodes.back()->connect(nodes.front(), label);
	
	//add them all to entry points
	PointType lastEntry = nodes.front()->get_position();
	int skip = 0;
	static const int SKIP_MAX = 5;
	static const Scalar SKIP_DIST = 10.0;
	for(std::list<node*>::iterator iter = nodes.begin(); 
			iter != nodes.end(); 
			++iter) {
		PointType currentEntry = (*iter)->get_position();
		//don't add each one, but every 2mm
		if(iter == nodes.begin() || skip > SKIP_MAX ||
				(currentEntry - lastEntry).magnitude() > SKIP_DIST) {
			entryNodeSet.insert(*iter);
			lastEntry = currentEntry;
			skip = 0;
		} else {
			++skip;
		}
	}
}

void pather_optimizer_graph::addBoundary(const OpenPath& path) {
	boundariesSorted = false;
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
	boundariesSorted = false;
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
	boundariesSorted = false;
}

void pather_optimizer_graph::clearPaths() {
	for(NodeSet::iterator iter = nodeSet.begin(); 
			iter != nodeSet.end(); 
			++iter) {
		delete *iter;
	}
	entryNodeSet.clear();
	nodePositions.clear();
}

void pather_optimizer_graph::optimizeInternal(abstract_optimizer::LabeledOpenPaths& 
		labeledpaths, PointType startPoint) {
	pruneEntries();
	if(entryNodeSet.empty())
		return;
	node* currentNode = bruteForceNearestRequired(startPoint);
    size_t startSize = nodeSet.size();
	optimizer_progress<pather_optimizer_graph> progress;
//	std::cout << "Current Number of total nodes: " 
//			<< nodeSet.size() << std::endl;
//	std::cout << "Current Number of entry nodes: " 
//			<< entryNodeSet.size() << std::endl;
	while(!nodeSet.empty()) {
		if(currentNode->outlinks_size() == 0) {
			std::list<nodePair> input, yescross, nocross;
			connectEntry(currentNode, input);
			bulkLineCrossings(input, nocross, yescross);
			CostType cost(CostType::TYP_CONNECTION, 
					CostType::OWN_MODEL, 0);
			if(!nocross.empty()) {
				for(std::list<nodePair>::const_iterator iter = 
						nocross.begin(); 
						iter != nocross.end(); 
						++iter) {
					if(iter->first == currentNode)
						iter->first->connect(iter->second, cost);
					else
						iter->second->connect(iter->first, cost);
				}
			} else if(!yescross.empty()) {
				cost.myOwner = CostType::OWN_INVALID;
				cost.myType = CostType::TYP_INVALID;
				cost.myValue = -1;
				for(std::list<nodePair>::const_iterator iter = 
						yescross.begin(); 
						iter != yescross.end(); 
						++iter) {
					if(iter->first == currentNode)
						iter->first->connect(iter->second, cost);
					else
						iter->second->connect(iter->first, cost);
				}
			}
		}
		if(currentNode->outlinks_size() == 0) {
			node* nextNode = bruteForceNearestRequired(currentNode);
			tryRemoveNode(currentNode);
			currentNode = nextNode;
			if(!nextNode)
				return;
		}
//		if(currentNode->outlinks_size() > 2) {
//			std::cout << "Current number of links: " << currentNode->outlinks_size() 
//					<< std::endl;
//		}
        progress.report(startSize - nodeSet.size(), startSize);
		link* currentChoice = *(currentNode->outlinks_begin());
		node::iterator linkIter = currentNode->outlinks_begin();
		for(++linkIter; linkIter != currentNode->outlinks_end(); 
				++linkIter) {
			if(isBetter(currentChoice, *linkIter, labeledpaths))
				currentChoice = *linkIter;
		}
		appendMove(currentChoice, labeledpaths);
		node* nextNode = currentChoice->get_to();
		currentNode->disconnect(nextNode);
		nextNode->disconnect(currentNode);
		tryRemoveNode(currentNode);
		currentNode = nextNode;
	}
}

void pather_optimizer_graph::appendMove(link* l, 
		LabeledOpenPaths& labeledpaths) {
	if(l->get_cost().isValid()) {
		if((!labeledpaths.empty()) && 
				labeledpaths.back().myLabel == l->get_cost() && 
				!labeledpaths.back().myPath.empty() && 
				(*labeledpaths.back().myPath.fromEnd()) == 
				l->get_from()->get_position()) {
			labeledpaths.back().myPath.appendPoint(
			l->get_to()->get_position());
		} else {
			LabeledOpenPath lp;
			lp.myLabel = l->get_cost();
			lp.myPath.appendPoint(l->get_from()->get_position());
			lp.myPath.appendPoint(l->get_to()->get_position());
			labeledpaths.push_back(lp);
		}
	}
}

pather_optimizer_graph::node* pather_optimizer_graph::tryCreateNode(
		const PointType& pos) {
	node* createdNode = NULL;
	NodePositionMap::iterator mapped = nodePositions.find(pos);
	if(mapped == nodePositions.end()) {
		//no node at this exact position, create one
		nodePositions.insert(std::pair<PointType, node*>(pos, 
				createdNode = new node(pos)));
		nodeSet.insert(createdNode);
	} else {
		//if we have a node at this EXACT position, just use it
		createdNode = mapped->second;
	}
	return createdNode;
}

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
	nodeSet.erase(n);
	entryNodeSet.erase(n);
	nodePositions.erase(n->get_position());
	delete n;
}

void pather_optimizer_graph::tryMarkEntry(node* n) {
	//count valid paths
	int reqLinks = 0;
	int valLinks = 0;
	for(node::const_iterator initer = n->inlinks_begin(); 
			initer != n->inlinks_end(); 
			++initer) {
		link* l = *initer;
		CostType cost = l->get_cost();
		if(cost.isRequired())
			++reqLinks;
		if(cost.isValid())
			++valLinks;
	}
	for(node::const_iterator outiter = n->outlinks_begin(); 
			outiter != n->outlinks_end(); 
			++outiter) {
		link* l = *outiter;
		CostType cost = l->get_cost();
		if(cost.isRequired())
			++reqLinks;
		if(cost.isValid())
			++valLinks;
	}
	if(reqLinks <= 2 || valLinks <=4)
		entryNodeSet.insert(n);
//	else
//		std::cout << "FAIL to entrypoint" << std::endl;
}

void pather_optimizer_graph::pruneEntries() {
	NodeSet entryCandidates = entryNodeSet;
	//entryNodeSet.clear();
	for(NodeSet::const_iterator iter = entryCandidates.begin(); 
			iter != entryCandidates.end(); 
			++iter) {
		//tryMarkEntry(*iter);
	}
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

void pather_optimizer_graph::connectEntry(node* n, std::list<nodePair>& entries) {
	for(NodeSet::const_iterator entryIter = entryNodeSet.begin(); 
			entryIter != entryNodeSet.end(); 
			++entryIter) {
		if(n != *entryIter)
			entries.push_back(nodePair(n, *entryIter));
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
	for(NodeSet::const_iterator iter = nodeSet.begin(); 
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

pather_optimizer_graph::node* 
		pather_optimizer_graph::bruteForceNearestRequired(
		PointType point) const {
	if(nodeSet.empty())
		return NULL;
	node* closest = (*nodeSet.begin());
	Scalar closestDist = (closest->get_position() - 
			point).magnitude();
	int closestVal = highestValue(closest);
	for(NodeSet::const_iterator iter = nodeSet.begin(); 
		iter != nodeSet.end(); 
		++iter){
		Scalar dist = ((*iter)->get_position() - 
				point).magnitude();
		int val = highestValue(*iter);
		if(val > closestVal ||
				( val == closestVal && 
				dist < closestDist)) {
			closestDist = dist;
			closest = (*iter);
			closestVal = val;
		}
	}
	return closest;
}

bool pather_optimizer_graph::isBetter(link* current, 
		link* alternate, const LabeledOpenPaths& labeledpaths) const {
	Scalar curDist = (current->get_from()->get_position() - 
			current->get_to()->get_position()).magnitude();
	Scalar altDist = (alternate->get_from()->get_position() - 
			alternate->get_to()->get_position()).magnitude();
	CostType curCost = current->get_cost();
	CostType altCost = alternate->get_cost();
	int curVal = highestValue(current->get_to());
	int altVal = highestValue(alternate->get_to());
	PointType lastUnit;
	if(!labeledpaths.empty() && labeledpaths.back().myPath.size() > 1) {
		OpenPath::const_reverse_iterator iter = 
				labeledpaths.back().myPath.fromEnd();
		PointType last1 = *(iter++);
		PointType last2 = *iter;
		lastUnit = (last1 - last2).unit();
	}
	PointType curUnit = (current->get_from()->get_position() - 
			current->get_to()->get_position()).unit();
	PointType altUnit = (alternate->get_from()->get_position() - 
			alternate->get_to()->get_position()).unit();
	Scalar curDot = curUnit.dotProduct(lastUnit);
	Scalar altDot = altUnit.dotProduct(lastUnit);
	
	if(curCost.isInvalid()) {
		if(altCost.isValid())
			return true;
		else
			return altVal > curVal || 
					(altVal == curVal && altDist < curDist);
	} else {
		if(altCost.isInvalid())
			return false;
	}
	//we're valid, as is the other guy
	if(curCost.isInset()) {
		if(!altCost.isInset())
			return false;
		else
			if(altCost.myValue == curCost.myValue) {
				return altDot > curDot;
			} else {
				return altCost.myValue > curCost.myValue;
			}
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
			return altVal > curVal || 
					(altVal == curVal && altDist < curDist);
		else 
			return false;
	}
	else return altVal > curVal || 
			(altVal == curVal && altDist < curDist);
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

bool pather_optimizer_graph::link_undirected_comparator::operator ()(
		const link* lhs, const link* rhs) const {
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

bool pather_optimizer_graph::node_position_comparator::operator ()(
		const node& lhs, const node& rhs) const {
	return myCompare(lhs.get_position(), rhs.get_position());
}

bool pather_optimizer_graph::node_position_comparator::operator ()(
		const node* lhs, const node* rhs) const {
	return this->operator ()(*lhs, *rhs);
}

/* UTILITY functions used by bulkLineCrossings */

void orientNodepair(pather_optimizer_graph::nodePair& np) {
	if(np.first->get_position().x >= np.second->get_position().x)
		std::swap(np.first, np.second);
}

void orientLineSegment(libthing::LineSegment2& ls) {
	if(ls.a.x >= ls.b.x)
		std::swap(ls.a, ls.b);
}

bool compareOrientedNodepair(const pather_optimizer_graph::nodePair& lhs, 
		const pather_optimizer_graph::nodePair& rhs) {
	return lhs.first->get_position().x < 
			rhs.first->get_position().x;
}

bool compareOrientedLineSegment(const libthing::LineSegment2& lhs, 
		const libthing::LineSegment2& rhs) {
	return lhs.a.x < rhs.a.x;
}

class CheckBelowXnodepair {
public:
	CheckBelowXnodepair(Scalar nx, 
			std::list<pather_optimizer_graph::nodePair>& dst) 
			: myX(nx), myDest(dst) {}
	bool operator ()(pather_optimizer_graph::nodePair& np) const {
		bool ret = np.second->get_position().x < myX;
		if(ret) {
			myDest.push_back(np);
		}
		return ret;
	}
private:
	Scalar myX;
	std::list<pather_optimizer_graph::nodePair>& myDest;
};

void stripBeforeX(std::vector<pather_optimizer_graph::nodePair>& current, 
		Scalar x, 
		std::list<pather_optimizer_graph::nodePair>& dest) {
	typedef std::vector<pather_optimizer_graph::nodePair>::iterator iterator;
	
	iterator ibegin = current.begin();
	iterator iend = current.end();
	
	iterator goodEnd = std::remove_if(ibegin, 
			iend, CheckBelowXnodepair(x, dest));
	
	current.erase(goodEnd, current.end());
}

class CheckBelowXlinesegment {
public:
	CheckBelowXlinesegment(Scalar nx) : myX(nx) {}
	bool operator ()(const libthing::LineSegment2& ls) const {
		return ls.b.x < myX;
	}
private:
	Scalar myX;
};

void stripBeforeX(std::vector<libthing::LineSegment2>& current, 
		Scalar x) {
	typedef std::vector<libthing::LineSegment2>::iterator iterator;
	
	iterator ibegin = current.begin();
	iterator iend = current.end();
	
	iterator goodEnd = std::remove_if(ibegin, 
			iend, CheckBelowXlinesegment(x));
	
	current.erase(goodEnd, current.end());
}

class CheckNodepairIntersects {
public:
	CheckNodepairIntersects(const std::vector<libthing::LineSegment2>& bounds, 
			std::list<pather_optimizer_graph::nodePair>& dest)
			: myBounds(bounds), myDest(dest) {}
	bool operator ()(const pather_optimizer_graph::nodePair& np) const {
		typedef std::vector<libthing::LineSegment2>::const_iterator const_iterator;
		libthing::LineSegment2 nodeSeg(np.first->get_position(), 
				np.second->get_position());
		for(const_iterator boundIter = myBounds.begin(); 
				boundIter != myBounds.end(); 
				++boundIter) {
			if(boundIter->intersects(nodeSeg)) {
				myDest.push_back(np);
				return true;
			}
		}
		return false;
	}
private:
	const std::vector<libthing::LineSegment2>& myBounds;
	std::list<pather_optimizer_graph::nodePair>& myDest;
};

void stripIntersects(
		std::vector<pather_optimizer_graph::nodePair>& currentNodes, 
		std::vector<libthing::LineSegment2>& currentBoundaries, 
		std::list<pather_optimizer_graph::nodePair>& crossings) {
	typedef std::vector<pather_optimizer_graph::nodePair>::iterator node_iterator;
	
	node_iterator goodEnd = std::remove_if(currentNodes.begin(), 
			currentNodes.end(), 
			CheckNodepairIntersects(currentBoundaries, crossings));
	currentNodes.erase(goodEnd, currentNodes.end());
}

/* END UTILITY functions */

void pather_optimizer_graph::bulkLineCrossings(
		std::list<nodePair>& inputs, 
		std::list<nodePair>& notcrossOut, 
		std::list<nodePair>& yescrossOut) {
	//properly orient things
	std::for_each(inputs.begin(), inputs.end(), orientNodepair);
	if(!boundariesSorted)
		std::for_each(boundaries.begin(), boundaries.end(), orientLineSegment);
	//sort them on their starting x axis;
	inputs.sort(compareOrientedNodepair);
	if(!boundariesSorted)
		std::sort(boundaries.begin(), boundaries.end(), 
				compareOrientedLineSegment);
	boundariesSorted = true;
	std::vector<nodePair> activeInputs;
	std::vector<libthing::LineSegment2> activeBoundaries;
	std::list<nodePair>::const_iterator currentInput = inputs.begin();
	//iterate through the ordered x coordinates
	for(BoundaryListType::const_iterator boundaryIter = boundaries.begin(); 
			boundaryIter != boundaries.end(); 
			++boundaryIter) {
		Scalar currentX = boundaryIter->a.x;
		//remove from active boundaries everything that ends before this X
		stripBeforeX(activeBoundaries, currentX);
		//add to notcross things that haven't been eliminated
		stripBeforeX(activeInputs, currentX, notcrossOut);
		//add this boundary
		activeBoundaries.push_back(*boundaryIter);
		//add inputs before current x
		while(currentInput != inputs.end() && 
				currentInput->first->get_position().x < currentX) {
			activeInputs.push_back(*currentInput);
			++currentInput;
		}
		//check for intersections
		stripIntersects(activeInputs, activeBoundaries, yescrossOut);
	}
	//insert remaining things
	activeInputs.insert(activeInputs.end(), currentInput, 
		static_cast<std::list<nodePair>::const_iterator>(inputs.end()));
	stripIntersects(activeInputs, activeBoundaries, yescrossOut);
	//final strip out noninters
	notcrossOut.insert(notcrossOut.end(), activeInputs.begin(), 
		activeInputs.end());
	if(yescrossOut.size() + notcrossOut.size() != inputs.size()) {
		Exception mixup("Line intersection algorithm lost/gained links!");
		std::cout << "Input Size: " << inputs.size() << std::endl;
		std::cout << "Cross Size: " << yescrossOut.size() << std::endl;
		std::cout << "Valid Size: " << notcrossOut.size() << std::endl;
		std::cout << "Total Size: " << yescrossOut.size() + notcrossOut.size() 
				<< std::endl;
		throw mixup;
	}
}

}



