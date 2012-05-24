/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

**/

#include "regioner.h"

using namespace mgl;
using namespace std;

Regioner::Regioner(const SlicerConfig &slicerCfg, ProgressBar *progress)
	:Progressive(progress), slicerCfg(slicerCfg)
{
	// move that to its own config element?
	roofLengthCutOff = 0.5 * slicerCfg.layerH;

}



void Regioner::generateSkeleton(const Tomograph &tomograph , Regions &regions)
{
//		outlines(	modelFile,
//					regions.layerMeasure,
//					regions.grid,
//					regions.outlines);

	insets(tomograph.outlines,
				  regions.insets);

	flatSurfaces(regions.insets,
			tomograph.grid,
						regions.flatSurfaces);

	roofing(regions.flatSurfaces, tomograph.grid, regions.roofings);

	flooring(regions.flatSurfaces, tomograph.grid, regions.floorings);

	infills( regions.flatSurfaces,
			tomograph.grid,
					regions.roofings,
					regions.floorings,
					regions.solids,
					regions.sparses,
					regions.infills);
}


void Regioner::insetsForSlice(const libthing::SegmentTable &sliceOutlines,
					libthing::Insets &sliceInsets, const char*scadFile)
{


	bool writeDebugScadFiles = false;
	inshelligence(sliceOutlines,
					slicerCfg.nbOfShells,
					slicerCfg.layerW,
					slicerCfg.insetDistanceMultiplier,
					scadFile,
					writeDebugScadFiles,
					sliceInsets);
}

void Regioner::insets(const std::vector<libthing::SegmentTable> & outlinesSegments, std::vector<libthing::Insets> & insets)
{

	unsigned int sliceCount = outlinesSegments.size();
	initProgress("insets", sliceCount);
	insets.resize(sliceCount);
	// slice id must be adjusted for
	for(size_t i = 0;i < sliceCount;i++)
	{
		tick();
		const libthing::SegmentTable & sliceOutlines = outlinesSegments[i];
		libthing::Insets & sliceInsets = insets[i];

		insetsForSlice(sliceOutlines, sliceInsets);
	}
}


void Regioner::flatSurfaces(	const std::vector<libthing::Insets> & insets,
					const Grid & grid,
					std::vector<GridRanges> & gridRanges)
{
	assert(gridRanges.size() == 0);
	unsigned int sliceCount = insets.size();
	initProgress("flat surfaces", sliceCount);
	gridRanges.resize(sliceCount);
	for(size_t i = 0;i < sliceCount;i++)
	{
		tick();
		const libthing::Insets & allInsetsForSlice = insets[i];
		GridRanges & surface = gridRanges[i];
		gridRangesForSlice(allInsetsForSlice, grid, surface);
	}
}

void Regioner::floorForSlice( const GridRanges & currentSurface,
								const GridRanges & surfaceBelow,
									const Grid & grid,
										GridRanges & flooring)
{
	GridRanges floor;
	grid.gridRangeDifference(currentSurface, surfaceBelow, floor);
	grid.trimGridRange(floor, roofLengthCutOff, flooring);
}

void Regioner::roofForSlice( const GridRanges & currentSurface, const GridRanges & surfaceAbove, const Grid & grid, GridRanges & roofing)
{
	GridRanges  roof;
	grid.gridRangeDifference(currentSurface, surfaceAbove, roof);
	grid.trimGridRange(roof, this->roofLengthCutOff, roofing );
}

void Regioner::roofing(const std::vector<GridRanges> & flatSurfaces, const Grid & grid, std::vector<GridRanges> & roofings)
{
	assert(flatSurfaces.size() > 0);
	assert(roofings.size() == 0);
	unsigned int sliceCount = flatSurfaces.size();
	initProgress("roofing", sliceCount);

	roofings.resize(sliceCount);
	for(size_t i = 0;i < sliceCount-1; i++)
	{
		tick();
		const GridRanges & currentSurface = flatSurfaces[i];
		const GridRanges & surfaceAbove = flatSurfaces[i + 1];
		GridRanges & roofing = roofings[i];

		GridRanges roof;
		roofForSlice(currentSurface, surfaceAbove, grid, roof);

		grid.trimGridRange(roof, this->roofLengthCutOff, roofing );

	}
	tick();
	roofings[sliceCount -1] = flatSurfaces[sliceCount -1];

}



void Regioner::flooring(const std::vector<GridRanges> & flatSurfaces, const Grid & grid, std::vector<GridRanges> & floorings)
{
	assert(flatSurfaces.size() > 0);
	assert(floorings.size() == 0);
	unsigned int sliceCount = flatSurfaces.size();
	initProgress("flooring", sliceCount);

	floorings.resize(sliceCount);
	for(size_t i = 1; i < sliceCount; i++)
	{
		tick();
		const GridRanges & currentSurface = flatSurfaces[i];
		const GridRanges & surfaceBelow = flatSurfaces[i - 1];
		GridRanges & flooring = floorings[i];
		floorForSlice(currentSurface, surfaceBelow, grid, flooring);
	}
	tick();
	floorings[0] = flatSurfaces[0];

}

void Regioner::infills(const std::vector<GridRanges> &flatSurfaces,
			 const Grid &grid,
			 const std::vector<GridRanges> &roofings,
			 const std::vector<GridRanges> &floorings,
			 std::vector<GridRanges> &solids,
			 std::vector<GridRanges> &sparses,
			 std::vector<GridRanges> &infills)
{


	assert(flatSurfaces.size() > 0);
	assert(roofings.size() > 0);

	unsigned int sliceCount = flatSurfaces.size();

	assert(infills.size() == 0);
	infills.resize(sliceCount);

	assert(sparses.size() == 0);
	sparses.resize(sliceCount);

	assert(solids.size() == 0);
	solids.resize(sliceCount);

	initProgress("infills", sliceCount);


	for(size_t i=0; i< sliceCount; i++)
	{
		const GridRanges &surface = flatSurfaces[i];
		tick();
		{
			const GridRanges &surface = flatSurfaces[i];
			const GridRanges &roofing = roofings[i];
			const GridRanges &flooring = floorings[i];
			GridRanges sparseInfill;

			size_t infillSkipCount = (int)(1/slicerCfg.infillDensity) - 1;
			grid.subSample(surface, infillSkipCount, sparseInfill);

			GridRanges roofed;
			grid.gridRangeUnion(sparseInfill, roofing, roofed);
			GridRanges &infill = infills[i];
			grid.gridRangeUnion(roofed, flooring, infill);
		}

		// Solids
		GridRanges solid;

		solid.xRays.resize(surface.xRays.size());
		solid.yRays.resize(surface.yRays.size());

		size_t firstFloor = 0;
		int f = i - this->slicerCfg.floorLayerCount;
		if(f > 0) firstFloor = (size_t)f;

		size_t lastRoof = i + this->slicerCfg.roofLayerCount;
		if(lastRoof > (sliceCount-1) ) lastRoof = sliceCount - 1;

		//cout << "Solid [" << i << "]  = sum (";
		//cout << " floor(";
		for (size_t j = (size_t)firstFloor; j <=  i; j++)
		{
			GridRanges multiFloor;
			//cout << j << ", ";
			const GridRanges &floor = floorings[j];
			grid.gridRangeUnion(solid, floor, multiFloor);
			solid = multiFloor;
		}
		//cout << "), roof(";
		for (size_t j = i; j <= lastRoof; j++)
		{
			GridRanges multiRoof;
			const GridRanges &roofing = roofings[j];
			grid.gridRangeUnion(solid, roofing, multiRoof);
			//cout << j << ", ";
			solid = multiRoof;
		}
		// solid = flooring;
		//cout << ") )" << endl;

		solids[i] = solid;

		GridRanges &sparse = sparses[i];
		sparse = infills[i];

	}

}

void Regioner::gridRangesForSlice(const libthing::Insets &allInsetsForSlice,
						const Grid &grid,
						GridRanges &surface)
{
	const libthing::SegmentTable &innerMostLoops = allInsetsForSlice.back();
	grid.createGridRanges(innerMostLoops, surface);
}
