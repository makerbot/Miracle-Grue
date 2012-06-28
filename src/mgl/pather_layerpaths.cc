#include "pather.h"

namespace mgl {


LayerPaths::layer_iterator LayerPaths::begin(){
	return layers.begin();
}
LayerPaths::const_layer_iterator LayerPaths::begin() const{
	return layers.begin();
}
LayerPaths::layer_iterator LayerPaths::end(){
	return layers.end();
}
LayerPaths::const_layer_iterator LayerPaths::end() const{
	return layers.end();
}
void LayerPaths::push_back(const Layer& value){
	layers.push_back(value);
}
void LayerPaths::push_front(const Layer& value){
	layers.push_front(value);
}
void LayerPaths::pop_back(){
	layers.pop_back();
}
void LayerPaths::pop_front(){
	layers.pop_front();
}
LayerPaths::layer_iterator LayerPaths::insert(layer_iterator at, 
		const Layer& value){
	return layers.insert(at, value);
}
LayerPaths::layer_iterator LayerPaths::erase(layer_iterator at){
	return layers.erase(at);
}
LayerPaths::layer_iterator LayerPaths::erase(layer_iterator from, 
		layer_iterator to){
	return layers.erase(from, to);
}
bool LayerPaths::empty() const { return layers.empty(); }


}
