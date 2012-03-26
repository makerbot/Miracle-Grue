#include "miracle.h"

using namespace std;
using namespace mgl;


void mgl::miracleGrue(	GCoder &gcoder,
					Slicer &slicer,
					const char *modelFile,
					const char *scadFile,
					const char *gcodeFile,
					int firstSlice,
					int lastSlice,
					std::vector< SliceData >  &slices)
{
	assert(slices.size() ==0);

	Meshy mesh(slicer.firstLayerZ, slicer.layerH); // 0.35
	loadMeshyFromStl(mesh, modelFile);

	unsigned int sliceCount = mesh.readSliceTable().size();
	unsigned int extruderId = 0;

	Slicy slicy(mesh.readAllTriangles(), mesh.readLimits(), slicer.layerW, slicer.layerH, sliceCount, scadFile);


	slices.reserve( mesh.readSliceTable().size());

	Scalar cuttOffLength = slicer.insetCuttOffMultiplier * slicer.layerW;

	ProgressBar progressSlice(sliceCount);
	cout << "Slicing" << endl;

    std::ofstream gout(gcodeFile);
    gcoder.writeStartOfFile(gout, modelFile);

    if(firstSlice == -1) firstSlice = 0;
    if(lastSlice  == -1) lastSlice = sliceCount-1;


	for(unsigned int sliceId=0; sliceId < sliceCount; sliceId++)
	{
		progressSlice.tick();

		const TriangleIndices & trianglesForSlice = mesh.readSliceTable()[sliceId];
		Scalar z = mesh.readLayerMeasure().sliceIndexToHeight(sliceId);
		Scalar sliceAngle = sliceId * slicer.angle;
		slices.push_back( SliceData(z,sliceId));
		SliceData &slice = slices[sliceId];

		if(sliceId <  firstSlice) continue;
		if(sliceId > lastSlice) continue;

		slicy.slice(	trianglesForSlice,
						z,
						sliceId,
						extruderId,
						slicer.tubeSpacing,
						sliceAngle,
						slicer.nbOfShells,
						cuttOffLength,
						slicer.infillShrinkingMultiplier,
						slicer.insetDistanceMultiplier,
						slicer.writeDebugScadFiles,
						slice);
	}

	ProgressBar progressGcode(sliceCount);
	unsigned int adjustedSliceId = 0;
	for(unsigned int sliceId=0; sliceId < sliceCount; sliceId++)
	{
		progressGcode.tick();
		SliceData &slice = slices[sliceId];

		if(sliceId <  firstSlice) continue;
		if(sliceId > lastSlice) continue;

		slice.sliceIndex = adjustedSliceId;
		slice.z = mesh.readLayerMeasure().sliceIndexToHeight(adjustedSliceId);
		gcoder.writeSlice(gout, slice);
		adjustedSliceId ++;
	}
    gout.close();
}
