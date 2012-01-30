/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/


// #include <time.h>
// #include <fstream>
// #include <algorithm> // find
// #include <iterator>  // distance
// #include <iomanip>
// #include <limits>
// #include <set>


#include "shrinky.h"

using namespace std;
using namespace mgl;

double AreaSign(const Vector2 &a, const Vector2 &b, const Vector2 &c)
{
    double area2;

    area2 = (b[0] - a[0] ) * (double)( c[1] - a[1]) -
            (c[0] - a[0] ) * (double)( b[1] - a[1]);

    return area2;
}

bool convexVertex(const Vector2 &i, const Vector2 &j, const Vector2 &k)
{
	return AreaSign(i,j,k) < 0;
}

std::ostream& operator << (std::ostream &os, const LineSegment2 &s)
{
	os << "[ " << s.a << ", " << s.b << "]";
	return os;
}

bool trim(LineSegment2 &s0, LineSegment2 &s1)
{
	Vector2 intersection;
	intersection.x = -10;
	intersection.y = -10;
	bool success = segmentSegmentIntersection(	s0, s1, intersection);

	cout << "Trimming: " <<endl;
	cout << "	s0" << s0 << endl;
	cout << "	s1" << s1 << endl;
	cout << "   has intersection: " << success << endl;
	cout << "	intersection: " << intersection << endl;

	s0.b = intersection;
	s1.a = intersection;

	//CPPUNIT_ASSERT(success);

	return success;
}



void createConvexList(const std::vector<LineSegment2> & segments, std::vector<bool> &convex)
{

    for(int id = 0; id < segments.size(); id++){

        const LineSegment2 &seg = segments[id];

        unsigned int previousSegmentId;
        if(id == 0)
            previousSegmentId = segments.size() - 1;
        else
            previousSegmentId = id - 1;

        const LineSegment2 &prevSeg = segments[previousSegmentId];

        const Vector2 & i = prevSeg.a;
        const Vector2 & j = seg.a;
        const Vector2 & j2 = prevSeg.b;
        const Vector2 & k = seg.b;
        bool isSameSame = j.sameSame(j2);

        if(!isSameSame)
        {
        	cout << endl << "ERROR" <<  endl;
        	cout << "id: " << id << "/" << segments.size()<< ", prevId: " <<  previousSegmentId << endl;
        	cout << "j: " << j << ", j2: "<< j2 << endl;
        	cout << "SameSame " << isSameSame<< endl;
        //	assert(isSameSame);
        }

        Scalar angle = angleFromPoint2s(i, j, k);
        bool vertex = convexVertex(i,j,k);
        convex.push_back(vertex);
    }
}


void dumpConvexList(const std::vector<bool> &convex)
{
	cout << "convex list (" <<  convex.size() << ")" << endl;
	for(int id = 0; id < convex.size(); id++)
	{
		cout << "   " << id << ") " << convex[id] << endl;
	}
}


Vector2 getInsetNormal(const LineSegment2 &seg)
{
	Vector3 v(seg.b.x - seg.a.x, seg.b.y - seg.a.y, 0);
	Vector3 up(0,0,1);
	Vector3 inset = v.crossProduct(up);
	inset.normalise();
	Vector2 inset2(inset.x, inset.y);
	return inset2;
}


void insetSegments(const std::vector<LineSegment2> &segments,
					std::vector<LineSegment2> &insets, Scalar d)
{
	assert(insets.size() == 0);
	for(int i=0; i<segments.size(); i++)
	{

		LineSegment2 seg = segments[i];
		Vector2 inset = getInsetNormal(seg);

		inset *= d;

		LineSegment2 newSeg(seg);
		newSeg.a += inset;
		newSeg.b += inset;

		insets.push_back(newSeg);

	}

}

void trimConvexSegments(const std::vector<LineSegment2> & rawInsets,
						const std::vector<bool> &convex,
						std::vector<LineSegment2> & segments)
{
	assert(segments.size() == 0);
	segments = rawInsets;

	for(unsigned int id = 0; id < segments.size(); id++)
	{
		unsigned int previousSegmentId;
		if(id == 0)
			previousSegmentId = segments.size() - 1;
		else
			previousSegmentId = id - 1;

		LineSegment2 &currentSegment = segments[id];
		LineSegment2 &previousSegment = segments[previousSegmentId];

		if (convex[id])
		{
			// cout << "Trimming convex: " << id << endl;
			Vector2 intersection;
			bool trimmed = segmentSegmentIntersection(previousSegment, currentSegment, intersection);
			if(!trimmed)
			{
				cout << endl;
				cout << endl;
				cout << "Trim ERROR:" << endl;
				cout << "segments = [" <<  previousSegment << ", " << currentSegment << "]; " << endl;
				cout << "color([0,0.5,0,1])infill_segments(segments);" << endl;
				// assert(trimmed);
			}
			previousSegment.b = intersection;
			currentSegment.a = intersection;
		}
		// cout << i << " , " << j << ", " << k << " ,\t " << angle << ", " << convex << endl;
	}
}

void AddReflexSegments(	const std::vector<LineSegment2> &segments,
						const std::vector<LineSegment2> &trimmedInsets,
						const std::vector<bool> &convexVertices,
						std::vector<LineSegment2> &newSegments)
{
	assert(newSegments.size() == 0 );
	newSegments.reserve(segments.size() * 2);

	for(unsigned int i=0; i < segments.size(); i++)
	{
		unsigned int prevId = i==0 ? segments.size()-1 : i-1;

		if(!convexVertices[i])
		{
			Vector2 center = segments[i].a;
			Vector2 start  = trimmedInsets[prevId].b;
			Vector2 end    = trimmedInsets[i].a;
//			cout << "HO YO!! semgment " << i << endl;
//			cout << " center is: " << center << endl;
//			cout << " start: " << start << endl;
//			cout << " end: " << end << endl;
			LineSegment2 straight(start, end);
			newSegments.push_back(straight);
		}
		newSegments.push_back(trimmedInsets[i] );
	}

}

void removeShortSegments(const std::vector<LineSegment2> &segments,
						Scalar cutoffLength,
						std::vector<LineSegment2> &finalInsets)
{

	finalInsets.reserve(segments.size()); // worst case
	assert(cutoffLength > 0);

	Scalar cutoff2 = cutoffLength * cutoffLength;


	for(unsigned int i=0; i < segments.size(); i++)
	{

		const LineSegment2 &seg = segments[i];
		Scalar length = seg.squaredLength();
		LineSegment2 newSeg(seg);
		while(length <= cutoff2  && i < segments.size()  )
		{
			i ++;
			const LineSegment2 &next = segments[i];
			newSeg.b = next.b;
			length += next.squaredLength();

		}
		finalInsets.push_back(newSeg);
	}

}

Shrinky::Shrinky( const char *scadFileName, // = NULL
					Scalar layerH) // =0.5
		:scadFileName(scadFileName), color(1), layerH(layerH), counter(0)
{
	if(scadFileName)
	{
		fscad.open(scadFileName);
		std::ofstream &out = fscad.getOut();
		out << endl;
		out << "module loop_segments(segments, z)"<<endl;
		out << "{"<<endl;
		out << "	corner (x=segments[0][0][0],  y=segments[0][0][1], z=z, diameter=0.25, faces=12, thickness_over_width=1);"<<endl;
		out << "    for(seg = segments)"<<endl;
		out << "    {"<<endl;
		out << "        tube(x1=seg[0][0], y1=seg[0][1], z1=z, x2=seg[1][0], y2=seg[1][1], z2=z , diameter1=0.1, diameter2=0.05, faces=4, thickness_over_width=1);"<<endl;
		out << "    }"<<endl;
		out << "}"<<endl;
	}
}


void Shrinky::inset(const std::vector<LineSegment2> & segments,
								Scalar insetDist,
									std::vector<LineSegment2> & finalInsets)
{
    std::cout << "	createConvexList" << std::endl;
    std::vector<bool> convexVertices;
    createConvexList(segments, convexVertices);
    //dumpConvexList(convexVertices);
    std::cout << "	insetSegments" <<std::endl;
    std::vector<LineSegment2> rawInsets;
    insetSegments(segments, rawInsets, insetDist);
    std::vector<LineSegment2> trimmedInsets;
    std::cout << "	trimConvexSegments" <<std::endl;
    trimConvexSegments(rawInsets, convexVertices, trimmedInsets);
    std::vector<LineSegment2> reflexedInsets;
    AddReflexSegments(segments, trimmedInsets, convexVertices, reflexedInsets);
    removeShortSegments(reflexedInsets, insetDist / 10, finalInsets);
    stringstream coloredOutline;
    // Scalar color = (1.0 * i)/(shells-1);
    color = color == 0 ? 1 : 0;
    coloredOutline << "color([" << color << "," << color << "," << 1 - color << " ,1])loop_segments";
    if(scadFileName)
    {
        Scalar z = layerH * this->counter;
        fscad.writeSegments("segments_", coloredOutline.str().c_str(), segments, z, this->counter);
        fscad.writeSegments("raw_insets_", "color([1,0,0.4,1])loop_segments", rawInsets, z + 0.2 * layerH, this->counter);
        fscad.writeSegments("trimmed_insets_", "color([0,0.2,0.2,1])loop_segments", trimmedInsets, z + 0.4 * layerH, this->counter);
        fscad.writeSegments("reflexed_insets_", "color([0,0.5,0,1])loop_segments", reflexedInsets, z + 0.6 * layerH, this->counter);
        fscad.writeSegments("shortened_insets_", "color([0.5,0.5,0,1])loop_segments", reflexedInsets, z + 0.8 * layerH, this->counter);
    }
    this->counter ++;
}

Shrinky::~Shrinky()
{
	if(scadFileName)
	{
		std::ofstream &out = fscad.getOut();

		unsigned int shells = counter;
		fscad.writeMinMax("draw_outlines",  "segments_", shells);
		fscad.writeMinMax("draw_raw_insets",  "raw_insets_", shells);
		fscad.writeMinMax("draw_trimmed_insets",  "trimmed_insets_", shells);
		fscad.writeMinMax("draw_reflexed_insets",  "reflexed_insets_", shells);
		fscad.writeMinMax("draw_shortened_insets",  "shortened_insets_", shells);

		out << "min=0;"<<endl;
		out << "max=" << shells -1 << ";"<<std::endl;
		out <<std::endl;
		out << "draw_outlines(min, max);" <<std::endl;
		out << "draw_raw_insets(min, max);" <<std::endl;
		out << "draw_trimmed_insets(min, max);" <<std::endl;
		out << "draw_reflexed_insets(min, max);" <<std::endl;
		out << "draw_shortened_insets(min, max);" <<std::endl;

		// out << "// ss = [\"s.push_back(LineSegment2(%s,%s));\" % (x[0],x[1]) for x in segments]" <<std::endl;
		out << "// s = [\"segs.push_back(LineSegment2(Vector2(%s, %s), Vector2(%s, %s)));\" %(x[0][0], x[0][1], x[1][0], x[1][1]) for x in segments]" << std::endl;
		fscad.close();
	}

}
