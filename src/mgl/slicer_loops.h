/* 
 * File:   slicer_loops.h
 * Author: Dev
 *
 * Created on June 27, 2012, 3:02 PM
 */

#ifndef SLICER_LOOPS_H
#define	SLICER_LOOPS_H

#include "mgl.h"
#include "grid.h"
#include "loop_path.h"
#include <list>

namespace mgl {

class LayerLoops{	
public:
	class Layer;
	typedef std::list<Loop> LoopList;
	typedef std::list<Layer> LayerList;
	typedef LoopList::iterator loop_iterator;
	typedef LayerList::iterator layer_iterator;
	typedef LoopList::const_iterator const_loop_iterator;
	typedef LayerList::const_iterator const_layer_iterator;

	class Layer{
	public:
		Layer(layer_measure_index_t ind = 0);
		loop_iterator begin();
		const_loop_iterator begin() const;
		loop_iterator end();
		const_loop_iterator end() const;
		void push_back(const Loop& value);
		void push_front(const Loop& value);
		void pop_back();
		void pop_front();
		loop_iterator insert(loop_iterator at, const Loop& value);
		loop_iterator erase(loop_iterator at);
		loop_iterator erase(loop_iterator from, loop_iterator to);
		bool empty() const;
		const LoopList& readLoops() const;
		layer_measure_index_t getIndex() const;
	private:
		LoopList loops;
		layer_measure_index_t measure_index;
	};
	LayerLoops(Scalar firstLayerZ = 0.33, Scalar layerH = 0.27, Scalar layerW = 0.43);
	layer_iterator begin();
	const_layer_iterator begin() const;
	layer_iterator end();
	const_layer_iterator end() const;
	void push_back(const Layer& value);
	void push_front(const Layer& value);
	void pop_back();
	void pop_front();
	layer_iterator insert(layer_iterator at, const Layer& value);
	layer_iterator erase(layer_iterator at);
	layer_iterator erase(layer_iterator from, layer_iterator to);
	bool empty() const;
	
	const LayerList& readLayers() const;

	LayerMeasure layerMeasure;

	size_t size() const { return layers.size(); }
private:
	LayerList layers;
};

}


#endif	/* SLICER_LOOPS_H */

