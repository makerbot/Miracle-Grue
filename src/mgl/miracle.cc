#include "miracle.h"

using namespace std;
using namespace mgl;




void mgl::miracleGrue(GCoder &gcoder,
                      const Slicer &slicer,
                      const char *modelFile,
                      const char *scadFile,
                      const char *gcodeFile,
                      int firstSliceIdx,
                      int lastSliceIdx,
                      std::vector< SliceData >  &slices)
{
    assert(slices.size() ==0);
    Meshy mesh(slicer.firstLayerZ, slicer.layerH); // 0.35
    loadMeshyFromStl(mesh, modelFile);

    unsigned int sliceCount = mesh.readSliceTable().size();
    unsigned int extruderId = 0;

    Slicy slicy(mesh.readAllTriangles(), mesh.readLimits(), slicer.layerW, slicer.layerH, sliceCount, scadFile);
    Scalar cuttOffLength = slicer.insetCuttOffMultiplier * slicer.layerW;

    ProgressBar progressSlice(sliceCount);
    cout << "Slicing" << endl;
    if(firstSliceIdx == -1) firstSliceIdx = 0;
    if(lastSliceIdx  == -1) lastSliceIdx = sliceCount-1;


    slices.reserve( mesh.readSliceTable().size());
    for(unsigned int sliceId=0; sliceId < sliceCount; sliceId++)
    {
        slices.push_back( SliceData() );
    }

    for(unsigned int sliceId=0; sliceId < sliceCount; sliceId++)
    {
        SliceData &slice = slices[sliceId];

        progressSlice.tick();

        const TriangleIndices & trianglesForSlice = mesh.readSliceTable()[sliceId];
        Scalar sliceAngle = sliceId * slicer.angle;

        if(sliceId <  firstSliceIdx) continue;
        if(sliceId > lastSliceIdx) continue;
        Scalar z = mesh.readLayerMeasure().sliceIndexToHeight(sliceId);

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

    LayerMeasure zMeasure = mesh.readLayerMeasure();

    cout << "Writing gcode" << endl;
    std::ofstream gout(gcodeFile);
    gcoder.writeStartOfFile(gout, modelFile);

    ProgressBar progressCode(sliceCount);
    unsigned int adjustedSliceId = 0;
    for(unsigned int sliceId=0; sliceId < sliceCount; sliceId++)
    {
        progressCode.tick();
        SliceData &slice = slices[sliceId];

        if(sliceId <  firstSliceIdx) continue;
        if(sliceId > lastSliceIdx) continue;

        // slice.sliceIndex = adjustedSliceId;
        Scalar z = zMeasure.sliceIndexToHeight(adjustedSliceId);
        gcoder.writeSlice(gout, slice, z, adjustedSliceId);
        adjustedSliceId ++;
    }
    gout.close();

}



/*


inputs that are invariant


void mgl::miracleGrue_split(	GCoder &gcoder,
					Slicer &slicer,
					const char *modelFile,
					const char *scadFile,
					const char *gcodeFile,
					int firstSliceIdx,
					int lastSliceIdx,
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

    if(firstSliceIdx == -1) firstSliceIdx = 0;
    if(lastSliceIdx  == -1) lastSliceIdx = sliceCount-1;

	for(unsigned int sliceId=0; sliceId < sliceCount; sliceId++)
	{
		progressSlice.tick();

		const TriangleIndices & trianglesForSlice = mesh.readSliceTable()[sliceId];
		Scalar z = mesh.readLayerMeasure().sliceIndexToHeight(sliceId);
		Scalar sliceAngle = sliceId * slicer.angle;
		slices.push_back( SliceData(z,sliceId));
		SliceData &slice = slices[sliceId];

		if(sliceId <  firstSliceIdx) continue;
		if(sliceId > lastSliceIdx) continue;

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
	cout << "Writing gcode" << endl;
	ProgressBar progressGcode(sliceCount);
	unsigned int adjustedSliceId = 0;

    std::ofstream gout(gcodeFile);
    gcoder.writeStartOfFile(gout, modelFile);

	for(unsigned int sliceId=0; sliceId < sliceCount; sliceId++)
	{
		progressGcode.tick();
		SliceData &slice = slices[sliceId];

		if(sliceId <  firstSliceIdx) continue;
		if(sliceId > lastSliceIdx) continue;

		slice.sliceIndex = adjustedSliceId;
		slice.z = mesh.readLayerMeasure().sliceIndexToHeight(adjustedSliceId);
		gcoder.writeSlice(gout, slice);
		adjustedSliceId ++;
	}
    gout.close();
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
bool mgl::slicesFromSlicerAndParams(
		std::vector< SliceData >  &slices,
		std::vector<Scalar>& zIndicies,
		Slicer &slicer,
		int firstSliceIdx,
		int lastSliceIdx,
		const char *modelFile,
		const char *scadFile)

		{
	assert(slices.size() ==0);

	Meshy mesh(slicer.firstLayerZ, slicer.layerH); // 0.35
	loadMeshyFromStl(mesh, modelFile);


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


 /// Writes to gcodeFile via gcoder the slices and data specified by slices
 /// @param gcodeFile - name out output gcode file
 /// @param gcoder  - instance of a gcode object
 /// @param slices - List of slices to write
 /// @param zIndicides - list of zHeight of each slice in slices, indexed by position
 /// @param modelSource - source of model data, usually the source .stl filename
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
*/
