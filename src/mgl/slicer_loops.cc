#include <list>

#include "slicer_loops.h"


namespace mgl {

LayerLoops::loop_iterator LayerLoops::Layer::begin(){
	return loops.begin();
}
LayerLoops::const_loop_iterator LayerLoops::Layer::begin() const{
	return loops.begin();
}
LayerLoops::loop_iterator LayerLoops::Layer::end(){
	return loops.end();
}
LayerLoops::const_loop_iterator LayerLoops::Layer::end() const{
	return loops.end();
}
void LayerLoops::Layer::push_back(const Loop& value){
	loops.push_back(value);
}
void LayerLoops::Layer::push_front(const Loop& value){
	loops.push_front(value);
}
void LayerLoops::Layer::pop_back(){
	loops.pop_back();
}
void LayerLoops::Layer::pop_front(){
	loops.pop_front();
}
LayerLoops::loop_iterator LayerLoops::Layer::insert(loop_iterator at, 
		const Loop& value){
	return loops.insert(at, value);
}
LayerLoops::loop_iterator LayerLoops::Layer::erase(loop_iterator at){
	return loops.erase(at);
}
LayerLoops::loop_iterator LayerLoops::Layer::erase(loop_iterator from, 
		loop_iterator to){
	return loops.erase(from, to);
}
bool LayerLoops::Layer::empty() const { return loops.empty(); }

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


