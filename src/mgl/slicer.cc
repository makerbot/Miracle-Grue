#include <vector>

#include "slicer.h"

using namespace mgl;


Slicer::Slicer(const SlicerConfig &slicerCfg, ProgressBar *progress)
	:Progressive(progress)
{
	layerCfg.firstLayerZ = slicerCfg.firstLayerZ;
	layerCfg.layerH = slicerCfg.layerH;
	layerCfg.layerW = slicerCfg.layerW;
	layerCfg.gridSpacingMultiplier = slicerCfg.gridSpacingMultiplier;
}
void Slicer::generateLoops(const Segmenter& seg, LayerLoops& layerloops) {
	unsigned int sliceCount = seg.readSliceTable().size();
	initProgress("outlines", sliceCount);
	
	layerloops.layerMeasure = seg.readLayerMeasure();
	
	for (size_t sliceId = 0; sliceId < sliceCount; sliceId++) {
		tick();
		LayerLoops::Layer currentLayer(layerloops.layerMeasure.createAttributes());
		layerloops.layerMeasure.getLayerAttributes(currentLayer.getIndex()) = 
				LayerMeasure::LayerAttributes(
				layerloops.layerMeasure.sliceIndexToHeight(sliceId), 
				layerloops.layerMeasure.getLayerH());
		libthing::SegmentTable segments;
		/*
		 Function outlinesForSlice is designed to use segmentTable rather than
		 the new Loop class. It makes use of clipper.cc, which was machine 
		 translated from Delphi, and is not currently practical to quickly 
		 convert to using new types. For this reason, we elected to 
		 use this function as is, and to convert its resulting SegmentTables
		 into lists of loops.
		 */
		outlinesForSlice(seg, sliceId, segments);
		//convert all SegmentTables into loops
		for(libthing::SegmentTable::iterator it = segments.begin();
				it != segments.end();
				++it){
			Loop currentLoop;
			Loop::cw_iterator iter = currentLoop.clockwiseEnd();
			//convert current SegmentTable into a loop
			for(std::vector<libthing::LineSegment2>::iterator it2 = it->begin(); 
					it2 != it->end(); 
					++it2){
				//add points 1 - N
				iter = currentLoop.insertPoint(it2->b, iter);
			}
			if(!it->empty())
				//add point 0
				iter = currentLoop.insertPoint(it->begin()->a, iter);
			//add the loop to the current layer
			currentLayer.push_back(currentLoop);
		}
		//finally, add the loop layer to the new data structure
		layerloops.push_back(currentLayer);
	}
	Scalar gridSpacing = layerCfg.layerW * layerCfg.gridSpacingMultiplier;
	Limits limits = seg.readLimits();
	layerloops.grid.init(limits, gridSpacing);
}



void Slicer::outlinesForSlice(const Segmenter& seg, size_t sliceId, libthing::SegmentTable & segments)
{
	Scalar tol = 1e-6;
	const LayerMeasure & layerMeasure = seg.readLayerMeasure();
	Scalar z = layerMeasure.sliceIndexToHeight(sliceId);
	const std::vector<libthing::Triangle3> & allTriangles = seg.readAllTriangles();
	const TriangleIndices & trianglesForSlice = seg.readSliceTable()[sliceId];
	std::vector<libthing::LineSegment2> unorderedSegments;
	segmentationOfTriangles(trianglesForSlice, allTriangles, z, unorderedSegments);
	assert(segments.size() ==0);

	// dumpSegments("unordered_", unorderedSegments);
	// cout << segments << endl;
	loopsFromLineSegments(unorderedSegments, tol, segments);
	// cout << " done " << endl;
}



void Slicer::loopsFromLineSegments(const std::vector<libthing::LineSegment2>& unorderedSegments, Scalar tol, libthing::SegmentTable & segments)
{
	// dumpSegments("unordered_", unorderedSegments);
	// cout << segments << endl;
	if(unorderedSegments.size() > 0){
		//cout << " loopsAndHoleOgy " << endl;
		std::vector<libthing::LineSegment2> segs =  unorderedSegments;
		loopsAndHoleOgy(segs, tol, segments);
	}
}
