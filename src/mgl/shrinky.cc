/*

   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/



#include "shrinky.h"
#include <map>
#include <set>
using namespace std;
using namespace mgl;


#define SHOUT(c) cout << c;


void lengthCheck(const std::vector<LineSegment2> &segments, const char *msg)
{
	for(unsigned int i=0; i < segments.size(); i++)
	{
		const LineSegment2 &seg = segments[i];
		Scalar l = seg.length();
		// cout << msg << " seg[" << i << "] = " << seg << " l=" << l << endl;
		if(!( l > 0 ) )
		{
			SHOUT("Z")
			stringstream ss;
			ss << msg << " Zero length: segment[" << i << "] = " << seg << endl;
			ScadTubeFile::segment3(ss,"","segments", segments, 0, 0.1);
			ShrinkyException mixup(ss.str().c_str());
			throw mixup;
		}
	}
}


//void connectivityFix(const std::vector<LineSegment2> &segments,
//		Scalar tol,
//		std::vector<LineSegment2> &outs)
//{
//
//}

void connectivityCheck( const std::vector<LineSegment2> &segments,
						Scalar tol)
{

	for(unsigned int i=0; i < segments.size(); i++)
	{
		unsigned int prevId = i==0 ? segments.size()-1 : i-1;
		const LineSegment2 &prevSeg = segments[prevId];
		LineSegment2 seg = segments[i];

		if(!prevSeg.b.tequals(seg.a, tol) )
		{
			Vector2 dist = prevSeg.b - seg.a;
			stringstream ss;
			ss << "Connectivity error: segment[" << prevId << "] = " << prevSeg << endl;
			ss << " and segment[" << i << "] = " << seg << endl;
			ss << "	segment[" << prevId << "] length = " << prevSeg.length() << endl;
			ss << " segment[" << i << "] length = " << seg.length() << endl;
			ss << " Distance between segments " << dist.magnitude();

			ss << endl;
			SHOUT("C")
			// cout << "|" << dist.magnitude() << "|" << prevSeg.length() << "|" << seg.length() << "|";
			ScadTubeFile::segment3(ss,"","segments", segments, 0, 0.1);
			ShrinkyException mixup(ss.str().c_str());
			throw mixup;

		}
	}
}

void createConvexList(const std::vector<LineSegment2> &segments, std::vector<bool> &convex)
{
	Scalar tol=0.3;

    for(size_t id = 0; id < segments.size(); id++)
    {
        size_t prevId = id==0 ? segments.size()-1 : id-1;

        const LineSegment2 &seg = segments[id];
        const LineSegment2 &prevSeg = segments[prevId];

        const Vector2 & i = prevSeg.a;
        const Vector2 & j = seg.a;
        const Vector2 & j2 = prevSeg.b;
        const Vector2 & k = seg.b;
        bool isSameSame = j.tequals(j2, tol);

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
        	SHOUT("_C_")
        	ShrinkyException mixup(ss.str().c_str());
        	throw mixup;

        }
        bool vertex = convexVertex(i,j,k);
        convex.push_back(vertex);
    }
}


void segmentsDiagnostic(const char* title , const std::vector<LineSegment2> &segments)
{

	cout << endl << title << endl;
	cout << "id\tconvex\tlength\tdistance\tangle\ta, b" << endl;

    for(size_t id = 0; id < segments.size(); id++)
    {

        const LineSegment2 &seg = segments[id];
        size_t prevId = id==0 ? segments.size()-1 : id-1;

        const LineSegment2 &prevSeg = segments[prevId];

        const Vector2 & i = prevSeg.a;
        const Vector2 & j = seg.a;
        const Vector2 & j2 = prevSeg.b;
        const Vector2 & k = seg.b;

        Vector2 d = j2 - j;
        Scalar distance = d.magnitude();
        Scalar length = seg.squaredLength();
        Scalar angle = d.angleFromPoint2s(i, j, k);
        bool vertex = convexVertex(i,j,k);

        cout << id << "\t" << vertex << "\t" << length << ",\t" << distance << ",\t" <<  angle << "\t" << seg.a << ", " << seg.b <<"\t" << endl;
    }
}


Vector2 getInsetDirection(const LineSegment2 &seg)
{
	Vector3 v(seg.b.x - seg.a.x, seg.b.y - seg.a.y, 0);
	Vector3 up(0,0,1);
	Vector3 inset = v.crossProduct(up);
	inset.normalise();
	Vector2 inset2(inset.x, inset.y);
	return inset2;
}


LineSegment2 elongateAndPrelongate(const LineSegment2 &s, Scalar dist)
{
	LineSegment2 segment(s);
	Vector2 l = segment.b - segment.a;
	l.normalise();
	l *= dist;
	segment.b += l;
	segment.a -= l;
	return segment;

}

void insetSegments(const std::vector<LineSegment2> &segments, Scalar d,
					std::vector<LineSegment2> &insets)
{
	assert(insets.size() == 0);
	for(size_t i=0; i<segments.size(); i++)
	{
		LineSegment2 seg = segments[i];
		Vector2 inset = getInsetDirection(seg);
		inset *= d;
		seg.a += inset;
		seg.b += inset;
		insets.push_back(seg);
	}
	assert(insets.size() == segments.size());
}

string segment3(const LineSegment2 &s, Scalar z)
{
	stringstream ss;
	ss << "[[" << s.a[0] << ", " << s.a[1] << ", " << z << "], [" <<  s.b[0] << ", " << s.b[1] << ", " << z << "]]";
	return ss.str();
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
				ShrinkyException mixup(ss.str().c_str());
				// throw mixup;
			}
		}
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
			LineSegment2 straight(start, end);
			newSegments.push_back(straight);
		}
		newSegments.push_back(trimmedInsets[i] );
	}

}

void removeShortSegments(const std::vector<LineSegment2> &segments,
						 Scalar cutoffLength,
						 std::vector<LineSegment2> &shorts)
{

	shorts.reserve(segments.size()); // worst case
	assert(cutoffLength > 0);
	Scalar cutoffLength2 = cutoffLength * cutoffLength;

	for(unsigned int i=0; i < segments.size(); i++)
	{
		const LineSegment2 &seg = segments[i];
		Scalar length2 = seg.squaredLength();
		if(length2 > cutoffLength2)
		{
			shorts.push_back(seg);
		}
	}
}

void Shrinky::openScadFile(const char *scadFileName)
{
    if(scadFileName){
        fscad.open(scadFileName);
        std::ostream & out = fscad.getOut();
        out << "module loop_segments3(segments, ball=true)" << endl;
        out << "{" << endl;
        out << "	if(ball) corner (x=segments[0][0][0],  y=segments[0][0][1], z=segments[0][0][2], diameter=0.25, faces=12, thickness_over_width=1);" << endl;
        out << "    for(seg = segments)" << endl;
        out << "    {" << endl;
        out << "        tube(x1=seg[0][0], y1=seg[0][1], z1=seg[0][2], x2=seg[1][0], y2=seg[1][1], z2=seg[1][2] , diameter1=0.1, diameter2=0.05, faces=4, thickness_over_width=1);" << endl;
        out << "    }" << endl;
        out << "}" << endl;
        fscad.writeHeader();
    }
}

Shrinky::Shrinky( const char *scadFileName)
		:scadFileName(scadFileName), color(1),  counter(0), dz(0), scadZ(0)
{
    openScadFile(scadFileName);
}


bool attachSegments(LineSegment2 &first, LineSegment2 &second, Scalar elongation)
{
//	LineSegment2 s0 = elongateAndPrelongate(first, elongation); // elongate(first, elongation);
//	LineSegment2 s1 = elongateAndPrelongate(second, elongation); //prelongate(second, elongation);

	LineSegment2 s0 = elongate(first, elongation);
	LineSegment2 s1 = prelongate(second, elongation);

	Vector2 intersection;
	bool trimmed = segmentSegmentIntersection(s0, s1, intersection);
	if(trimmed)
	{
		first.b = intersection;
		second.a = intersection;
		return true;
	}
	return false;
}

//
// Calculates altitude in terms of the sides
// http://en.wikipedia.org/wiki/Altitude_(triangle)
//
//  @inputs: the length of 3 triangle sides
//  @returns the altitude  from side a
Scalar triangleAltitude(Scalar a, Scalar b, Scalar c)
{
	Scalar s = 0.5 * (a+b+c);
	Scalar h = 2 * SCALAR_SQRT( s*(s-a)*(s-b)*(s-c) ) /a;
	return h;
}

// an edged has collapsed when its 2 bisectors intersect
// at an altitude that is lower than the inset distance
bool edgeCollapse(const LineSegment2& segment,
					const Vector2& bisector0,
					const Vector2& bisector1,
					Scalar elongation,
					Scalar &collapseDistance)
{
	// segment is the base of the triangle
	// from which we want the altitude

	LineSegment2 bisectorSegment0;
	bisectorSegment0.a = segment.a;
	bisectorSegment0.b = segment.a + bisector0;

	LineSegment2 bisectorSegment1;
	bisectorSegment1.a = segment.b + bisector1;
	bisectorSegment1.b = segment.b;

	LineSegment2 s0 = elongate(bisectorSegment0, elongation);
	LineSegment2 s1 = prelongate(bisectorSegment1, elongation);
	Vector2 intersection;
	bool attached = segmentSegmentIntersection(s0, s1, intersection);
	if(attached)
	{
		// the triangle is made from
		Vector2 edge0 = segment.a -intersection;
		Vector2 edge1 = segment.b -intersection;
		Scalar a, b,c;
		a = segment.length();
		b = edge0.magnitude();
		c = edge1.magnitude();

		collapseDistance = triangleAltitude(a,b,c);
		if(collapseDistance < 0) {
			assert(0);
		}
		return true;
	}
	return false;
}

void outMap(const std::multimap<Scalar, unsigned int> &collapsingSegments)
{
	cout << "collapse distance\tsegment id" << endl;
	cout << "--------------------------------" << endl;
	for(std::multimap<Scalar, unsigned int>::const_iterator it= collapsingSegments.begin();
			it != collapsingSegments.end(); it++)
	{
		const std::pair<Scalar, unsigned int>& seg = *it;
		cout << "\t" <<seg.first<< ",\t" << seg.second << endl;
	}
}



Scalar removeFirstCollapsedSegments(	const std::vector<LineSegment2> &originalSegments,
									const std::vector<Vector2> &bisectors,
									Scalar insetDist,
									std::vector<LineSegment2> &relevantSegments)
{
	Scalar elongation = 100;
	assert(relevantSegments.size()==0);

	relevantSegments.reserve(originalSegments.size());
	//cout << "NB of segments:" << originalSegments.size() << endl;

	multimap<Scalar, unsigned int> collapsingSegments;

	// cout << endl << "removeFirstCollapsedSegments:: looking for collapses" << endl;
	std::vector<LineSegment2> segments =  originalSegments;
	for (unsigned int i=0; i < segments.size(); i++)
	{
		unsigned int nextId = i==segments.size()-1 ? 0 : i+1;

		const LineSegment2 &nextSeg = segments[nextId];
		const Vector2 &nextBisector = bisectors[nextId];

		const LineSegment2 &currentSegment =  segments[i];
		const Vector2 &currentBisector = bisectors[i];

		Scalar collapseDistance;
		// check
		//cout << "segment[" << i << "] = " << currentSegment << endl;
		bool collapsed = edgeCollapse(	currentSegment,
										currentBisector,
										nextBisector,
										elongation,
										collapseDistance);
		if(collapsed)
		{
			//cout << " **  segment " << i << " ,collapse distance " <<  collapseDistance << endl;
			if(collapseDistance < insetDist)
			{
				// shortestCollapseDistance = collapseDistance;
				collapsingSegments.insert(std::pair<Scalar, unsigned int>(collapseDistance, i));
			}
		}
	}

	if(collapsingSegments.empty())
	{
		// no problem... inset all the way!
		for (unsigned int i=0; i < segments.size(); i++)
		{
			relevantSegments.push_back(segments[i]);
		}
		return insetDist;
	}

	// otherwise...

	std::multimap<Scalar, unsigned int>::iterator collapserator = collapsingSegments.begin();
	Scalar collapseDistance = (*collapserator).first;
	//cout << "COLLAPSED ID " << firstCollapse << endl;
	std::set<unsigned int> toRemove;

	// cout << "removeFirstCollapsedSegments:: who to remove" << endl;
	bool done = false;
	do
	{
		Scalar d = (*collapserator).first;
		unsigned int segmentId = (*collapserator).second;
		//cout << "  " << d << ": Removing collapsed segment[" << segmentId <<"]=" << segments[segmentId] << endl;
		toRemove.insert(segmentId);
		collapserator++;
		if(collapserator == collapsingSegments.end() )
		{
			done = true;
		}
		if(d > collapseDistance )
		{
		//	cout << "d(" << d << ") > collapseDistance (" << collapseDistance << endl;
			done = true;
		}
	}
	while(!done);

	//cout << "removeFirstCollapsedSegments:: making new list" << endl;
	for (unsigned int i=0; i < segments.size(); i++)
	{
		if(toRemove.find(i) ==  toRemove.end() )
		{
			relevantSegments.push_back(segments[i]);
		}
	}
	return collapseDistance;
}

// True if the 3 points are collinear
bool collinear(const Vector2 &a, const Vector2 &b, const Vector2 &c, Scalar tol)
{
	Scalar dot = ((b[0]- a[0]) * (c[1] - a[1]) - (c[0] - c[0]) * (b[1] - a[1]));
	bool r = tequals(dot, 0, tol);
	return r;
}

void elongateAndTrimSegments(const std::vector<LineSegment2> & longSegments,
					Scalar elongation,
					std::vector<LineSegment2> &segments)
{
	Scalar tol = 1e-6;


	segments = longSegments;
	for(unsigned int i = 0; i < segments.size(); i++)
	{
		unsigned int prevId = i==0 ? segments.size()-1 : i-1;

		LineSegment2 &previousSegment = segments[prevId];
		LineSegment2 &currentSegment =  segments[i];

		//cout << "prev: seg[" << prevId << "] = " << previousSegment << endl;
		//cout << "cur:  seg[" << i << "] = " << currentSegment << endl;

		if (previousSegment.b.tequals(currentSegment.a, tol))
		{
			// the job is already done.. segments are attached,
			// nothing to see
			// cout << "already attached" << endl;
			continue;
		}

		if(previousSegment.length()==0)
		{
			cout << "X";
			continue;
		}

		if(currentSegment.length()==0)
		{
			cout << "Y";
			continue;
		}


/*
		if(collinear(previousSegment.a, currentSegment.a, currentSegment.b, tol ))
		{
			Vector2 m = (previousSegment.a + currentSegment.b) * 0.5;
			previousSegment.b = m;
			currentSegment.a = m;
			continue;
		}
*/
		bool attached = attachSegments(previousSegment, currentSegment, elongation);
		if(!attached)
		{
			cout << "!";
			Vector2 m = (previousSegment.a + currentSegment.b) * 0.5;
			previousSegment.b = m;
			currentSegment.a = m;

		}
		// cout << "attach point " << currentSegment.a << endl;
		//cout << endl;
	}
}



void createBisectors(const std::vector<LineSegment2>& segments,
						Scalar tol,
						std::vector<Vector2>  &motorCycles)
{
	for(unsigned int i=0; i < segments.size(); i++)
	{
		unsigned int prevId = i==0 ? segments.size()-1 : i-1;

		const LineSegment2 &prevSeg = segments[prevId];
		const LineSegment2 &seg = segments[i];

		Vector2 prevInset = getInsetDirection(prevSeg);
		Vector2 inset = getInsetDirection(seg);

		Vector2 bisector = inset;

		// if points are disjoint, do not combine both insets
		if(prevSeg.b.tequals(seg.a, tol) )
		{
			bisector += prevInset;
		}
		else
		{
			//
			// ok... maybe this is a bit drastic and we could combine the biesctors
			// this author needs to make up his mind about non closed polygon support
			//
			Vector2 dist = prevSeg.b - seg.a;
			stringstream ss;
			ss << "This is not a closed polygon. segment[" << prevId << "].b = " << prevSeg.b ;
			ss << " and segment[" << i << "].a = " << seg.a << " are distant by " << dist.magnitude();
			ss << endl;
			ScadTubeFile::segment3(ss,"","segments", segments, 0, 0.1);
			SHOUT("O")
			ShrinkyException mixup(ss.str().c_str());
			throw mixup;
			// assert(0);
		}
		if(bisector.squaredMagnitude() == 0)
		{
			stringstream ss;
			ss << "Null bisector at segment [" << i << "] position=" << seg.a << endl;
			ss << " previous_inset=" << prevInset << " inset=" << inset;
			SHOUT("N")
			ShrinkyException mixup(ss.str().c_str());
			throw mixup;
		}
		bisector.normalise();

		motorCycles.push_back(bisector);
	}
}


void Shrinky::writeScadBisectors(const std::vector<Vector2> & bisectors, const std::vector<LineSegment2> & originalSegments)
{
    if(scadFileName){
        std::vector<LineSegment2> motorCycleTraces;
        for(size_t i = 0;i < bisectors.size();i++){
            Vector2 a = originalSegments[i].a;
            Vector2 dir = bisectors[i];
            dir *= 2;
            Vector2 b = a + dir;
            LineSegment2 s(a, b);
            motorCycleTraces.push_back(s);
        }
        scadZ = fscad.writeSegments3("bisectors_", "color([0.75,0.5,0.2,1])loop_segments3", motorCycleTraces, scadZ, dz, this->counter);
    }

}

void Shrinky::writeScadSegments(const char* segNames,
								const char* prefix,
								const std::vector<LineSegment2> & segments)
{
	if(scadFileName)
	{
		string funcName = prefix;
		funcName += "loop_segments3";
		scadZ = fscad.writeSegments3(segNames, funcName.c_str(), segments, scadZ, dz, this->counter);
	}
}

void Shrinky::inset(const std::vector<LineSegment2>& originalSegments,
								Scalar insetDist,
								std::vector<LineSegment2> &finalInsets)
{
	bool writePartialSteps = true;

	int count = originalSegments.size();

	if(count < 2)
	{
		assert(0);
	}
	assert(finalInsets.size() ==0);

	finalInsets.reserve(originalSegments.size());

	Scalar tol = 1e-6; // for continuity testing and distance to go
	Scalar distanceToGo = insetDist;

	std::vector<LineSegment2> initialSegs = originalSegments;

	bool done = false;
	//cout << "INSET " << endl;
	while (!done)
	{
		connectivityCheck(initialSegs, tol);

		//cout << " ** distance to go: " <<  distanceToGo << endl;
		finalInsets.clear();

		Scalar distanceGone = insetStep(initialSegs, distanceToGo, tol, writePartialSteps, finalInsets);

		distanceToGo -= distanceGone;
		if( tequals(distanceToGo, 0, tol))
		{
		//	cout << " ** INSET Done!" << endl;
			done = true;
			return;
		}
		if(finalInsets.size() > 2)
		{
			initialSegs = finalInsets;
		}
		else
		{
		//	cout << " ** INSET stopped." << endl;
			return;
		}
	}
	//cout << "Thank you:  " << finalInsets.size() << endl;
}

void removeZeroLengthSegments(const std::vector<LineSegment2> &inputSegments, std::vector<LineSegment2> &segments, Scalar tol)
{

	assert(inputSegments.size() > 0);
	segments.reserve(inputSegments.size());
	// deep copy
	for(unsigned int i = 0; i < inputSegments.size(); i++)
	{
		const LineSegment2 &seg = inputSegments[i];
  		if(tequals(seg.squaredLength(), 0, tol) )
		{
			continue;
		}
		segments.push_back(seg);
	}
}

Scalar Shrinky::insetStep(const std::vector<LineSegment2>& originalSegments,
								Scalar insetDist,
								Scalar continuityTolerance,
								bool writePartialStep,
								std::vector<LineSegment2> &finalInsets)
{
	Scalar tol = 1e-6;
	// magic numbers
	Scalar elongation = insetDist * 100; // continuityTolerance * 5;

	unsigned int segmentCount = originalSegments.size();

	assert(segmentCount >0);
    assert(finalInsets.size() == 0);
	assert(&originalSegments != &finalInsets);

	if(segmentCount < 2)
	{
		stringstream ss;
		ss <<  "1 line segment is not enough to create a closed polygon";
		assert(0);
		ShrinkyException mixup(ss.str().c_str());
		throw mixup;
	}

	bool dumpSteps = false;

	if(dumpSteps)segmentsDiagnostic("originalSegments", originalSegments);

	Scalar insetStepDistance;
	try
	{
	    if(scadFileName)
	    {
	        // OpenScad
	        Scalar dz = 0.1;
	        stringstream coloredOutline;
	        // Scalar color = (1.0 * i)/(shells-1);
	        int color = 0;
	        coloredOutline << "color([" << color << "," << color << "," << 1 - color << " ,1])";
	        Scalar dzBefore = scadZ;
	        writeScadSegments("outlines_",coloredOutline.str().c_str(), originalSegments );
	        // trick to get the bisector in place
	        scadZ = dzBefore;
	    }

	    std::vector<LineSegment2> relevantSegments;
	    if(originalSegments.size()>2)
	    {
	    	//cout << "...BISECTING..." << endl;
	    	std::vector<Vector2> bisectors;
	    	createBisectors(originalSegments, continuityTolerance, bisectors);
	    	writeScadBisectors(bisectors, originalSegments);

	    	//cout << "...COLLAPSING..." << endl;
	    	insetStepDistance =  removeFirstCollapsedSegments(originalSegments, bisectors, insetDist, relevantSegments);
	    	if(dumpSteps) segmentsDiagnostic("relevantSegments",relevantSegments);
	    	writeScadSegments("relevants_", "color([0.5,0.5,0,1])", relevantSegments);
	    }

	    std::vector<LineSegment2> insets;
		unsigned int relevantCount = relevantSegments.size();
		if( relevantCount > 2)
		{
			//cout << "...INSETTING..." << endl;
			insetSegments(relevantSegments, insetStepDistance, insets);
			if(dumpSteps) segmentsDiagnostic("Insets", insets);
			writeScadSegments("raw_insets_", "color([1,0,0.4,1])", insets);
			lengthCheck(insets, "insets");
		}


		std::vector<LineSegment2> connected;
		if(insets.size()>2)
		{
			//cout << "...ATTACHING..." << endl;
			elongateAndTrimSegments(insets, elongation, connected);
			writeScadSegments("connected_", "color([0.25,0.25,0.25,1])", connected);
			// lengthCheck(finalInsets, "finalInsets");
		}

		if(connected.size()>2)
		{
			removeZeroLengthSegments(connected, finalInsets, tol);
			writeScadSegments("final_insets_", "color([0.5,0.5,0.5,1])", connected);
		}

	}
	catch(ShrinkyException &mixup)
	{
		mixup;
		cout << " ^ "; //  << mixup.error << endl;

		// cout << "ABORT MISSION!!! " << insetStepDistance << ": " << mixup.error << endl;
		// this is a lie...  but we want to break the loop
		insetStepDistance = insetDist;
		throw;
	}
	this->counter++;
    return insetStepDistance;
}


void Shrinky::closeScadFile()
{
    if(scadFileName)
    {
        std::ostream & out = fscad.getOut();
        int shells = counter;
        fscad.writeMinMax("draw_outlines", "outlines_", shells);
        fscad.writeMinMax("draw_bisectors", "bisectors_", shells);
        fscad.writeMinMax("draw_raw_insets", "raw_insets_", shells);
        //fscad.writeMinMax("draw_trimmed_insets",  "trimmed_insets_", shells);
        fscad.writeMinMax("draw_relevants", "relevants_", shells);
        fscad.writeMinMax("draw_final_insets", "final_insets_", shells);
        out << "min=0;" << endl;
        out << "max=" << shells-1  << ";" << std::endl;
        out << std::endl;
        out << "draw_outlines(min, max);" << std::endl;
        out << "draw_bisectors(min, max);" << std::endl;
        out << "draw_relevants(min, max);" << std::endl;
        out << "draw_raw_insets(min, max);" << std::endl;
        out << "draw_final_insets(min, max);" << std::endl;
        out << endl;
        out << "// s = [\"segs.push_back(TriangleSegment2(Vector2(%s+x, %s+y), Vector2(%s+x, %s+y)));\" %(x[0][0], x[0][1], x[1][0], x[1][1]) for x in segments]" << std::endl;
        out << "// print '\\n'.join(s) " << endl;
        fscad.close();
    }
}

Shrinky::~Shrinky()
{
    closeScadFile();
}



void mgl::createShellsForSliceUsingShrinky( const SegmentTable & outlinesSegments,
									   const std::vector<Scalar> &insetDistances,
									   unsigned int sliceId,
									   const char *scadFile,
									   bool writeDebugScadFiles,
									   std::vector<SegmentTable> & insetsForLoops)

{
	assert(insetsForLoops.size() ==0);
	unsigned int nbOfShells = insetDistances.size();



	// dbgs__( "outlineSegmentCount " << outlineSegmentCount)
    for(unsigned int outlineId=0; outlineId < outlinesSegments.size(); outlineId++)
	{
    	const std::vector<LineSegment2> &outlineLoop = outlinesSegments[outlineId];
    	assert(outlineLoop.size() > 0);

		insetsForLoops.push_back(SegmentTable());
		assert(insetsForLoops.size() == outlineId + 1);

		SegmentTable &insetTable = *insetsForLoops.rbegin(); // inset curves for a single loop
		insetTable.reserve(nbOfShells);
		for (unsigned int shellId=0; shellId < nbOfShells; shellId++)
		{
			insetTable.push_back(std::vector<LineSegment2>());
		}

		unsigned int segmentCountBefore =0;
		unsigned int segmentCountAfter =0;

		unsigned int currentShellIdForErrorReporting=0;
		try
		{

			Shrinky shrinky;
			const vector<LineSegment2> *previousInsets  = &outlineLoop;
			for (unsigned int shellId=0; shellId < nbOfShells; shellId++)
			{
				currentShellIdForErrorReporting = shellId;
				Scalar insetDistance = insetDistances[shellId];
				std::vector<LineSegment2> &insets = insetTable[shellId];
				if((*previousInsets).size() > 2)
				{
					shrinky.inset(*previousInsets, insetDistance, insets);
					previousInsets = &insets;
				}
			}
		}
		catch(ShrinkyException &messup)
		{
			if(writeDebugScadFiles)
			{
				static int counter =0;
				cout << endl;
				cout << "----- ------ ERROR " << counter <<" ------ ------"<< endl;
				cout << "sliceId: " <<  sliceId   << endl;
				cout << "loopId : " <<  outlineId << endl;
				cout << "shellId: " <<  currentShellIdForErrorReporting   << endl;

				stringstream ss;
				ss << "_slice_" << sliceId << "_loop_" << outlineId << ".scad";

				MyComputer myComputer;
				string loopScadFile = myComputer.fileSystem.ChangeExtension(scadFile, ss.str().c_str());
				Shrinky shriker(loopScadFile.c_str());
				shriker.dz=0.1;
				try
				{
					std::ostream &scad = shriker.fscad.getOut();
					scad << "/*" << endl;
					scad << messup.error;
					scad << endl << "*/" << endl;


					vector<LineSegment2> previousInsets  = outlineLoop;
					cout << "Creating file: " << loopScadFile << endl;
					cout << "	Number of points " << previousInsets.size() << endl;
					ScadTubeFile::segment3(cout,"","segments", previousInsets, 0, 0.1);
					std::vector<LineSegment2> insets;
					for (unsigned int shellId=0; shellId < nbOfShells; shellId++)
					{
						Scalar insetDistance = insetDistances[shellId];
						shriker.inset(previousInsets, insetDistance, insets);
						previousInsets = insets;
						insets.clear(); // discard...
					}
				}
				catch(ShrinkyException &messup2) // the same excpetion is thrown again
				{
					messup2; //ignore
					cout << "saving " << endl;
				}
				cout << "--- --- ERROR " << counter << " END --- ----" << endl;
				counter ++;
			}
		}
	}
}
