

#include "JsonConverter.h"
#include <json/writer.h>

// #include "abstractable.h"
#include "miracle.h"

using namespace std;
using namespace mgl;
using namespace Json;
using namespace libthing;


class Slicinator
{
	Regioner regioner;
	Pather slicor;

	ModelSkeleton skeleton;
//	std::vector< SliceData >  slices;

public:
	Slicinator(	GCoder &gcoderCfg,
            	const SlicerConfig &slicerCfg)
	{
	}

	void slice(const char *modelFileStr, std::vector< SliceData >  &slices, ProgressBar &progress)
	{

	}

	void writeGcode(ostream& output)
	{

	}

};

//// @param slices list of output slice (output )
void mgl::miracleGrue(GCoder &gcoder,
                      const SlicerConfig &slicerCfg,
                      const char *modelFile,
                      const char *scadFileStr,
                      const char *gcodeFile,
                      int firstSliceIdx,
                      int lastSliceIdx,
                      ModelSkeleton &skeleton,
                      std::vector< SliceData >  &slices,
                      ProgressBar *progress)
{

	ProgressLog log;
	if(!progress)
	{
		progress = &log;
	}

	unsigned int roofLayerCount = 3;
	unsigned int floorLayerCount = 3;
	unsigned int skipCount = 2;

	Regioner regioner;
	regioner.init(slicerCfg, 0.95,
					roofLayerCount,
					floorLayerCount,
					skipCount,
					progress);

	regioner.generateSkeleton(modelFile, skeleton);

	Pather pather;
	pather.init(progress);
	pather.generatePaths(skeleton, slices);

	// pather.writeGcode(gcodeFileStr, modelFile, slices);

	std::ofstream gout(gcodeFile);
	gcoder.writeStartOfFile(gout, modelFile);

	size_t sliceCount = slices.size();
	if(firstSliceIdx <0)firstSliceIdx = 0;
	if(lastSliceIdx <0)lastSliceIdx = sliceCount -1;


	initProgress("gcode", sliceCount);
	size_t codeSlice = 0;
	for(size_t sliceId=0; sliceId < sliceCount; sliceId++)
	{
		tick();
		if(sliceId < firstSliceIdx) continue;
		if(sliceId > lastSliceIdx) break;

		Scalar z = skeleton.layerMeasure.sliceIndexToHeight(codeSlice);
		SliceData &slice = slices[sliceId];
		slice.updatePosition(z, sliceId);

		gcoder.writeSlice(gout, slice);
		codeSlice ++;
	}
	gout.close();

}




