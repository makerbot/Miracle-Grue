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



void Slicer::tomographyze( Segmenter& seg, Tomograph &tomograph)
{
	// grid.init(mesh.limits, slicerCfg.layerW);
	unsigned int sliceCount = seg.readSliceTable().size();
	tomograph.outlines.resize(sliceCount);

	initProgress("outlines", sliceCount);


	for(size_t sliceId =0; sliceId < sliceCount; sliceId++)
	{
		tick();
		//cout << sliceId << "/" << sliceCount << " outlines" << endl;
		libthing::SegmentTable &segments = tomograph.outlines[sliceId];
		outlinesForSlice(seg, sliceId, segments);
	}

	Scalar gridSpacing = layerCfg.layerW * layerCfg.gridSpacingMultiplier;
	Limits limits = seg.readLimits();
	tomograph.grid.init(limits, gridSpacing);
	tomograph.layerMeasure = seg.readLayerMeasure();
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
