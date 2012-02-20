#include "miracle.h"

using namespace std;
using namespace mgl;


void mgl::miracleGrue(	GCoder &gcoder,
					Slicer &slicer,
					const char *modelFile,
					const char *scadFile,
					const char *gcodeFile)
{
	Meshy mesh(slicer.firstLayerZ, slicer.layerH); // 0.35
	loadMeshyFromStl(mesh, modelFile);

	unsigned int sliceCount = mesh.readSliceTable().size();
	unsigned int extruderId = 0;

	Slicy slicy(mesh.readAllTriangles(), mesh.readLimits(), slicer.layerW, slicer.layerH, sliceCount, scadFile);

	std::vector< SliceData >  slices;
	slices.reserve( mesh.readSliceTable().size());

	Scalar cuttOffLength = slicer.insetCuttOffMultiplier * slicer.layerW;

	ProgressBar progress(sliceCount);
	cout << "Slicing" << endl;

    std::ofstream gout(gcodeFile);
    gcoder.writeStartOfFile(gout);

	for(unsigned int sliceId=0; sliceId < sliceCount; sliceId++)
	{
		progress.tick();
        cout.flush();
		const TriangleIndices & trianglesForSlice = mesh.readSliceTable()[sliceId];
		Scalar z = mesh.readLayerMeasure().sliceIndexToHeight(sliceId);
		Scalar sliceAngle = sliceId * slicer.angle;
		slices.push_back( SliceData(z,sliceId));
		SliceData &slice = slices[sliceId];

		bool hazNewPaths = slicy.slice( trianglesForSlice,
										z,
										sliceId,
										extruderId,
										slicer.tubeSpacing,
										sliceAngle,
										slicer.nbOfShells,
										cuttOffLength,
										slicer.infillShrinkingMultiplier,
										slicer.insetDistanceMultiplier,
										slice);
		// cout << slice;
		if(hazNewPaths)
		{
			gcoder.writeSlice(gout, slice);
		}
		else
		{
	    	cout << "WARNING: Layer " << sliceId << " has no outline!" << endl;
//			slices.pop_back();
		}
	}
    gout.close();
}
