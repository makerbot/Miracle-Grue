/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/
#ifndef MIRACLE_H_
#define MIRACLE_H_

#include "gcoder.h"


namespace mgl
{

void miracleGrue(GCoder &gcoder,
                      const Slicer &slicer,
                      const char *modelFile,
                      const char *scadFile,
                      const char *gcodeFile,
                      int firstSliceIdx,
                      int lastSliceIdx,
                      std::vector< SliceData >  &slices);

/*
bool slicesFromSlicerAndMesh(
		std::vector< SliceData >  &slices,
		std::vector<Scalar>& zIndicies,
		Slicer &slicer,
		Meshy& mesh,
		const char *scadFile,
		int firstSliceIdx  = -1,
		int lastSliceIdx  = -1);


bool writeGcodeFromSlicesAndParams(
		const char *gcodeFile, GCoder &gcoder,
		std::vector<SliceData >& slices, std::vector<Scalar>& zIndicies,
		const char *modelSource );

*/
};


#endif
