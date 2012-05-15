

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
                      const char *gcodeFile,
                      int firstSliceIdx,
                      int lastSliceIdx,
                      Tomograph &tomograph,
                      Regions &regions,
                      std::vector< SliceData >  &slices,
                      ProgressBar *progress)
{

	Slicer slicer(slicerCfg, progress);

	slicer.tomographyze(modelFile, tomograph);

	Regioner regioner(slicerCfg, progress);

	regioner.generateSkeleton(tomograph, regions);

	Pather pather(progress);

	pather.generatePaths(tomograph, regions, slices);

	// pather.writeGcode(gcodeFileStr, modelFile, slices);
	std::ofstream gout(gcodeFile);

        GCoder gcoder(gcoderCfg, progress);
    gcoder.writeGcodeFile(slices, tomograph.layerMeasure, gout, modelFile, firstSliceIdx, lastSliceIdx);

	gout.close();

}




