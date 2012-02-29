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



#include "core.h"
#include "segment.h"
#include "scadtubefile.h"

namespace mgl // Miracle-Grue's geometry library
{




class ShrinkyMess : public Messup {	public: ShrinkyMess(const char *msg) :Messup(msg){}};


class Shrinky
{


	const char *scadFileName;

	Scalar scadZ ;
	int color;
	unsigned int counter;

    void writeScadBisectors(const std::vector<Vector2> & bisectors,
    						const std::vector<TriangleSegment2> & originalSegments);

    void writeScadSegments(	const char* segNames,
							const char* prefix,
							const std::vector<TriangleSegment2> & segments);

    void openScadFile(const char *scadFileName);
    void closeScadFile();
public:
	Scalar dz;
	ScadTubeFile fscad;
public:
	Shrinky( const char *scadFileName=NULL);

//	void insetClassic(const std::vector<TriangleSegment2> & segments,
//							Scalar insetDist,
//								Scalar cutoffLength,
//									std::vector<TriangleSegment2> & finalInsets);



	void inset(const std::vector<TriangleSegment2> & segments,
								Scalar insetDist,
									std::vector<TriangleSegment2> & finalInsets);

	~Shrinky();

private:
	Scalar insetStep(const std::vector<TriangleSegment2> & segments,
							Scalar insetDist,
								Scalar cutoffLength,
								bool writeThisStep,
									std::vector<TriangleSegment2> & finalInsets);
};

}

#endif
