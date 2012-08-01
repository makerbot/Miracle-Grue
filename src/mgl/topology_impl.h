/* 
 * File:   topology_impl.h
 * Author: Dev
 *
 * Created on August 1, 2012, 10:37 AM
 */

#ifndef TOPOLOGY_IMPL_H
#define	TOPOLOGY_IMPL_H

#include "topology_decl.h"

namespace topo {

template<typename CT, typename VT>
bool operator==(const link_template<CT, VT>& a, 
		const link_template<CT, VT>& b){
	return a.get_from() == b.get_from() && a.get_to() == b.get_to();
}
template<typename CT, typename VT>
bool operator!=(const link_template<CT, VT>& a, 
		const link_template<CT, VT>& b){
	return !(a==b);
}

template<typename CT, typename VT>
node_template<CT, VT>::node_template(vector_type position){
	set_position(position);
}

template<typename CT, typename VT>
node_template<CT, VT>::~node_template(){
	break_inlinks();
	break_outlinks();
}

template<typename CT, typename VT>
typename node_template<CT, VT>::link_type* 
		node_template<CT, VT>::connect(node_template* other, cost_type cost){
	//is there already such a connection?
	for(iterator i = outlinks.begin(); i!=outlinks.end(); ++i){
		link_type* lp = *i;
		if(lp->get_to() == other){
			//yes there is, update its cost
			lp->set_cost(cost);
			return lp;		//no duplicates allowed
		}
	}
	//there is not such a connection existing
	link_type* lp = new link_type(this, other, cost);
	outlinks.push_back(lp);
	other->become_connected(lp);
	return lp;
}

template<typename CT, typename VT>
void node_template<CT, VT>::disconnect(node_template* other){
	//find the link that links to other
	for(iterator i = outlinks.begin(); i!=outlinks.end(); ++i){
		link_type* lp = *i;
		if(lp->get_to() == other){
			//remove it
			other->become_disconnected(lp);
			outlinks.erase(i);
			delete lp;
			return;		//no duplicates
		}
	}
}

template<typename CT, typename VT>
typename node_template<CT, VT>::vector_type 
		node_template<CT, VT>::get_position() const{
	return position;
}

template<typename CT, typename VT>
void node_template<CT, VT>::set_position(vector_type p){
	position = p;
}

template<typename CT, typename VT>
void node_template<CT, VT>::break_inlinks(){
	while(!inlinks.empty()){
		link_type* lp = inlinks.front();
		lp->get_from()->disconnect(this);
	}
}

template<typename CT, typename VT>
void node_template<CT, VT>::break_outlinks(){
	while(!outlinks.empty()){
		link_type* lp = outlinks.front();
		disconnect(lp->get_to());
	}
}

template<typename CT, typename VT>
void node_template<CT, VT>::become_connected(link_type* l){
	inlinks.push_back(l);
}

template<typename CT, typename VT>
void node_template<CT, VT>::become_disconnected(link_type* l){
	inlinks.remove(l);
}

template<typename CT, typename VT>
typename node_template<CT, VT>::iterator node_template<CT, VT>::inlinks_begin(){
	return inlinks.begin();
}

template<typename CT, typename VT>
typename node_template<CT, VT>::iterator 
		node_template<CT, VT>::outlinks_begin(){
	return outlinks.begin();
}

template<typename CT, typename VT>
typename node_template<CT, VT>::const_iterator 
		node_template<CT, VT>::inlinks_begin() const{
	return inlinks.begin();
}

template<typename CT, typename VT>
typename node_template<CT, VT>::const_iterator 
		node_template<CT, VT>::outlinks_begin() const{
	return outlinks.begin();
}

template<typename CT, typename VT>
typename node_template<CT, VT>::const_iterator 
		node_template<CT, VT>::inlinks_end() const{
	return inlinks.end();
}

template<typename CT, typename VT>
typename node_template<CT, VT>::const_iterator 
		node_template<CT, VT>::outlinks_end() const{
	return outlinks.end();
}

template<typename CT, typename VT>
typename link_template<CT, VT>::node_type* 
		link_template<CT, VT>::get_from() const{
	return from;
}

template<typename CT, typename VT>
typename link_template<CT, VT>::node_type* 
		link_template<CT, VT>::get_to() const{
	return to;
}

template<typename CT, typename VT>
void link_template<CT, VT>::set_from(node_type* f){
	from = f;
}

template<typename CT, typename VT>
void link_template<CT, VT>::set_to(node_type* t){
	to = t;
}

template<typename CT, typename VT>
typename link_template<CT, VT>::cost_type 
		link_template<CT, VT>::get_cost() const{
	return cost;
}

template<typename CT, typename VT>
void link_template<CT, VT>::set_cost(cost_type c){
	cost = c;
}



}



#endif	/* TOPOLOGY_IMPL_H */

