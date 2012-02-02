/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/



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


void createConvexList(const std::vector<LineSegment2> &segments, std::vector<bool> &convex)
{
    for(int id = 0; id < segments.size(); id++)
    {
        unsigned int prevId = id==0 ? segments.size()-1 : id-1;

        const LineSegment2 &seg = segments[id];
        const LineSegment2 &prevSeg = segments[prevId];

        const Vector2 & i = prevSeg.a;
        const Vector2 & j = seg.a;
        const Vector2 & j2 = prevSeg.b;
        const Vector2 & k = seg.b;
        bool isSameSame = j.sameSame(j2);

        if(!isSameSame)
        {
			stringstream ss;
        	ss << endl << "CONNECTIVITY ERROR" <<  endl;
        	ss << "id: " << id << ", prevId: " <<  prevId << endl;
        	ss << "i: " << i << ", j: " << j << ", j2: "<< j2 << endl;
        	Vector2 d = j2 - j;
        	Scalar distance = d.magnitude();
        	ss << "distance " << distance << endl;
        	ss << "SameSame " << isSameSame << endl;
        	ShrinkyMess mixup(ss.str().c_str());
        	throw mixup;

        }
        bool vertex = convexVertex(i,j,k);
        convex.push_back(vertex);
    }
}


void dumpSegments(const std::vector<LineSegment2> &segments)
{
	/*
	cout << "id\tconvex\tlength\tdistance\tangle\ta, b" << endl;

    for(int id = 0; id < segments.size(); id++)
    {

        const LineSegment2 &seg = segments[id];
        unsigned int prevId = id==0 ? segments.size()-1 : id-1;

        const LineSegment2 &prevSeg = segments[prevId];

        const Vector2 & i = prevSeg.a;
        const Vector2 & j = seg.a;
        const Vector2 & j2 = prevSeg.b;
        const Vector2 & k = seg.b;
        bool isSameSame = j.sameSame(j2);

        Vector2 d = j2 - j;
        Scalar distance = d.magnitude();
        Scalar length = seg.squaredLength();
        Scalar angle = angleFromPoint2s(i, j, k);
        bool vertex = convexVertex(i,j,k);


        cout << id << "\t" << vertex << "\t" << length << ",\t" << distance << ",\t" <<  angle << "\t" << seg.a << ", " << seg.b <<"\t" << endl;
    }
    */
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


void insetSegments(const std::vector<LineSegment2> &segments, Scalar d,
					std::vector<LineSegment2> &insets)
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
	assert(insets.size() == segments.size());
}

string segment3(const LineSegment2 &s, Scalar z)
{
	stringstream ss;
	ss << "[[" << s.a[0] << ", " << s.a[1] << ", " << z << "], [" <<  s.b[0] << ", " << s.b[1] << ", " << z << "]]";
	return ss.str();
}


void elongate(LineSegment2 &s)
{
	Vector2 l = s.b - s.a;
	l.normalise();
	l *= 100000;
	s.b += l;
	s.a -= l;
}

void elongateAndTrim(LineSegment2 &s0, LineSegment2 &s1)
{
	elongate(s0);
	elongate(s1);

	Vector2 intersection;
	bool trimmed = segmentSegmentIntersection(s0, s1, intersection);
	if(trimmed)
	{
		s0.b = intersection;
		s1.a = intersection;
	}
}

void trimConvexSegments(const std::vector<LineSegment2> & rawInsets,
						const std::vector<bool> &convex,
						std::vector<LineSegment2> & segments)
{
	assert(segments.size() == 0);
	segments = rawInsets;

	for(unsigned int i = 0; i < segments.size(); i++)
	{
		unsigned int prevId = i==0 ? segments.size()-1 : i-1;

		LineSegment2 &currentSegment = segments[i];
		LineSegment2 &previousSegment = segments[prevId];
//		elongateAndTrim(currentSegment, currentSegment);

		if (convex[i])
		{

			Vector2 intersection;
			bool trimmed = segmentSegmentIntersection(previousSegment, currentSegment, intersection);
			if(trimmed)
			{
				previousSegment.b = intersection;
				currentSegment.a = intersection;
			}
			else
			{
				stringstream ss;
				ss << "Trim ERROR: segment " << i << endl;
				ss << "segments = [  " <<  segment3(previousSegment,0) << " , " << segment3(currentSegment,0) << " ]; " << endl;
				ss << "color([0,0.5,0,1])loop_segments3(segments, false);" << endl;
				ShrinkyMess mixup(ss.str().c_str());
				// throw mixup;
			}
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


	for(unsigned int i=0; i < segments.size()-1; i++)
	{
		const LineSegment2 &seg = segments[i];
		const LineSegment2 &nextSeg = segments[i+1];
		Scalar length = nextSeg.squaredLength();
		LineSegment2 newSeg(seg);
		while(length <= cutoff2  && i < segments.size()-1  )
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
		:scadFileName(scadFileName), color(1),  counter(0), dz(0), z(0)
{
	if(scadFileName)
	{
		fscad.open(scadFileName);
		std::ofstream &out = fscad.getOut();
//		out << endl;
//		out << "module loop_segments2(segments, z)"<<endl;
//		out << "{"<<endl;
//		out << "	corner (x=segments[0][0][0],  y=segments[0][0][1], z=z, diameter=0.25, faces=12, thickness_over_width=1);"<<endl;
//		out << "    for(seg = segments)"<<endl;
//		out << "    {"<<endl;
//		out << "        tube(x1=seg[0][0], y1=seg[0][1], z1=z, x2=seg[1][0], y2=seg[1][1], z2=z , diameter1=0.1, diameter2=0.05, faces=4, thickness_over_width=1);"<<endl;
//		out << "    }"<<endl;
//		out << "}"<<endl;


		out << "module loop_segments3(segments, ball=true)"<<endl;
		out << "{"<<endl;
		out << "	if(ball) corner (x=segments[0][0][0],  y=segments[0][0][1], z=segments[0][0][2], diameter=0.25, faces=12, thickness_over_width=1);"<<endl;
		out << "    for(seg = segments)"<<endl;
		out << "    {"<<endl;
		out << "        tube(x1=seg[0][0], y1=seg[0][1], z1=seg[0][2], x2=seg[1][0], y2=seg[1][1], z2=seg[1][2] , diameter1=0.1, diameter2=0.05, faces=4, thickness_over_width=1);"<<endl;
		out << "    }"<<endl;
		out << "}"<<endl;
	}

}


void Shrinky::inset(const std::vector<LineSegment2> & segments,
								Scalar insetDist,
									std::vector<LineSegment2> & finalInsets)
{
    // OpenScad
	Scalar z = 0;
    Scalar dz =0.1;

	assert(segments.size() > 0);
	assert(finalInsets.size() == 0);


	assert((const void*)&segments != (const void*)&finalInsets);

	//std::vector<LineSegment2> shorts;
	std::vector<bool> convexVertices;
	std::vector<LineSegment2> insets;
	std::vector<LineSegment2> trims;

	unsigned int segmentCount = segments.size();
	if(segmentCount < 3)
	{
		stringstream ss;
		ss << segmentCount << " segments are not enough for a polygon";
		ShrinkyMess mixup(ss.str().c_str());
		throw mixup;
	}

    try
    {
/*
    	// std::cout << std::endl << "*** Shrinky::inset " << std::endl;
    	dumpSegments(segments);
		// std::cout << std::endl << "*** RemoveShortSegments" << std::endl;
    	//	removeShortSegments(segments, 0.000001 , shorts);

		//std::cout << std::endl<< "*** createConvexList" << std::endl;
		createConvexList(segments, convexVertices);

		std::cout << endl << "*** insetSegments" << std::endl;
		insetSegments(segments, insetDist, insets);
		dumpSegments(insets);

		std::cout << endl << "*** trimConvexSegments" << std::endl;
		trimConvexSegments(insets, convexVertices, trims);
		dumpSegments(trims);

		std::cout << std::endl << "*** AddReflexSegments" << std::endl;
		AddReflexSegments(segments, trims, convexVertices, finalInsets); // reflexedInsets);
		dumpSegments(finalInsets);
*/
		insets = segments;
		trims = segments;
		finalInsets.reserve(segments.size());
		for(int i=0; i< segments.size(); i++)
		{
			finalInsets.push_back(segments[i]);
		}

		// cout << "FINALS" << endl;
		dumpSegments(finalInsets);

		assert(finalInsets.size() > 0);

    }
    catch(const ShrinkyMess& ouch)
    {
    	//cout << ouch.error << endl;
    	dumpSegments(segments);

    	cout << endl;
    	cout << "// s = ['segs.push_back(LineSegment2(Vector2(%s, %s), Vector2(%s, %s)));' %(x[0][0], x[0][1], x[1][0], x[1][1]) for x in segments]" << endl;
        std::cout << std::endl << "// loop_segments3(segments);" << std::endl;

        ScadTubeFile::segment3(cout,"","segments", segments, 0, 0.1);
        ScadTubeFile::segment3(cout,"","rawInsets", insets, 0, 0.1);
        ScadTubeFile::segment3(cout,"","trimmedInsets", trims, 0, 0.1);
        ScadTubeFile::segment3(cout,"","finalInsets", finalInsets, 0, 0.1);
    }

    if(scadFileName)
    {
        stringstream coloredOutline;
        // Scalar color = (1.0 * i)/(shells-1);
        color = color == 0 ? 1 : 0;
        coloredOutline << "color([" << color << "," << color << "," << 1 - color << " ,1])";
        coloredOutline << "loop_segments3";

    	z = fscad.writeSegments3("outlines_", coloredOutline.str().c_str(), segments, z, dz,  this->counter);
        //z = fscad.writeSegments3("shortened_", "color([0.5,0.5,0,1])loop_segments3", shorts, z, dz, this->counter);
        z = fscad.writeSegments3("raw_insets_", "color([1,0,0.4,1])loop_segments3", insets, z, dz, this->counter);
        z = fscad.writeSegments3("trimmed_insets_", "color([0,0.2,0.2,1])loop_segments3", trims, z , dz, this->counter);
        z = fscad.writeSegments3("reflexed_insets_", "color([0,0.5,0,1])loop_segments3", finalInsets, z ,  dz,this->counter);
        this->counter ++;
    }
}

Shrinky::~Shrinky()
{
	if(scadFileName)
	{
		std::ofstream &out = fscad.getOut();

		unsigned int shells = counter;
		fscad.writeMinMax("draw_outlines",  "outlines_", shells);
		fscad.writeMinMax("draw_raw_insets",  "raw_insets_", shells);
		fscad.writeMinMax("draw_trimmed_insets",  "trimmed_insets_", shells);
		fscad.writeMinMax("draw_reflexed_insets",  "reflexed_insets_", shells);
		// fscad.writeMinMax("draw_shortened_insets",  "shortened_", shells);

		out << "min=0;"<<endl;
		out << "max=" << shells -1 << ";"<<std::endl;
		out <<std::endl;
		out << "draw_outlines(min, max);" <<std::endl;
		out << "draw_raw_insets(min, max);" <<std::endl;
		out << "draw_trimmed_insets(min, max);" <<std::endl;
		out << "draw_reflexed_insets(min, max);" <<std::endl;

		// out << "draw_shortened_insets(min, max);" <<std::endl;
		// out << "// ss = [\"s.push_back(LineSegment2(%s,%s));\" % (x[0],x[1]) for x in segments]" <<std::endl;

		out << "// s = [\"segs.push_back(LineSegment2(Vector2(%s, %s), Vector2(%s, %s)));\" %(x[0][0], x[0][1], x[1][0], x[1][1]) for x in segments]" << std::endl;
		fscad.close();
	}
}
