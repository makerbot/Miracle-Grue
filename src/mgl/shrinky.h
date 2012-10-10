/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/


#ifndef SHRINKY_H_
#define SHRINKY_H_



#include "mgl.h"
#include "segment.h"
#include "ScadDebugFile.h"

namespace mgl
{



/// Exception for Shrinky errors
class ShrinkyException : public Exception {
	public: ShrinkyException(const char *msg) :Exception(msg){};
};


class Shrinky
{
	const char *scadFileName;

	Scalar scadZ ;
	int color;
	unsigned int counter;

    void writeScadBisectors(const std::vector<libthing::Vector2> & bisectors,
    						const std::vector<Segment2Type> & originalSegments);

    void writeScadSegments(	const char* segNames,
							const char* prefix,
							const std::vector<Segment2Type> & segments);

    void closeScadFile();
public:
    Scalar dz;
	ScadDebugFile fscad;
public:
	Shrinky( const char *scadFileName=NULL);

	void openScadFile(const char *scadFileName);

//	void insetClassic(const std::vector<LineSegment2d> & segments,
//							Scalar insetDist,
//								Scalar cutoffLength,
//									std::vector<LineSegment2d> & finalInsets);



	void inset(const std::vector<Segment2Type> & segments,
			Scalar insetDist,
			std::vector<Segment2Type> & finalInsets);

	~Shrinky();

private:
	Scalar insetStep(const std::vector<Segment2Type> & segments,
			Scalar insetDist,
			Scalar cutoffLength,
			bool writeThisStep,
			std::vector<Segment2Type> & finalInsets);
};

/// uses a Straight Skeleton algorithm to 'shrink' the outlines to create
/// shells
/// a) takes in a segment table (i.e a series of loops, clockwise segments for perimeters,
/// and counter clockwise for holes)
/// b) creates insets for distance in insetDistances (expressed relative distances)
/// c) stores them in insetsForLoops (a list of segment tables: one table per loop,
/// and nbOffShels insets)
///
void createShellsForSliceUsingShrinky(const libthing::SegmentVector & 	outlinesSegments,
		const std::vector<Scalar> &insetDistances,
		unsigned int sliceId,
		const char *scadFile,
		std::vector<libthing::SegmentVector> & insetsForLoops);

bool attachSegments(Segment2Type &first, 
		Segment2Type &second, Scalar elongation);

}

#endif
