#include "miracle.h"

using namespace std;
using namespace mgl;


//// @param input
//// @param input
//// @param input
//// @param input
//// @param slices list of output slice (output )
void mgl::miracleGrue(GCoder &gcoder,
                      const Slicer &slicer,
                      const char *modelFile,
                      const char *scadFile,
                      const char *gcodeFile,
                      int firstSliceIdx,
                      int lastSliceIdx,
                      std::vector< SliceData >  &slices)
{
//
	assert(slices.size() ==0);
    Meshy mesh(slicer.firstLayerZ, slicer.layerH); // 0.35
    mesh.readStlFile(modelFile);

    slicesFromSlicerAndMesh(slices,slicer,mesh,scadFile,firstSliceIdx,lastSliceIdx);


	LayerMeasure zMeasure = mesh.readLayerMeasure();

	size_t first = 0,last= 0;
	if(firstSliceIdx > 0 ) {
		first  = firstSliceIdx;
	}

	if(lastSliceIdx == -1 || lastSliceIdx <= slices.size() ){
		last = slices.size()-1;
	}
	else{
		last = lastSliceIdx;
	}

	adjustSlicesToPlate(slices, zMeasure, first, last);

	writeGcodeFromSlicesAndParams(gcodeFile, gcoder, slices,  modelFile);



}


 
///
/// Creates slices from the specified model file, and saves them
/// into the slices object
///
/// @param slices - List of slices to write
/// @param zIndicides - list of zHeight of each slice in slices, indexed by position
/// @param slicer - instance of a slicer object
/// @param firstSliceIdx - for debugging, first slice to store into slices.
/// @param lastSliceIdx - for debugging, last slice to store into slices.
/// @param modelSource - source .stl filename
/// @param scadFile - debugging SCAD filename for debugging to scad file
void mgl::slicesFromSlicerAndMesh(
		std::vector< SliceData >  &slices,
		const Slicer &slicer,
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

	Slicy slicy(mesh.readAllTriangles(), mesh.readLimits(),
			slicer.layerW, slicer.layerH, sliceCount, scadFile);

	/// for future use of multithreading, we need to create slices to not lock
	/// the structure in the loop
    slices.reserve( mesh.readSliceTable().size());
    for(unsigned int sliceId=0; sliceId < sliceCount; sliceId++)
    {
		Scalar z = mesh.readLayerMeasure().sliceIndexToHeight(sliceId);
		slices.push_back( SliceData(z,sliceId) );

    }

	Scalar cuttOffLength = slicer.insetCuttOffMultiplier * slicer.layerW;

	ProgressBar progressSlice(sliceCount);
	cout << "Slicing" << endl;

	for(unsigned int sliceId=0; sliceId < sliceCount; sliceId++)
	{
		progressSlice.tick();

		if(sliceId <  firstSliceIdx) continue;
		if(sliceId > lastSliceIdx) break;

		const TriangleIndices & trianglesForSlice = mesh.readSliceTable()[sliceId];
		Scalar sliceAngle = sliceId * slicer.angle;
		SliceData &slice = slices[sliceId];

		slicy.slice(	trianglesForSlice,
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
}


///
/// Does an inplcae update to the passed slices to adjust/move them
/// so the lowest layer is a z height zero
/// @param slices slices to adjust
/// @param layerMeasure class for layer measurements in Z plane
/// @param firstSliceIdx - 'bottom' of slice section to transport down
/// @param lastSliceIdx - 'top' of slice section to transport down
///
void mgl::adjustSlicesToPlate(
		std::vector<SliceData>& slices,
		const LayerMeasure& layerMeasure,
		size_t firstSliceIdx, size_t lastSliceIdx)
{
	/// remove unwanted slices at the end of the vector

//	slices.erase(slices.begin()+ lastSliceIdx, slices.end());
//	/// remove unwanted slices at the start of the vector
//	slices.erase(slices.begin(),slices.begin() + firstSliceIdx-1);
	size_t sliceCounter = 0;

	for(size_t sliceId = firstSliceIdx; sliceId < lastSliceIdx; sliceId++, sliceCounter++)
	{
		Scalar adjustedZ = layerMeasure.sliceIndexToHeight(sliceCounter);
		// cout << " slice info: " << slices[sliceId].getIndex() << " " << slices[sliceId].getZHeight() << endl;
		slices[sliceId].updatePosition(adjustedZ, sliceId );
		sliceCounter ++;
	}
}



 /// Writes to gcodeFile via gcoder the slices and data specified by slices
 /// @param gcodeFile - name out output gcode file
 /// @param gcoder  - instance of a gcode object
 /// @param slices - List of slices to write
 /// @param modelSource - source of model data, usually the source .stl filename
void mgl::writeGcodeFromSlicesAndParams(
		const char *gcodeFile,
		GCoder &gcoder,
		std::vector<SliceData >& slices,
		const char *modelSource  )
{

	assert(slices.size() != 0);
	assert(gcodeFile != 0x00);
	assert(modelSource != 0x00);
	size_t sliceCount = slices.size();

	cout << "Writing gcode" << endl;
	ProgressBar progressGcode(sliceCount);

    std::ofstream gout(gcodeFile);
    gcoder.writeStartOfFile(gout, modelSource);

	for(size_t sliceId=0; sliceId < sliceCount; sliceId++)
	{
		progressGcode.tick();
		SliceData &slice = slices[sliceId];
		gcoder.writeSlice(gout, slice);
	}
    gout.close();
}

