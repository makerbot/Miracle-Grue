/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/

#include "gcoder.h"

namespace mgl
{

// this is the entry point for slicing a model and writing a gcode /scad file
// it is used by the application and the high level tests
//
void miracleGrue(	GCoder &gcoder,
					const Slicer &slicer,
					const char *modelFile,
					const char *scadFile,
					const char *gcodeFile,
					int firstSlice,
					int lastSlice,
					std::vector< SliceData >  &slices);

void slicesFromSlicerAndParams(const char *modelFile,
								int firstSliceIdx,
								int lastSliceIdx,
								const Slicer &slicer,
								const char *scadFile,
								std::vector< SliceData >  &slices);



/*

void miracleGrue_split(	GCoder &gcoder,
					Slicer &slicer,
					const char *modelFile,
					const char *scadFile,
					const char *gcodeFile,
					int firstSlice,
					int lastSlice,
					std::vector< SliceData >  &slices);




void slicesFromSlicerAndParams(
		std::vector< SliceData >  &slices,
		std::vector<Scalar>& zIndicies,
		Slicer &slicer,
		int firstSliceIdx,
		int lastSliceIdx,
		const char *modelFile,
		const char *scadFile );


void writeGcodeFromSlicesAndParams(
		const char *gcodeFile, GCoder &gcoder,
		std::vector<SliceData >& slices, std::vector<Scalar>& zIndicies,
		const char *modelSource );

*/
}


