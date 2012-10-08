

#include "configuration.h"
#include <json/writer.h>

// #include "abstractable.h"
#include "miracle.h"

using namespace std;
using namespace mgl;
using namespace Json;
using namespace libthing;


//// @param slices list of output slice (output )

void mgl::miracleGrue(const GrueConfig& grueCfg, 
		const char *modelFile,
		const char *, // scadFileStr,
		ostream& gcodeFile,
		int, // firstSliceIdx,
		int, // lastSliceIdx,
		RegionList &regions,
		std::vector< SliceData >&, // slices,
		ProgressBar *progress) {

	Meshy mesh;
	mesh.readStlFile(modelFile);
	mesh.alignToPlate();
	
	Limits limits = mesh.readLimits();
	Grid grid;

	Segmenter segmenter(grueCfg);
	segmenter.tablaturize(mesh);

	Slicer slicer(grueCfg, progress);
	LayerLoops layerloops(grueCfg.get_firstLayerZ(), grueCfg.get_layerH());

	//old interface
	//slicer.tomographyze(segmenter, tomograph);
	//new interface
	slicer.generateLoops(segmenter, layerloops);
    
    LayerLoops processedLoops;
    
    LoopProcessor processor(grueCfg, progress);
    processor.processLoops(layerloops, processedLoops);
    
    LayerMeasure& layerMeasure = processedLoops.layerMeasure;


	Regioner regioner(grueCfg, progress);

	//old interface
	//regioner.generateSkeleton(tomograph, regions);
	//new interface
	regioner.generateSkeleton(processedLoops, layerMeasure, regions ,
			limits, grid);

	Pather pather(grueCfg, progress);

	LayerPaths layers;
	pather.generatePaths(grueCfg, regions,
						 layerloops.layerMeasure, grid, layers);

	// pather.writeGcode(gcodeFileStr, modelFile, slices);
	//std::ofstream gout(gcodeFile);

	GCoder gcoder(grueCfg, progress);

	//old interface
	//	gcoder.writeGcodeFile(slices, layerloops.layerMeasure, gcodeFile, 
	//			modelFile, firstSliceIdx, lastSliceIdx);
	//new interface
	gcoder.writeGcodeFile(layers, layerMeasure, 
			gcodeFile, modelFile);

	//gout.close();

}




