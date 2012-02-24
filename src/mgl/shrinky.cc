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

void connectivityCheck(const std::vector<TriangleSegment2> &segments, Scalar tol)
{
	for(unsigned int i=0; i < segments.size(); i++)
	{
		unsigned int prevId = i==0 ? segments.size()-1 : i-1;
		const TriangleSegment2 &prevSeg = segments[prevId];
		const TriangleSegment2 &seg = segments[i];

		if(!prevSeg.b.sameSame(seg.a, tol) )
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

	Scalar cutoffLength2 = cutoffLength * cutoffLength;

	for(unsigned int i=0; i < segments.size(); i++)
	{
		unsigned int prevId = i==0 ? segments.size()-1 : i-1;
		unsigned int nextId = i==segments.size()-1 ? 0 : i+1;

		const TriangleSegment2 &prevSeg = segments[prevId];
		const TriangleSegment2 &seg = segments[i];
		const TriangleSegment2 &nextSeg = segments[nextId];
		Scalar length2 = nextSeg.squaredLength();
		TriangleSegment2 newSeg(seg);

		if(length2 < cutoffLength2)
		{
			newSeg.b = nextSeg.b;
			i++; // skip one
		}
		//else
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

void Shrinky::openScadFile(const char *scadFileName)
{
    if(scadFileName){
        fscad.open(scadFileName);
        std::ofstream & out = fscad.getOut();
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

Shrinky::Shrinky( const char *scadFileName, // = NULL
					Scalar layerH) // =0.5
		:scadFileName(scadFileName), color(1),  counter(0), dz(0), z(0)
{
    openScadFile(scadFileName);
}


bool attachSegments(TriangleSegment2 &first, TriangleSegment2 &next, Scalar elongation)
{
	TriangleSegment2 s0 = elongate(first, elongation);
	TriangleSegment2 s1 = prelongate(next, elongation);
	Vector2 intersection;
	bool trimmed = segmentSegmentIntersection(s0, s1, intersection);
	if(trimmed)
	{
		first.b = intersection;
		next.a = intersection;
		//cout <<  endl<<"attach success" << endl;
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
					Scalar insetDistance,
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

	bool attached = attachSegments(bisectorSegment0, bisectorSegment1, elongation);
	if(attached)
	{
		Vector2 intersection = bisectorSegment0.b;
		// the triangle is made from
		Vector2 edge0 = segment.a -intersection;
		Vector2 edge1 = segment.b -intersection;

		Scalar a, b,c;
		a = segment.length();
		b = edge0.magnitude();
		c = edge1.magnitude();

		collapseDistance = triangleAltitude(a,b,c);
		if(collapseDistance < insetDistance )
		{
			return true;
		}
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

void removeCollapsedSegments(	const std::vector<TriangleSegment2> &originalSegments,
								const std::vector<Vector2> &bisectors,
								Scalar insetDist,
								std::vector<TriangleSegment2> &relevantSegments)
{
	Scalar elongation = 100;

	assert(relevantSegments.size()==0);

	relevantSegments.reserve(originalSegments.size());
	cout << "segments before:" << originalSegments.size() << endl;



	multimap<Scalar, unsigned int> collapsingSegments;

	std::vector<TriangleSegment2> segments =  originalSegments;
	for (unsigned int i=0; i < segments.size(); i++)
	{
		unsigned int nextId = i==segments.size()-1 ? 0 : i+1;
		const TriangleSegment2 &nextSeg = segments[nextId];
		const TriangleSegment2 &currentSegment =  segments[i];

		Scalar collapseDistance;
		// check
		bool collapsed = edgeCollapse(	currentSegment,
										bisectors[i],
										bisectors[nextId],
										insetDist,
										elongation,
										collapseDistance);
		if(collapsed)
		{
			if(collapseDistance < insetDist)
			{
				// shortestCollapseDistance = collapseDistance;
				cout << " segment " << i << " ,collapse distance " <<  collapseDistance << endl;
				collapsingSegments.insert(std::pair<Scalar, unsigned int>(collapseDistance, i));
			}
		}
		// do nothing

	}
	int firstCollapse = ( popFirstElem(collapsingSegments) );
	if( firstCollapse > -1)
	{
		cout << "Collapsing segments" << endl;
		for (unsigned int i=0; i < segments.size(); i++)
		{
			if(i != firstCollapse)
				relevantSegments.push_back(segments[i]);
		}
	}
	//collapsingSegments.erase(collapsingSegments.begin());

	cout << "segments after:" << relevantSegments.size() << endl;
}

Scalar removeFirstCollapsedSegment(	const std::vector<TriangleSegment2> &originalSegments,
									const std::vector<Vector2> &bisectors,
									Scalar insetDist,
									std::vector<TriangleSegment2> &relevantSegments)
{
	Scalar elongation = 100;

	assert(relevantSegments.size()==0);

	relevantSegments.reserve(originalSegments.size());
//	cout << "segments before:" << originalSegments.size() << endl;

	multimap<Scalar, unsigned int> collapsingSegments;

	std::vector<TriangleSegment2> segments =  originalSegments;
	for (unsigned int i=0; i < segments.size(); i++)
	{
		unsigned int nextId = i==segments.size()-1 ? 0 : i+1;
		const TriangleSegment2 &nextSeg = segments[nextId];
		const TriangleSegment2 &currentSegment =  segments[i];

		Scalar collapseDistance;
		// check
		bool collapsed = edgeCollapse(	currentSegment,
										bisectors[i],
										bisectors[nextId],
										insetDist,
										elongation,
										collapseDistance);
		if(collapsed)
		{
			// shortestCollapseDistance = collapseDistance;
			//cout << " segment " << i << " ,collapse distance " <<  collapseDistance << endl;
			if( collapseDistance < insetDist )
			{
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
	std::multimap<Scalar, unsigned int>::iterator b = collapsingSegments.begin();
	unsigned int firstCollapse = (*b).second;
	Scalar collapseDistance = (*b).first;

	for (unsigned int i=0; i < firstCollapse; i++)
	{
		relevantSegments.push_back(segments[i]);
	}
	for (unsigned int i=firstCollapse+1; i < segments.size(); i++)
	{
		relevantSegments.push_back(segments[i]);
	}

	return collapseDistance;
}



void elongateAndTrimSegments(const std::vector<TriangleSegment2> & longSegments,
					Scalar elongation,
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
		bool attached = attachSegments(previousSegment, currentSegment, elongation);
		if(!attached)
		{
			cout << "module elongation()" <<  endl;
			cout << "{" << endl;
			cout << endl;
			cout << "	// ATTACH ERROR!" << endl;
			cout << "	previousSegment = " << previousSegment <<";" <<  endl;
			cout << "	prev_length= " << previousSegment.length() <<";" <<  endl;
			cout << "	currentSegment = " << currentSegment << ";" << endl;
			cout << "	curr_length= " << currentSegment.length() << ";" << endl;
			TriangleSegment2 s0 = elongate(previousSegment, elongation );
			TriangleSegment2 s1 = prelongate(currentSegment, elongation );
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
		cout<< "BEFORE" << endl;
		connectivityCheck(initialSegs, tol);
		cout<< "AFTER BEFORE" << endl;
		//cout << " ** distance to go: " <<  distanceToGo << endl;
		finalInsets.clear();
		Scalar distanceGone = insetStep(initialSegs, distanceToGo, tol, writePartialSteps, finalInsets);

		distanceToGo -= distanceGone;
		if( sameSame(distanceToGo, 0, tol))
		{
			//cout << " ** INSET STOPPED!" << endl;
			done = true;
		}
		else
		{
			initialSegs = finalInsets;
		}
	}
	//cout << "Thank you:  " << finalInsets.size() << endl;
}

void Shrinky::writeScadInset(const std::vector<TriangleSegment2> & originalSegments,
							const std::vector<Vector2> &bisectors,
							const std::vector<TriangleSegment2> &relevantSegments,
							const std::vector<TriangleSegment2> &insets,
							const std::vector<TriangleSegment2> & finalInsets)
{
    if(scadFileName){
        // OpenScad
        Scalar scadZ = 0;
        Scalar dz = 0.1;
        stringstream coloredOutline;
        // Scalar color = (1.0 * i)/(shells-1);
        int color = color == 0 ? 1 : 0;
        coloredOutline << "color([" << color << "," << color << "," << 1 - color << " ,1])";
        coloredOutline << "loop_segments3";
        scadZ = fscad.writeSegments3("outlines_", coloredOutline.str().c_str(), originalSegments, scadZ, dz, this->counter);
        std::vector<TriangleSegment2> motorCycleTraces;
        for(int i = 0;i < bisectors.size();i++){
            Vector2 a = originalSegments[i].a;
            Vector2 dir = bisectors[i];
            dir *= 2;
            Vector2 b = a + dir;
            TriangleSegment2 s(a, b);
            motorCycleTraces.push_back(s);
        }
        scadZ = fscad.writeSegments3("motorcycles_", "color([0.75,0.5,0.2,1])loop_segments3", motorCycleTraces, 0, dz, this->counter);
        scadZ = fscad.writeSegments3("relevants_", "color([0.5,0.5,0,1])loop_segments3", relevantSegments, scadZ, dz, this->counter);
        //z += dz;
        scadZ = fscad.writeSegments3("raw_insets_", "color([1,0,0.4,1])loop_segments3", insets, scadZ, dz, this->counter);
        //scadZ += 2 * dz;
        scadZ = fscad.writeSegments3("final_insets_", "color([0,0.5,0,1])loop_segments3", finalInsets, scadZ, dz, this->counter);
        this->counter++;
    }

}

Scalar Shrinky::insetStep(const std::vector<TriangleSegment2>& originalSegments,
								Scalar insetDist,
								Scalar continuityTolerance,
								bool writePartialStep,
								std::vector<TriangleSegment2> &finalInsets)
{
	// magic numbers
	Scalar elongation = insetDist * 100; // continuityTolerance * 5;


    assert(originalSegments.size() >0);
    // assert(originalSegments.size() > 0);
	assert(finalInsets.size() == 0);
	// check that we're not playing with ourselves
	assert(&originalSegments != &finalInsets);


	unsigned int segmentCount = originalSegments.size();
	if(segmentCount < 2)
	{
		stringstream ss;
		ss <<  "1 line segment is not enough to create a closed polygon";
		ShrinkyMess mixup(ss.str().c_str());
		throw mixup;
	}


	std::vector<TriangleSegment2> relevantSegments;
	std::vector<bool> convexVertices;
	std::vector<TriangleSegment2> insets;
	std::vector<TriangleSegment2> trims;
	std::vector<Vector2> bisectors;

	bool dumpSteps = false;


	// std::cout << std::endl << "*** Shrinky::inset " << std::endl;
	if(dumpSteps)segmentsDiagnostic("originalSegments", originalSegments);
	// std::cout << std::endl << "*** RemoveShortSegments" << std::endl;
	//shorts = originalSegments;
	Scalar insetStepDistance;
	try
	{
		createBisectors(originalSegments, continuityTolerance, bisectors);
		insetStepDistance =  removeFirstCollapsedSegment(originalSegments, bisectors, insetDist, relevantSegments);
		// assert(relevantSegments.size() > 0);
		if( relevantSegments.size() > 0)
		{
			if(dumpSteps) segmentsDiagnostic("relevantSegments",relevantSegments);

			insetSegments(relevantSegments, insetStepDistance, insets);
			if(dumpSteps) segmentsDiagnostic("Insets", insets);

			elongateAndTrimSegments(insets, elongation, finalInsets);

			// currentSegments = finalInsets ;
			if(dumpSteps) segmentsDiagnostic("Finals", finalInsets);
			//assert(currentSegments.size() > 0);
			// cout << "FINALS" << endl;
		}
	}
	catch(...)
	{
		writeScadInset(originalSegments, bisectors, relevantSegments, insets, finalInsets);
		throw;
	}

	bool writeThisStep = writePartialStep;
	if(!writePartialStep)
	{
		writeThisStep = sameSame(insetDist, insetStepDistance, continuityTolerance);
	}

	if(writeThisStep)
	{
	    writeScadInset(originalSegments, bisectors, relevantSegments, insets, finalInsets);
	}
    return insetStepDistance;
}




void Shrinky::closeScadFile()
{
    if(scadFileName){
        std::ofstream & out = fscad.getOut();
        unsigned int shells = counter;
        fscad.writeMinMax("draw_outlines", "outlines_", shells);
        fscad.writeMinMax("draw_motorcycles", "motorcycles_", shells);
        fscad.writeMinMax("draw_raw_insets", "raw_insets_", shells);
        //fscad.writeMinMax("draw_trimmed_insets",  "trimmed_insets_", shells);
        fscad.writeMinMax("draw_relevants", "relevants_", shells);
        fscad.writeMinMax("draw_final_insets", "final_insets_", shells);
        out << "min=0;" << endl;
        out << "max=" << shells - 1 << ";" << std::endl;
        out << std::endl;
        out << "draw_outlines(min, max);" << std::endl;
        out << "draw_motorcycles(min, max);" << std::endl;
        out << "draw_relevants(min, max);" << std::endl;
        out << "draw_raw_insets(min, max);" << std::endl;
        //out << "draw_trimmed_insets(min, max);" <<std::endl;
        out << "draw_final_insets(min, max);" << std::endl;
        out << "// s = [\"segs.push_back(TriangleSegment2(Vector2(%s+x, %s+y), Vector2(%s+x, %s+y)));\" %(x[0][0], x[0][1], x[1][0], x[1][1]) for x in segments]" << std::endl;
        fscad.close();
    }
}

Shrinky::~Shrinky()
{
    closeScadFile();
}
