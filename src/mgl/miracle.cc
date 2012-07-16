

#include "configuration.h"
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
		const RegionerConfig& regionerCfg, 
 	    const ExtruderConfig &extruderCfg,
		const char *modelFile,
		const char *, // scadFileStr,
		ostream& gcodeFile,
		int firstSliceIdx,
		int lastSliceIdx,
		RegionList &regions,
		std::vector< SliceData > &slices,
		ProgressBar *progress) {

	Meshy mesh;
	mesh.readStlFile(modelFile);
	mesh.alignToPlate();

	Segmenter segmenter(slicerCfg.firstLayerZ, slicerCfg.layerH);
	segmenter.tablaturize(mesh);

	Slicer slicer(slicerCfg, progress);
	LayerLoops layerloops(slicerCfg.firstLayerZ, slicerCfg.layerH);

	//old interface
	//slicer.tomographyze(segmenter, tomograph);
	//new interface
	slicer.generateLoops(segmenter, layerloops);


	Regioner regioner(regionerCfg, progress);

	//old interface
	//regioner.generateSkeleton(tomograph, regions);
	//new interface
	regioner.generateSkeleton(layerloops, regions);

	Pather pather(progress);

	LayerPaths layers;
	pather.generatePaths(extruderCfg, layerloops, regions, layers);

	// pather.writeGcode(gcodeFileStr, modelFile, slices);
	//std::ofstream gout(gcodeFile);

	GCoder gcoder(gcoderCfg, progress);

	//old interface
	//	gcoder.writeGcodeFile(slices, layerloops.layerMeasure, gcodeFile, 
	//			modelFile, firstSliceIdx, lastSliceIdx);
	//new interface
	gcoder.writeGcodeFile(layers, layerloops.layerMeasure, gcodeFile, modelFile);

	//gout.close();

}




