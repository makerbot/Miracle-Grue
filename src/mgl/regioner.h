
/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/


#ifndef REGIONER_H_
#define REGIONER_H_ (1)

#include "slicer.h"
#include "loop_path.h"

namespace mgl
{




struct Regions
{
    std::vector<libthing::Insets> 	insets;
    std::vector<std::list<LoopList> > 	        insetLoops;
    std::vector<GridRanges>     flatSurfaces; // # number of slices + roofCount * 2
    std::vector<GridRanges>     roofings;
    std::vector<GridRanges>     floorings;


    std::vector<GridRanges>     infills;

    std::vector<GridRanges>     solids;
    std::vector<GridRanges>     sparses;

    Regions()
    {}
};



//// Class to calculate regions of a model
///
class Regioner : public Progressive
{


	Scalar roofLengthCutOff;
public:
	SlicerConfig slicerCfg;


	Regioner(const SlicerConfig &slicerCfg,
				ProgressBar *progress = NULL);

	void generateSkeleton(const Tomograph &tomograph , Regions &regions);

	void insetsForSlice(const libthing::SegmentTable &sliceOutlines,
	    					libthing::Insets &sliceInsets,
	    					const char*scadFile=NULL);

	void insets(const std::vector<libthing::SegmentTable> & outlinesSegments, std::vector<libthing::Insets> & insets);

	void flatSurfaces(	const std::vector<libthing::Insets> & insets,
	    					const Grid & grid,
	    					std::vector<GridRanges> & gridRanges);

	void floorForSlice( const GridRanges & currentSurface, const GridRanges & surfaceBelow, const Grid & grid,
	    					GridRanges & flooring);

    void roofing(const std::vector<GridRanges> & flatSurfaces, const Grid & grid, std::vector<GridRanges> & roofings);
    void roofForSlice( const GridRanges & currentSurface, const GridRanges & surfaceAbove, const Grid & grid, GridRanges & roofing);
    void flooring(const std::vector<GridRanges> & flatSurfaces, const Grid & grid, std::vector<GridRanges> & floorings);
    void infills(const std::vector<GridRanges> &flatSurfaces,
    			 const Grid &grid,
    			 const std::vector<GridRanges> &roofings,
    			 const std::vector<GridRanges> &floorings,
    			 std::vector<GridRanges> &solids,
    			 std::vector<GridRanges> &sparses,
    			 std::vector<GridRanges> &infills);
    void gridRangesForSlice(const libthing::Insets &allInsetsForSlice,
    						const Grid &grid,
    						GridRanges &surface);

private:




};

}

#endif /* SKELETOR_H_ */
