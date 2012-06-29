

#include "JsonConverter.h"
#include <json/writer.h>

// #include "abstractable.h"
#include "miracle.h"

using namespace std;
using namespace mgl;
using namespace Json;
using namespace libthing;


//// @param slices list of output slice (output )
void mgl::miracleGrue(const GCoderConfig &gcoderCfg,
                      const SlicerConfig &slicerCfg,
                      const char *modelFile,
                      const char *, // scadFileStr,
                      ostream& gcodeFile,
                      int firstSliceIdx,
                      int lastSliceIdx,
                      Tomograph &tomograph,
                      Regions &regions,
                      std::vector< SliceData >  &slices,
                      ProgressBar *progress)
{

	Meshy mesh;
	mesh.readStlFile(modelFile);
	mesh.alignToPlate();
	
	Segmenter segmenter(slicerCfg.firstLayerZ, slicerCfg.layerH);
	segmenter.tablaturize(mesh);

	Slicer slicer(slicerCfg, progress);
	LayerLoops layerloops(slicerCfg.firstLayerZ, slicerCfg.layerH);

	//old interface
	slicer.tomographyze(segmenter, tomograph);
	//new interface
	slicer.generateLoops(segmenter, layerloops);
	

	Regioner regioner(slicerCfg, progress);

	regioner.generateSkeleton(tomograph, regions);

	Pather pather(progress);

	LayerPaths layers;
	pather.generatePaths(layerloops, regions, layers);

	// pather.writeGcode(gcodeFileStr, modelFile, slices);
	//std::ofstream gout(gcodeFile);

    GCoder gcoder(gcoderCfg, progress);
    gcoder.writeGcodeFile(slices, tomograph.layerMeasure, gcodeFile, modelFile, firstSliceIdx, lastSliceIdx);

	//gout.close();

}




