#include <list>

#include "slicer_loops.h"


namespace mgl {


LayerLoops::LayerLoops(Scalar firstLayerZ, Scalar layerH) : 
		layerMeasure(firstLayerZ, layerH) {}
LayerLoops::layer_iterator LayerLoops::begin(){
	return layers.begin();
}
LayerLoops::const_layer_iterator LayerLoops::begin() const{
	return layers.begin();
}
LayerLoops::layer_iterator LayerLoops::end(){
	return layers.end();
}
LayerLoops::const_layer_iterator LayerLoops::end() const{
	return layers.end();
}
void LayerLoops::push_back(const Layer& value){
	layers.push_back(value);
}
void LayerLoops::push_front(const Layer& value){
	layers.push_front(value);
}
void LayerLoops::pop_back(){
	layers.pop_back();
}
void LayerLoops::pop_front(){
	layers.pop_front();
}
LayerLoops::layer_iterator LayerLoops::insert(layer_iterator at, 
		const Layer& value){
	return layers.insert(at, value);
}
LayerLoops::layer_iterator LayerLoops::erase(layer_iterator at){
	return layers.erase(at);
}
LayerLoops::layer_iterator LayerLoops::erase(layer_iterator from, 
		layer_iterator to){
	return layers.erase(from, to);
}
bool LayerLoops::empty() const { return layers.empty(); }




}


