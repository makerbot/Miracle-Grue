#include "miracle.h"

using namespace std;
using namespace mgl;


/**
 * Creates slices from the specified model file, and saves them
 * into the slices object
 *
 * @param slices - List of slices to write
 * @param zIndicides - list of zHeight of each slice in slices, indexed by position
 * @param slicer - instance of a slicer object
 * @param firstSliceIdx - for debugging, first slice to store into slices.
 * @param lastSliceIdx - for debugging, last slice to store into slices.
 * @param mesh - Mesh object
 * @param scadFile - debugging SCAD filename for debugging to scad file
 *
 */
bool mgl::slicesFromSlicerAndMesh(
		std::vector< SliceData >  &slices,
		std::vector<Scalar>& zIndicies,
		Slicer &slicer,
		Meshy& mesh,
		const char *scadFile,
		int firstSliceIdx,
		int lastSliceIdx )

		{
	assert(slices.size() ==0);

	unsigned int sliceCount = mesh.readSliceTable().size();
	unsigned int extruderId = 0;

	if(firstSliceIdx == -1) firstSliceIdx = 0;
    if(lastSliceIdx  == -1) lastSliceIdx = sliceCount-1;

	Slicy slicy(mesh.readAllTriangles(), mesh.readLimits(), slicer.layerW, slicer.layerH, sliceCount, scadFile);

	slices.reserve( mesh.readSliceTable().size());

	Scalar cuttOffLength = slicer.insetCuttOffMultiplier * slicer.layerW;

	ProgressBar progressSlice(sliceCount);
	cout << "Slicing" << endl;

	for(unsigned int sliceId=0; sliceId < sliceCount; sliceId++)
	{
		progressSlice.tick();

		if(sliceId <  firstSliceIdx) continue;
		if(sliceId > lastSliceIdx) break;

		const TriangleIndices & trianglesForSlice = mesh.readSliceTable()[sliceId];
		Scalar z = mesh.readLayerMeasure().sliceIndexToHeight(sliceId);
		Scalar sliceAngle = sliceId * slicer.angle;
		slices.push_back( SliceData(z,sliceId));
		zIndicies.push_back(Scalar(z));
		SliceData &slice = slices[sliceId];

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
	return true;
}


/**
 * Writes to gcodeFile via gcoder the slices and data specified by slices
 * @param gcodeFile - name out output gcode file
 * @param gcoder  - instance of a gcode object
 * @param slices - List of slices to write
 * @param zIndicides - list of zHeight of each slice in slices, indexed by position
 * @param modelSource - source of model data, usually the source .stl filename
 */
bool mgl::writeGcodeFromSlicesAndParams( const char *gcodeFile, GCoder &gcoder,
		std::vector<SliceData >& slices, std::vector<Scalar>& zIndicies,
		const char *modelSource )
{

	unsigned int sliceCount = slices.size();
	unsigned int adjustedSliceId = 0;
	assert(zIndicies.size() == slices.size());
	cout << "Writing gcode" << endl;
	ProgressBar progressGcode(sliceCount);


    std::ofstream gout(gcodeFile);
    gcoder.writeStartOfFile(gout, modelSource);

	for(unsigned int sliceId=0; sliceId < sliceCount; sliceId++)
	{
		progressGcode.tick();
		SliceData &slice = slices[sliceId];
		slice.sliceIndex = adjustedSliceId;
		slice.z = zIndicies[sliceId];
		gcoder.writeSlice(gout, slice);
		adjustedSliceId ++;
	}
    gout.close();
    return true;
}
