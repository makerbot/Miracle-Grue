/* 
 * File:   topology_decl.h
 * Author: Dev
 *
 * Created on August 1, 2012, 10:36 AM
 */

#ifndef TOPOLOGY_DECL_H
#define	TOPOLOGY_DECL_H

#include <list>
#include <vector>

namespace topo {

template<typename CT , typename VT>
class node_template;

template<typename CT, typename VT>
class link_template;

template<typename CT, typename VT>
struct topo_template{
	typedef CT cost_type;
	typedef VT vector_type;
	typedef node_template<CT, VT> node_type;
	typedef link_template<CT, VT> link_type;
};

template<typename CT, typename VT>
class link_template {
public:
	
	typedef typename topo_template<CT, VT>::node_type node_type;
	typedef typename topo_template<CT, VT>::cost_type cost_type;
	
	link_template(node_type* f, node_type* t, CT c = 0) : 
			from(f), to(t), cost(c) {}
	~link_template() {}
	node_type* get_from() const;
	node_type* get_to() const;
	void set_from(node_type* f);
	void set_to(node_type* t);
	cost_type get_cost() const;
	void set_cost(cost_type c);
protected:
	node_type* from;
	node_type* to;
	typename node_type::cost_type cost;
};

template<typename CT, typename VT>
class node_template{
public:
	
	typedef typename topo_template<CT, VT>::cost_type cost_type;
	typedef typename topo_template<CT, VT>::vector_type vector_type;
	typedef typename topo_template<CT, VT>::link_type link_type;
	
	typedef std::vector<link_type*> llp;
	typedef typename llp::iterator iterator;
	typedef typename llp::const_iterator const_iterator;
	
	node_template(vector_type position = vector_type());
	~node_template();
	link_type* connect(node_template* other, cost_type cost);
	void disconnect(node_template* other);
	vector_type get_position() const;
	void set_position(vector_type p);
	
	iterator inlinks_begin();
	const_iterator inlinks_begin() const;
	iterator outlinks_begin();
	const_iterator outlinks_begin() const;
	const_iterator inlinks_end() const;
	const_iterator outlinks_end() const;
	
	size_t inlinks_size() const;
	size_t outlinks_size() const;
	
	void break_inlinks();
	void break_outlinks();
protected:
	void become_connected(link_type* l);
	void become_disconnected(link_type* l);
	
	vector_type position;
	
	llp outlinks;
	/*
	 Outlinks are reachable from this node.
	 Pointers owned by this node
	 */
	llp inlinks;
	/*
	 Inlinks are nodes from which this one
	 can be reached. Pointers owned by those
	 nodes, not this one.
	 */
private:
	node_template(const node_template& other);	//no copy!
	node_template& operator=(const node_template& other);	//no assignment!
};

template<typename CT, typename VT>
bool operator==(const link_template<CT, VT>& a, 
		const link_template<CT, VT>& b);

template<typename CT, typename VT>
bool operator!=(const link_template<CT, VT>& a, 
		const link_template<CT, VT>& b);

}



#endif	/* TOPOLOGY_DECL_H */

