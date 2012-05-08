
/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/


#ifndef SKELETOR_H_
#define SKELETOR_H_

#include "grid.h"
#include "configuration.h"
#include "insets.h"
#include "slicy.h"

namespace mgl
{


struct Regions
{
	Grid grid;
	LayerMeasure layerMeasure;
	std::vector<libthing::SegmentTable>   outlines;
    std::vector<libthing::Insets> 		insets;
    std::vector<GridRanges>     flatSurfaces; // # number of slices + roofCount * 2
    std::vector<GridRanges>     roofings;
    std::vector<GridRanges>     floorings;
    std::vector<GridRanges>     infills;

    Regions()
    :layerMeasure(0,0)
    {}
};

class Regioner : public Progressive
{
public:
	SlicerConfig slicerCfg;
	size_t roofCount;
	size_t floorCount;
	Scalar gridSpacingMultiplier;
	size_t skipCount;
	ProgressBar *progress;

public:

	Regioner(const SlicerConfig &slicerCfg,
			Scalar gridSpacingMultiplier,
			size_t roofCount,
			size_t floorCount,
			size_t  skipCount,
			ProgressBar *progress = NULL)
	:Progressive(progress)
	{
		init(slicerCfg, gridSpacingMultiplier, roofCount, floorCount, skipCount, progress);
	}
private:
	void init(	const SlicerConfig &slicerCfg,
				Scalar gridSpacingMultiplier,
				size_t roofCount,
				size_t floorCount,
				size_t  skipCount,
				ProgressBar *progress = NULL)
	{
		this->progress = progress;
		//std::cout << "progress = " << progress << std::endl;

		this->slicerCfg = slicerCfg;
		this->roofCount = roofCount;
		this->floorCount = floorCount;
		this->gridSpacingMultiplier = gridSpacingMultiplier;
		this->skipCount = skipCount;
	}


public:

	void generateSkeleton(const char* modelFile, Regions &skeleton)
	{
		outlines(	modelFile,
					skeleton.layerMeasure,
					skeleton.grid,
					skeleton.outlines);

		insets(skeleton.outlines,
					  skeleton.insets);

		flatSurfaces(skeleton.insets,
							skeleton.grid,
							skeleton.flatSurfaces);

		roofing(skeleton.flatSurfaces, skeleton.grid, skeleton.roofings);

		flooring(skeleton.flatSurfaces, skeleton.grid, skeleton.floorings);

		infills( skeleton.flatSurfaces,
						skeleton.grid,
						skeleton.roofings,
						skeleton.floorings,
						skeleton.infills);
	}

	void outlines( const char* modelFile, LayerMeasure &layerMeasure, Grid &grid, std::vector<libthing::SegmentTable> &outlines)
	{
		Meshy mesh(slicerCfg.firstLayerZ, slicerCfg.layerH);
		mesh.readStlFile(modelFile);

		// grid.init(mesh.limits, slicerCfg.layerW);
		unsigned int sliceCount = mesh.readSliceTable().size();
		outlines.resize(sliceCount);

		initProgress("outlines", sliceCount);

		for(size_t sliceId =0; sliceId < sliceCount; sliceId++)
		{
			tick();
			//cout << sliceId << "/" << sliceCount << " outlines" << endl;
			libthing::SegmentTable &segments = outlines[sliceId];
			outlinesForSlice(mesh, sliceId, segments);
		}

		Scalar gridSpacing = slicerCfg.layerW * gridSpacingMultiplier;
		Limits limits = mesh.readLimits();
		grid.init(limits, gridSpacing);
		layerMeasure = mesh.readLayerMeasure();
	}



    void insetsForSlice(const libthing::SegmentTable &sliceOutlines,
    					libthing::Insets &sliceInsets,
    					const char*scadFile=NULL)
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

    void insets(const std::vector<libthing::SegmentTable> & outlinesSegments, std::vector<libthing::Insets> & insets)
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


    void flatSurfaces(	const std::vector<libthing::Insets> & insets,
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

    void floorForSlice( const GridRanges & currentSurface, const GridRanges & surfaceBelow, const Grid & grid, GridRanges & flooring)
    {
    	grid.gridRangeDifference(currentSurface, surfaceBelow,  flooring);
    }

    void roofForSlice( const GridRanges & currentSurface, const GridRanges & surfaceAbove, const Grid & grid, GridRanges & roofing)
    {
        grid.gridRangeDifference(currentSurface, surfaceAbove, roofing);
    }

    void roofing(const std::vector<GridRanges> & flatSurfaces, const Grid & grid, std::vector<GridRanges> & roofings)
    {
        assert(flatSurfaces.size() > 0);
        assert(roofings.size() == 0);
        unsigned int sliceCount = flatSurfaces.size();
        initProgress("roofing", sliceCount);

        roofings.resize(sliceCount);
        for(size_t i = 0;i < sliceCount-1; i++){
            tick();
        	const GridRanges & currentSurface = flatSurfaces[i];
            const GridRanges & surfaceAbove = flatSurfaces[i + 1];
            GridRanges & roofing = roofings[i];
            roofForSlice(currentSurface, surfaceAbove, grid, roofing);
    	}
    	tick();
        roofings[sliceCount -1] = flatSurfaces[sliceCount -1];

    }

    void flooring(const std::vector<GridRanges> & flatSurfaces, const Grid & grid, std::vector<GridRanges> & floorings)
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
    void infills(const std::vector<GridRanges> &flatSurfaces,
    			 const Grid &grid,
    			 const std::vector<GridRanges> &roofings,
    			 const std::vector<GridRanges> &floorings,
    			 std::vector<GridRanges> &infills)
    {

    	assert(infills.size() == 0);
    	assert(flatSurfaces.size() > 0);
    	assert(roofings.size() > 0);
        unsigned int sliceCount = flatSurfaces.size();
        initProgress("infills", sliceCount);
        infills.resize(sliceCount);
        for(size_t i=0; i< sliceCount; i++)
        {
        	tick();
        	// std::cout  << "INFILL " << i << "/" << sliceCount << std::endl;
        	const GridRanges &surface = flatSurfaces[i];
        	const GridRanges &roofing = roofings[i];
        	const GridRanges &flooring = floorings[i];
        	GridRanges sparseInfill;
        	// cout << i << "/" << sliceCount << " subsample " << skipCount << endl;
        	grid.subSample(surface, skipCount, sparseInfill);

        	// std::cout << " infill = union roofing and surface " << i << "/" << sliceCount << std::endl;
        	GridRanges roofed;
        	grid.gridRangeUnion(sparseInfill, roofing, roofed);
        	GridRanges &infill = infills[i];
        	grid.gridRangeUnion(roofed, flooring, infill);
        }

    }

    void gridRangesForSlice(const libthing::Insets &allInsetsForSlice,
    						const Grid &grid,
    						GridRanges &surface)
    {
    	const libthing::SegmentTable &innerMostLoops = allInsetsForSlice.back();
    	grid.createGridRanges(innerMostLoops, surface);
    }


	void loopsFromLineSegments(const std::vector<libthing::LineSegment2>& unorderedSegments, Scalar tol, libthing::SegmentTable & segments)
    {
        // dumpSegments("unordered_", unorderedSegments);
        // cout << segments << endl;
        if(unorderedSegments.size() > 0){
            //cout << " loopsAndHoleOgy " << endl;
          	std::vector<libthing::LineSegment2> segs =  unorderedSegments;
            loopsAndHoleOgy(segs, tol, segments);
        }
    }

    void outlinesForSlice(const Meshy & mesh, size_t sliceId, libthing::SegmentTable & segments)
    {
        Scalar tol = 1e-6;
        const LayerMeasure & layerMeasure = mesh.readLayerMeasure();
        Scalar z = layerMeasure.sliceIndexToHeight(sliceId);
        const std::vector<libthing::Triangle3> & allTriangles = mesh.readAllTriangles();
        const TriangleIndices & trianglesForSlice = mesh.readSliceTable()[sliceId];
        std::vector<libthing::LineSegment2> unorderedSegments;
        segmentationOfTriangles(trianglesForSlice, allTriangles, z, unorderedSegments);
        assert(segments.size() ==0);
        // dumpSegments("unordered_", unorderedSegments);
        // cout << segments << endl;
        loopsFromLineSegments(unorderedSegments, tol, segments);
		// cout << " done " << endl;
	}

};

}

#endif /* SKELETOR_H_ */
