

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
                      const char *scadFileStr,
                      const char *gcodeFile,
                      int firstSliceIdx,
                      int lastSliceIdx,
                      Regions &regions,
                      std::vector< SliceData >  &slices,
                      ProgressBar *progress)
{

	unsigned int roofLayerCount = 3;
	unsigned int floorLayerCount = 3;
	unsigned int skipCount = 2;

	Regioner regioner(slicerCfg, 0.95,
					roofLayerCount,
					floorLayerCount,
					skipCount,
					progress);

	regioner.generateSkeleton(modelFile, regions);

	Pather pather(progress);

	pather.generatePaths(regions, slices);

	// pather.writeGcode(gcodeFileStr, modelFile, slices);
	std::ofstream gout(gcodeFile);

	GCoder gcoder(gcoderCfg);
    gcoder.writeGcodeFile(slices, regions.layerMeasure, gout, modelFile, firstSliceIdx, lastSliceIdx);

	gout.close();

}




