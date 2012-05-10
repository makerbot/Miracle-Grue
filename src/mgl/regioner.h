
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

#include "abstractable.h"
#include "grid.h"
#include "configuration.h"
#include "insets.h"


#include "meshy.h"

namespace mgl
{


// Slicer configuration data
struct SlicerConfig
{
	SlicerConfig()
	:layerH(0.27),
	 firstLayerZ(0.1),
	 tubeSpacing(1),
	 angle(1.570796326794897),
	 nbOfShells(2),
	 layerW(0.4),
	 infillShrinkingMultiplier(0.25),
	 insetDistanceMultiplier(0.9),
	 insetCuttOffMultiplier(0.01),
	 writeDebugScadFiles(false),
	 roofLayerCount(1),
	 floorLayerCount(1),
	 infillSkipCount(2),
	 gridSpacingMultiplier(0.95)
	{}

	Scalar layerH;
	Scalar firstLayerZ;
	Scalar tubeSpacing;
	Scalar angle;
	unsigned int nbOfShells;
	Scalar layerW;
	Scalar infillShrinkingMultiplier;
	Scalar insetDistanceMultiplier;
	Scalar insetCuttOffMultiplier;
	bool writeDebugScadFiles;

	unsigned int roofLayerCount;
	unsigned int floorLayerCount;
	unsigned int infillSkipCount;
	double infillDensity;
	Scalar gridSpacingMultiplier;
};

struct Regions
{
    std::vector<libthing::Insets> 		insets;
    std::vector<GridRanges>     flatSurfaces; // # number of slices + roofCount * 2
    std::vector<GridRanges>     roofings;
    std::vector<GridRanges>     floorings;
    std::vector<GridRanges>     infills;

    Regions()
    {}
};


struct LayerConfig
{
	Scalar firstLayerZ;
	Scalar layerH;
	Scalar layerW;
	Scalar gridSpacingMultiplier;
};

struct Tomograph
{
	Tomograph():layerMeasure(0,0){}

	std::vector<libthing::SegmentTable>   outlines;
	Grid grid;
	LayerMeasure layerMeasure;
};

class Slicer : public Progressive
{
	LayerConfig layerCfg;

public:
	Slicer(const SlicerConfig &slicerCfg, ProgressBar *progress =NULL)
		:Progressive(progress)
	{
		layerCfg.firstLayerZ = slicerCfg.firstLayerZ;
		layerCfg.layerH = slicerCfg.layerH;
		layerCfg.layerW = slicerCfg.layerW;
		layerCfg.gridSpacingMultiplier = slicerCfg.gridSpacingMultiplier;
	}

	void tomographyze( const char* modelFile, Tomograph &tomograph)
	{
		Meshy mesh(layerCfg.firstLayerZ, layerCfg.layerH);
		mesh.readStlFile(modelFile);

		// grid.init(mesh.limits, slicerCfg.layerW);
		unsigned int sliceCount = mesh.readSliceTable().size();
		tomograph.outlines.resize(sliceCount);

		initProgress("outlines", sliceCount);

		for(size_t sliceId =0; sliceId < sliceCount; sliceId++)
		{
			tick();
			//cout << sliceId << "/" << sliceCount << " outlines" << endl;
			libthing::SegmentTable &segments = tomograph.outlines[sliceId];
			outlinesForSlice(mesh, sliceId, segments);
		}

		Scalar gridSpacing = layerCfg.layerW * layerCfg.gridSpacingMultiplier;
		Limits limits = mesh.readLimits();
		tomograph.grid.init(limits, gridSpacing);
		tomograph.layerMeasure = mesh.readLayerMeasure();
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
};

class Regioner : public Progressive
{
public:
	SlicerConfig slicerCfg;


	Regioner(const SlicerConfig &slicerCfg,
			ProgressBar *progress = NULL)
	:Progressive(progress), slicerCfg(slicerCfg)
	{
	}

public:

	void generateSkeleton(const Tomograph &tomograph , Regions &regions)
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
						regions.infills);
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
        	grid.subSample(surface, slicerCfg.infillSkipCount, sparseInfill);

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





};

}

#endif /* SKELETOR_H_ */
