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


void Slicer::tomographyze( const char* modelFile, Tomograph &tomograph)
{
	Meshy mesh(layerCfg.firstLayerZ, layerCfg.layerH);
	mesh.readStlFile(modelFile);

	// grid.init(mesh.limits, slicerCfg.layerW);
	unsigned int sliceCount = mesh.readSliceTable().size();
	tomograph.outlines.resize(sliceCount);

	initProgress("outlines", sliceCount);


	for(size_t sliceId =0; sliceId < sliceCount; sliceId++)
	{
		tick();
		//cout << sliceId << "/" << sliceCount << " outlines" << endl;
		libthing::SegmentTable &segments = tomograph.outlines[sliceId];
		outlinesForSlice(mesh, sliceId, segments);
	}

	Scalar gridSpacing = layerCfg.layerW * layerCfg.gridSpacingMultiplier;
	Limits limits = mesh.readLimits();
	tomograph.grid.init(limits, gridSpacing);
	tomograph.layerMeasure = mesh.readLayerMeasure();
}

void Slicer::outlinesForSlice(const Meshy & mesh, size_t sliceId, libthing::SegmentTable & segments)
{
	Scalar tol = 1e-6;
	const LayerMeasure & layerMeasure = mesh.readLayerMeasure();
	Scalar z = layerMeasure.sliceIndexToHeight(sliceId);
	const std::vector<libthing::Triangle3> & allTriangles = mesh.readAllTriangles();
	const TriangleIndices & trianglesForSlice = mesh.readSliceTable()[sliceId];
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
