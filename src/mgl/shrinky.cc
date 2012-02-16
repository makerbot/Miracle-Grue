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

std::ostream& operator << (std::ostream &os, const TriangleSegment2 &s)
{
	os << "[ " << s.a << ", " << s.b << "]";
	return os;
}

void connectivityCheck(const std::vector<TriangleSegment2> &segments)
{

}

void createConvexList(const std::vector<TriangleSegment2> &segments, std::vector<bool> &convex)
{
	Scalar tol=0.3;

    for(int id = 0; id < segments.size(); id++)
    {
        unsigned int prevId = id==0 ? segments.size()-1 : id-1;

        const TriangleSegment2 &seg = segments[id];
        const TriangleSegment2 &prevSeg = segments[prevId];

        const Vector2 & i = prevSeg.a;
        const Vector2 & j = seg.a;
        const Vector2 & j2 = prevSeg.b;
        const Vector2 & k = seg.b;
        bool isSameSame = j.sameSame(j2, tol);

        if(!isSameSame)
        {
			stringstream ss;
        	ss << endl << "CONNECTIVITY ERROR" <<  endl;
        	ss << "Segment id: " << id << ", prevId: " <<  prevId << endl;
        	ss << "i: " << i  << endl;
        	ss << "j: " << j  << endl;
        	ss << "j2: "<< j2 << endl;
        	ss << "k: " << k  << endl;
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


void segmentsDiagnostic(const char* title , const std::vector<TriangleSegment2> &segments)
{

	cout << endl << title << endl;
	cout << "id\tconvex\tlength\tdistance\tangle\ta, b" << endl;

    for(int id = 0; id < segments.size(); id++)
    {

        const TriangleSegment2 &seg = segments[id];
        unsigned int prevId = id==0 ? segments.size()-1 : id-1;

        const TriangleSegment2 &prevSeg = segments[prevId];

        const Vector2 & i = prevSeg.a;
        const Vector2 & j = seg.a;
        const Vector2 & j2 = prevSeg.b;
        const Vector2 & k = seg.b;

        Vector2 d = j2 - j;
        Scalar distance = d.magnitude();
        Scalar length = seg.squaredLength();
        Scalar angle = angleFromPoint2s(i, j, k);
        bool vertex = convexVertex(i,j,k);

        cout << id << "\t" << vertex << "\t" << length << ",\t" << distance << ",\t" <<  angle << "\t" << seg.a << ", " << seg.b <<"\t" << endl;
    }
}


Vector2 getInsetDirection(const TriangleSegment2 &seg)
{
	Vector3 v(seg.b.x - seg.a.x, seg.b.y - seg.a.y, 0);
	Vector3 up(0,0,1);
	Vector3 inset = v.crossProduct(up);
	inset.normalise();
	Vector2 inset2(inset.x, inset.y);
	return inset2;
}


void insetSegments(const std::vector<TriangleSegment2> &segments, Scalar d,
					std::vector<TriangleSegment2> &insets)
{
	assert(insets.size() == 0);
	for(int i=0; i<segments.size(); i++)
	{

		TriangleSegment2 seg = segments[i];
		Vector2 inset = getInsetDirection(seg);

		inset *= d;

		TriangleSegment2 newSeg(seg);
		newSeg.a += inset;
		newSeg.b += inset;

		insets.push_back(newSeg);

	}
	assert(insets.size() == segments.size());
}

string segment3(const TriangleSegment2 &s, Scalar z)
{
	stringstream ss;
	ss << "[[" << s.a[0] << ", " << s.a[1] << ", " << z << "], [" <<  s.b[0] << ", " << s.b[1] << ", " << z << "]]";
	return ss.str();
}


TriangleSegment2 elongate(const TriangleSegment2 &s, Scalar dist, Scalar tol)
{
	TriangleSegment2 segment(s);
	Vector2 l = segment.b - segment.a;
	if(l.squaredMagnitude() > tol)
	{
		l.normalise();
		l *= dist;
		segment.b += l;
		segment.a -= l;
	}
	return segment;
}


void trimConvexSegments(const std::vector<TriangleSegment2> & rawInsets,
						const std::vector<bool> &convex,
						std::vector<TriangleSegment2> & segments)
{
	assert(segments.size() == 0);
	segments = rawInsets;

	for(unsigned int i = 0; i < segments.size(); i++)
	{
		unsigned int prevId = i==0 ? segments.size()-1 : i-1;

		TriangleSegment2 &currentSegment = segments[i];
		TriangleSegment2 &previousSegment = segments[prevId];

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
	}
}

void AddReflexSegments(	const std::vector<TriangleSegment2> &segments,
						const std::vector<TriangleSegment2> &trimmedInsets,
						const std::vector<bool> &convexVertices,
						std::vector<TriangleSegment2> &newSegments)
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

			TriangleSegment2 straight(start, end);
			newSegments.push_back(straight);
		}
		newSegments.push_back(trimmedInsets[i] );
	}

}

void removeShortSegments(const std::vector<TriangleSegment2> &segments,
						Scalar cutoffLength,
						std::vector<TriangleSegment2> &shorts)
{

	shorts.reserve(segments.size()); // worst case
	assert(cutoffLength > 0);

	Scalar cutoff2 = cutoffLength * cutoffLength;

	for(unsigned int i=0; i < segments.size(); i++)
	{
		unsigned int prevId = i==0 ? segments.size()-1 : i-1;
		unsigned int nextId = i==segments.size()-1 ? 0 : i+1;

		const TriangleSegment2 &prevSeg = segments[prevId];
		const TriangleSegment2 &seg = segments[i];
		const TriangleSegment2 &nextSeg = segments[nextId];
		Scalar length = nextSeg.squaredLength();
		TriangleSegment2 newSeg(seg);
		if(length < cutoffLength)
		{
			//newSeg.b = nextSeg.b;
			//i++; // skip one
		}
		else
		{
			shorts.push_back(newSeg);
		}

//		while(length <= cutoff2  && i < segments.size()-1  )
//		{
//			i ++;
//			const LineSegment2 &next = segments[i];
//			newSeg.b = next.b;
//			length += next.squaredLength();
//		}
//		finalInsets.push_back(newSeg);
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

		fscad.writeHeader();
	}

}

void trimSegments(const std::vector<TriangleSegment2> & longSegments,
					std::vector<TriangleSegment2> &segments)
{
	assert(segments.size() == 0);
	segments.reserve(longSegments.size());
	for(unsigned int i = 0; i < longSegments.size(); i++)
	{
		TriangleSegment2 seg = longSegments[i];
		segments.push_back(seg);
	}

	for(unsigned int i = 0; i < segments.size(); i++)
	{
		unsigned int prevId = i==0 ? segments.size()-1 : i-1;

		TriangleSegment2 &previousSegment = segments[prevId];
		TriangleSegment2 &currentSegment =  segments[i];

		Scalar tol = 0.1;
		Scalar dist = 100000;
		TriangleSegment2 prev = elongate(previousSegment, dist, tol);
		TriangleSegment2 cur =  elongate(currentSegment, dist, tol);

		Vector2 intersection;
		bool trimmed = segmentSegmentIntersection(prev, cur, intersection);
		if(trimmed)
		{
			previousSegment.b = intersection;
			currentSegment.a = intersection;
			//cout << "x";
		}
		else
		{
			//cout << "o";
		}
	}
}


void createBisectors(const std::vector<TriangleSegment2>& segments, Scalar tol, std::vector<Vector2>  &motorCycles)
{
	for(unsigned int i=0; i < segments.size(); i++)
	{
		unsigned int prevId = i==0 ? segments.size()-1 : i-1;
		// unsigned int nextId = i==segments.size()-1 ? 0 : i+1;

		const TriangleSegment2 &prevSeg = segments[prevId];
		const TriangleSegment2 &seg = segments[i];
		// const TriangleSegment2 &nextSeg = segments[nextId];

		Vector2 prevInset = getInsetDirection(prevSeg);
		Vector2 inset = getInsetDirection(seg);

		Vector2 bisector = inset;

		// if points are disjoint, do not combine both insets
		if(prevSeg.b.sameSame(seg.a, tol) )
		{
			bisector += prevInset;
		}
		else
		{
			stringstream ss;
			ss << "This is not a closed polygon";
			ShrinkyMess mixup(ss.str().c_str());
			throw mixup;
			// assert(0);
		}
		bisector.normalise();

		motorCycles.push_back(bisector);
	}
}

void Shrinky::inset(const std::vector<TriangleSegment2>& originalSegments,
								Scalar insetDist,
								Scalar cutoffLength,
								std::vector<TriangleSegment2> &finalInsets)
{
	Scalar tol = 0.35*4;

    // OpenScad
	Scalar z = 0;
    Scalar dz =0.1;
    if(originalSegments.size() ==0)
    {
    		stringstream ss;
    		ss << "Trying to inset an empty polygon";
    		ShrinkyMess mixup(ss.str().c_str());
    		throw mixup;
    }
	// assert(originalSegments.size() > 0);
	assert(finalInsets.size() == 0);

	// check that we're not playing with ourselves
	assert(&originalSegments != &finalInsets);

	std::vector<TriangleSegment2> shorts;
	std::vector<bool> convexVertices;
	std::vector<TriangleSegment2> insets;
	std::vector<TriangleSegment2> trims;

	std::vector<Vector2> bisectors;

	unsigned int segmentCount = originalSegments.size();
	if(segmentCount < 2)
	{
		stringstream ss;
		ss << segmentCount << " line segment is not enough to create a closed polygon";
		ShrinkyMess mixup(ss.str().c_str());
		throw mixup;
	}

	bool dumpSteps = false;
//	dumpSteps = true;

	bool byPass = false;
	if(byPass)
	{
		finalInsets.reserve(originalSegments.size());
		for(int i=0; i< originalSegments.size(); i++)
		{
			finalInsets.push_back(originalSegments[i]);
		}
		return;
	}
    try
    {
    	// std::cout << std::endl << "*** Shrinky::inset " << std::endl;
    	if(dumpSteps)segmentsDiagnostic("originalSegments", originalSegments);
		// std::cout << std::endl << "*** RemoveShortSegments" << std::endl;
    	shorts = originalSegments;

    	//removeShortSegments(originalSegments, tol , shorts);
    	assert(shorts.size() > 0);
    	if(dumpSteps) segmentsDiagnostic("Shorts",shorts);

		insetSegments(shorts, insetDist, insets);
		if(dumpSteps) segmentsDiagnostic("Insets", insets);

		createBisectors(shorts, tol, bisectors);
		trimSegments(insets, finalInsets);

		// currentSegments = finalInsets ;
		if(dumpSteps) segmentsDiagnostic("Finals", finalInsets);
		//assert(currentSegments.size() > 0);
		// cout << "FINALS" << endl;

    }
    catch(const ShrinkyMess& ouch)
    {
    	cout << "\n\n\n\n" << endl;
    	cout << "INSET ABORT!!" << endl;
    	cout << "\n\n\n\n" << endl;
    	cout << ouch.error << endl;
    	segmentsDiagnostic("segments",originalSegments);

    	cout << endl;
    	cout << "// s = ['segs.push_back(LineSegment2(Vector2(%s, %s), Vector2(%s, %s)));' %(x[0][0], x[0][1], x[1][0], x[1][1]) for x in segments]" << endl;
        std::cout << std::endl << "// loop_segments3(segments);" << std::endl;
        Scalar z = 0;
        Scalar dz = 0.1;
        z = ScadTubeFile::segment3(cout,"","segments", originalSegments, z, dz);
        z = ScadTubeFile::segment3(cout,"","rawInsets", insets, z, dz);
        z = ScadTubeFile::segment3(cout,"","trimmedInsets", trims, z, dz);
        z = ScadTubeFile::segment3(cout,"","finalInsets", finalInsets, z, dz);
    }

    if(scadFileName)
    {
        stringstream coloredOutline;
        // Scalar color = (1.0 * i)/(shells-1);
        color = color == 0 ? 1 : 0;
        coloredOutline << "color([" << color << "," << color << "," << 1 - color << " ,1])";
        coloredOutline << "loop_segments3";

    	z = fscad.writeSegments3("outlines_", coloredOutline.str().c_str(), originalSegments, z, dz,  this->counter);

    	std::vector<TriangleSegment2> motorCycleTraces;
    	for(int i=0; i < shorts.size(); i++)
    	{
    		Vector2 a = shorts[i].a;
    		Vector2 dir = bisectors[i];
    		dir *= 2;
    		Vector2 b = a + dir;
    		TriangleSegment2 s(a, b);
    		motorCycleTraces.push_back(s);
    	}

    	Scalar shortz = z;
        z = fscad.writeSegments3("shortened_", "color([0.5,0.5,0,1])loop_segments3", shorts, z, 0, this->counter);

        z = fscad.writeSegments3("motorcycles_", "color([0.75,0.5,0.2,1])loop_segments3", motorCycleTraces, shortz, 0, this->counter);
        z += dz;
    	z = fscad.writeSegments3("raw_insets_", "color([1,0,0.4,1])loop_segments3", insets, z, 0, this->counter);
        z += 2 * dz;
        //z = fscad.writeSegments3("trimmed_insets_", "color([0,0.2,0.2,1])loop_segments3", trims, z , dz, this->counter);
        z += 2 * dz;
        z = fscad.writeSegments3("final_insets_", "color([0,0.5,0,1])loop_segments3", finalInsets, z ,  0,this->counter);
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
		fscad.writeMinMax("draw_motorcycles",  "motorcycles_", shells);
		fscad.writeMinMax("draw_raw_insets",  "raw_insets_", shells);
		//fscad.writeMinMax("draw_trimmed_insets",  "trimmed_insets_", shells);
		fscad.writeMinMax("draw_shortened_insets",  "shortened_", shells);
		fscad.writeMinMax("draw_final_insets",  "final_insets_", shells);

		out << "min=0;"<<endl;
		out << "max=" << shells -1 << ";"<<std::endl;
		out <<std::endl;
		out << "draw_outlines(min, max);" <<std::endl;
		out << "draw_motorcycles(min, max);" <<std::endl;
		out << "draw_shortened_insets(min, max);" <<std::endl;
		out << "draw_raw_insets(min, max);" <<std::endl;
		//out << "draw_trimmed_insets(min, max);" <<std::endl;
		out << "draw_final_insets(min, max);" <<std::endl;

		// out << "draw_shortened_insets(min, max);" <<std::endl;
		// out << "// ss = [\"s.push_back(LineSegment2(%s,%s));\" % (x[0],x[1]) for x in segments]" <<std::endl;

		out << "// s = [\"segs.push_back(TriangleSegment2(Vector2(%s, %s), Vector2(%s, %s)));\" %(x[0][0], x[0][1], x[1][0], x[1][1]) for x in segments]" << std::endl;
		fscad.close();
	}
}
