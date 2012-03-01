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

std::ostream& mgl::operator << (std::ostream &os, const TriangleSegment2 &s)
{
	os << "[ " << s.a << ", " << s.b << "]";
	return os;
}

void connectivityCheck( const std::vector<TriangleSegment2> &segments, Scalar tol,
						std::vector<TriangleSegment2> &outs)
{
	outs.reserve(segments.size());
	for(unsigned int i=0; i < segments.size(); i++)
	{
		unsigned int prevId = i==0 ? segments.size()-1 : i-1;
		const TriangleSegment2 &prevSeg = segments[prevId];
		TriangleSegment2 seg = segments[i];

		if(!prevSeg.b.sameSame(seg.a, tol) )
		{
			Vector2 dist = prevSeg.b - seg.a;
			stringstream ss;
			ss << "Connectivity error: segment[" << prevId << "] = " << prevSeg << endl;
			ss << " and segment[" << i << "] = " << seg << endl;
			ss << "	segment[" << prevId << "] length = " << prevSeg.length() << endl;
			ss << " segment[" << i << "] length = " << seg.length() << endl;
			ss << " Distance between segments " << dist.magnitude();

			ss << endl;
			ScadTubeFile::segment3(ss,"","segments", segments, 0, 0.1);
			ShrinkyMess mixup(ss.str().c_str());
			throw mixup;
			// assert(0);
//			TriangleSegment2 newSeg(prevSeg.b, seg.a);
	//	outs.push_back(newSeg);
			//seg.a = prevSeg.b;
		}
		outs.push_back(seg);
	}
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

TriangleSegment2 elongate(const TriangleSegment2 &s, Scalar dist)
{
	TriangleSegment2 segment(s);
	Vector2 l = segment.b - segment.a;
	l.normalise();
	l *= dist;
	segment.b += l;
	return segment;
}



TriangleSegment2 prelongate(const TriangleSegment2 &s, Scalar dist)
{
	TriangleSegment2 segment(s);
	Vector2 l = segment.a - segment.b;
	l.normalise();
	l *= dist;
	segment.a += l;
	return segment;
}


TriangleSegment2 elongateAndPrelongate(const TriangleSegment2 &s, Scalar dist)
{
	TriangleSegment2 segment(s);
	Vector2 l = segment.b - segment.a;
	l.normalise();
	l *= dist;
	segment.b += l;
	segment.a -= l;
	return segment;

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
		seg.a += inset;
		seg.b += inset;
		insets.push_back(seg);
	}
	assert(insets.size() == segments.size());
}

string segment3(const TriangleSegment2 &s, Scalar z)
{
	stringstream ss;
	ss << "[[" << s.a[0] << ", " << s.a[1] << ", " << z << "], [" <<  s.b[0] << ", " << s.b[1] << ", " << z << "]]";
	return ss.str();
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

	Scalar cutoffLength2 = cutoffLength * cutoffLength;

	for(unsigned int i=0; i < segments.size(); i++)
	{
		const TriangleSegment2 &seg = segments[i];
		Scalar length2 = seg.squaredLength();
		if(length2 > cutoffLength2)
		{
			assert(seg.length() >0);
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


bool attachSegments(TriangleSegment2 &first, TriangleSegment2 &second, Scalar elongation)
{
	TriangleSegment2 s0 = elongateAndPrelongate(first, elongation); // elongate(first, elongation);
	TriangleSegment2 s1 = elongateAndPrelongate(second, elongation); //prelongate(second, elongation);
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

// http://en.wikipedia.org/wiki/Altitude_(triangle)
// see Altitude in terms of the sides
//
// returns the altitude  from side a
// imputs: the lenght of each side
Scalar triangleAltitude(Scalar a, Scalar b, Scalar c)
{
	Scalar s = 0.5 * (a+b+c);
	Scalar h = 2 * SCALAR_SQRT( s*(s-a)*(s-b)*(s-c) ) /a;
	return h;
}

// an edged has collapsed when its 2 bisectors intersect
// at an altitude that is lower than the inset distance
bool edgeCollapse(const TriangleSegment2& segment,
					const Vector2& bisector0,
					const Vector2& bisector1,
					Scalar elongation,
					Scalar &collapseDistance)
{


	// segment is the base of the triangle
	// from which we want the altitude

	TriangleSegment2 bisectorSegment0;
	bisectorSegment0.a = segment.a;
	bisectorSegment0.b = segment.a + bisector0;

	TriangleSegment2 bisectorSegment1;
	bisectorSegment1.a = segment.b + bisector1;
	bisectorSegment1.b = segment.b;

	TriangleSegment2 s0 = elongate(bisectorSegment0, elongation);
	TriangleSegment2 s1 = prelongate(bisectorSegment1, elongation);
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

		assert(collapseDistance>0);
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

int popFirstElem(std::multimap<Scalar, unsigned int> &collapsingSegments)
{
	if(collapsingSegments.empty())
		return -1;

	std::multimap<Scalar, unsigned int>::iterator b = collapsingSegments.begin();
	unsigned int result =  (*b).second;
	collapsingSegments.erase(b);
	return result;
}



Scalar removeFirstCollapsedSegments(	const std::vector<TriangleSegment2> &originalSegments,
									const std::vector<Vector2> &bisectors,
									Scalar insetDist,
									std::vector<TriangleSegment2> &relevantSegments)
{
	Scalar elongation = 100;
	assert(relevantSegments.size()==0);

	relevantSegments.reserve(originalSegments.size());
	//cout << "NB of segments:" << originalSegments.size() << endl;

	multimap<Scalar, unsigned int> collapsingSegments;

	// cout << endl << "removeFirstCollapsedSegments:: looking for collapses" << endl;
	std::vector<TriangleSegment2> segments =  originalSegments;
	for (unsigned int i=0; i < segments.size(); i++)
	{
		unsigned int nextId = i==segments.size()-1 ? 0 : i+1;

		const TriangleSegment2 &nextSeg = segments[nextId];
		const Vector2 &nextBisector = bisectors[nextId];

		const TriangleSegment2 &currentSegment =  segments[i];
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


bool collinear(const TriangleSegment2 &prev, const TriangleSegment2 &current, Scalar tol, Vector2 &mid)
{

	Scalar x1 = prev.a[0];
	Scalar y1 = prev.a[1];
	mid.x = 0.5 * (prev.b[0] + current.a[0]);
	mid.y = 0.5 * (prev.b[1] + current.a[1]);
	Scalar x3 = current.b[0];
	Scalar y3 = current.b[1];

	Scalar c = ((mid.x - x1) * (y3 - y1) - (x3 - x1) * (mid.y - y1));
	bool r = sameSame(c, 0, tol);
	return r;
}

void elongateAndTrimSegments(const std::vector<TriangleSegment2> & longSegments,
					Scalar elongation,
					std::vector<TriangleSegment2> &segments)
{
	Scalar tol = 1e-6;
	assert(longSegments.size() > 0);

//	std::vector<TriangleSegment2> segments =longSegments;
//	segments.reserve(longSegments.size());
//	cout<< "****" << endl;

	// deep copy
	std::vector<TriangleSegment2>* p =  &segments;
	(*p) = longSegments;


	assert(longSegments.size() == segments.size());

	for(unsigned int i = 0; i < segments.size(); i++)
	{
		unsigned int prevId = i==0 ? segments.size()-1 : i-1;

		TriangleSegment2 &previousSegment = segments[prevId];
		TriangleSegment2 &currentSegment =  segments[i];

		if(previousSegment.length()==0)
			continue;
		if(currentSegment.length()==0)
			continue;


		if (previousSegment.b.sameSame(currentSegment.a, tol))
			continue;

		bool attached = attachSegments(previousSegment, currentSegment, elongation);
		if(!attached)
		{
			Scalar elong =10;
			cout << "// ATTACH ERROR in elongateAndTrimSegments!" << endl;
			cout << "module elongation()" <<  endl;
			cout << "{" << endl;
			cout << endl;
			cout << "	// ATTACH ERROR!" << endl;
			cout << "   d1 = 0.1;" << endl;
			cout << "   d2 = 0.01;" << endl;
			cout << "	previousSegment = " << previousSegment <<";" <<  endl;
			cout << "	prev_length= " << previousSegment.length() <<";" <<  endl;
			cout << "	currentSegment = " << currentSegment << ";" << endl;
			cout << "	curr_length= " << currentSegment.length() << ";" << endl;
			TriangleSegment2 s0 = elongateAndPrelongate(previousSegment, elong );
			TriangleSegment2 s1 = elongateAndPrelongate(currentSegment, elong );
			cout << "	s0 = " << s0 << ";" << endl;
			cout << "	s1 = " << s1 << ";" << endl;
			cout << "	elongation = " << elongation << ";" << endl;

			cout << "	translate([0,0,0])" << endl;
			cout << "	{" << endl;
			cout << "		tube(x1=previousSegment[0][0], y1=previousSegment[0][1], z1=0, x2=previousSegment[1][0], y2=previousSegment[1][1], z2=0 ,  diameter1=d1, diameter2=d2, faces=4, thickness_over_width=1);" << endl;
			cout << "		tube(x1=currentSegment[0][0], y1=currentSegment[0][1], z1=0, x2=currentSegment[1][0], y2=currentSegment[1][1], z2=0 ,  diameter1=d1, diameter2=d2, faces=4, thickness_over_width=1);" << endl;
			cout << "	}" << endl;
			cout << "" << endl;
			cout << "	translate([0, 0,0.1])" << endl;
			cout << "	{" << endl;
			cout << "		tube(x1=s0[0][0], y1=s0[0][1], z1=0, x2=s0[1][0], y2=s0[1][1], z2=0 ,  diameter1=d1, diameter2=d2, faces=4, thickness_over_width=1);" << endl;
			cout << "		tube(x1=s1[0][0], y1=s1[0][1], z1=0, x2=s1[1][0], y2=s1[1][1], z2=0 ,  diameter1=d1, diameter2=d2, faces=4, thickness_over_width=1);" << endl;
			cout << "	" << endl;
			cout << "	}" << endl;
			cout << ""  << endl;
			cout << "}" << endl;
			cout << "//----" << endl << endl;

		}
	}

}


void createBisectors(const std::vector<TriangleSegment2>& segments,
						Scalar tol,
						std::vector<Vector2>  &motorCycles)
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
			Vector2 dist = prevSeg.b - seg.a;
			stringstream ss;
			ss << "This is not a closed polygon. segment[" << prevId << "].b = " << prevSeg.b ;
			ss << " and segment[" << i << "].a = " << seg.a << " are distant by " << dist.magnitude();
			ss << endl;
			ScadTubeFile::segment3(ss,"","segments", segments, 0, 0.1);
			ShrinkyMess mixup(ss.str().c_str());
			throw mixup;
			// assert(0);
		}
		if(bisector.squaredMagnitude() == 0)
		{
			stringstream ss;
			ss << "Null bisector";
			ShrinkyMess mixup(ss.str().c_str());
			throw mixup;
		}
		bisector.normalise();

		motorCycles.push_back(bisector);
	}
}

void Shrinky::writeScadBisectors(const std::vector<Vector2> & bisectors, const std::vector<TriangleSegment2> & originalSegments)
{
    if(scadFileName){
        std::vector<TriangleSegment2> motorCycleTraces;
        for(int i = 0;i < bisectors.size();i++){
            Vector2 a = originalSegments[i].a;
            Vector2 dir = bisectors[i];
            dir *= 2;
            Vector2 b = a + dir;
            TriangleSegment2 s(a, b);
            motorCycleTraces.push_back(s);
        }
        scadZ = fscad.writeSegments3("motorcycles_", "color([0.75,0.5,0.2,1])loop_segments3", motorCycleTraces, scadZ, dz, this->counter);
    }

}

void Shrinky::writeScadSegments(const char* segNames,
								const char* prefix,
								const std::vector<TriangleSegment2> & segments)
{
	if(scadFileName)
	{
		string funcName = prefix;
		funcName += "loop_segments3";
		scadZ = fscad.writeSegments3(segNames, funcName.c_str(), segments, scadZ, dz, this->counter);
	}
}

void Shrinky::inset(const std::vector<TriangleSegment2>& originalSegments,
								Scalar insetDist,
								std::vector<TriangleSegment2> &finalInsets)
{
	bool writePartialSteps = true;

	int count = originalSegments.size();

	assert(count>0);
	assert(finalInsets.size() ==0);

	finalInsets.reserve(originalSegments.size());

	Scalar tol = 1e-6; // for continuity testing and distance to go
	Scalar distanceToGo = insetDist;

	std::vector<TriangleSegment2> initialSegs = originalSegments;

	bool done = false;
	//cout << "INSET " << endl;
	while (!done)
	{
		std::vector<TriangleSegment2> connected;
		connectivityCheck(initialSegs, tol, connected);

		//cout << " ** distance to go: " <<  distanceToGo << endl;
		finalInsets.clear();

		Scalar distanceGone = insetStep(connected, distanceToGo, tol, writePartialSteps, finalInsets);

		distanceToGo -= distanceGone;
		if( sameSame(distanceToGo, 0, tol))
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

Scalar Shrinky::insetStep(const std::vector<TriangleSegment2>& originalSegments,
								Scalar insetDist,
								Scalar continuityTolerance,
								bool writePartialStep,
								std::vector<TriangleSegment2> &finalInsets)
{
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
		ShrinkyMess mixup(ss.str().c_str());
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
	        int color = color == 0 ? 1 : 0;
	        coloredOutline << "color([" << color << "," << color << "," << 1 - color << " ,1])";
	        Scalar dzBefore = scadZ;
	        writeScadSegments("outlines_",coloredOutline.str().c_str(), originalSegments );
	        // trick to get the bisector in place
	        scadZ = dzBefore;
	    }

	    std::vector<TriangleSegment2> relevantSegments;
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

	    std::vector<TriangleSegment2> insets;
		unsigned int relevantCount = relevantSegments.size();
		if( relevantCount > 2)
		{
			//cout << "...INSETTING..." << endl;
			insetSegments(relevantSegments, insetStepDistance, insets);
			if(dumpSteps) segmentsDiagnostic("Insets", insets);
			writeScadSegments("raw_insets_", "color([1,0,0.4,1])", insets);
		}


		std::vector<TriangleSegment2> trims;
		if(insets.size()>2)
		{
			//cout << "...ATTACHING..." << endl;
			elongateAndTrimSegments(insets, elongation, finalInsets);
			writeScadSegments("trim_insets_", "color([1,0,0.4,1])", trims);
		}

	}
	catch(ShrinkyMess &mixup)
	{
		cout << "ABORT MISSION!!! " << insetStepDistance << ": " << mixup.error << endl;
		// this is a lie...  but we want to break the loop
		insetStepDistance = insetDist;
		throw;
	}
	this->counter++;
    return insetStepDistance;
}


void Shrinky::closeScadFile()
{
    if(scadFileName){
        std::ostream & out = fscad.getOut();
        int shells = counter;
        fscad.writeMinMax("draw_outlines", "outlines_", shells);
        fscad.writeMinMax("draw_motorcycles", "motorcycles_", shells);
        fscad.writeMinMax("draw_raw_insets", "raw_insets_", shells);
        //fscad.writeMinMax("draw_trimmed_insets",  "trimmed_insets_", shells);
        fscad.writeMinMax("draw_relevants", "relevants_", shells);
        fscad.writeMinMax("draw_final_insets", "final_insets_", shells);
        out << "min=0;" << endl;
        out << "max=" << shells-1  << ";" << std::endl;
        out << std::endl;
        out << "draw_outlines(min, max);" << std::endl;
        out << "draw_motorcycles(min, max);" << std::endl;
        out << "draw_relevants(min, max);" << std::endl;
        out << "draw_raw_insets(min, max);" << std::endl;
        //out << "draw_trimmed_insets(min, max);" <<std::endl;
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
