#include "Vector2.h"
#include "LineSegment2.h"

#include "Exception.h"

using namespace mgl;
using namespace std;
#define EZLOGGER_OUTPUT_FILENAME "ezlogger.txt"
#include "ezlogger_headers.hpp"

LineSegment2::LineSegment2(){}

LineSegment2::LineSegment2(const LineSegment2& other)
	:a(other.a), b(other.b){}

LineSegment2::LineSegment2(const Vector2 &a, const Vector2 &b)
	:a(a), b(b){}

LineSegment2 & LineSegment2::operator= (const LineSegment2 & other)
{
	if (this != &other)
	{
		a = other.a;
		b = other.b;

	}
	return *this;
}

Scalar LineSegment2::squaredLength() const
{
	Vector2 l = b-a;
	return l.squaredMagnitude();
}

Scalar LineSegment2::length() const
{
	Scalar l = squaredLength();
	l = sqrt(l);
	return l;
}


/**
 * @returns a new LineSegment2, elongated to be normalized to a unit vector
 */
LineSegment2 LineSegment2::elongate(const Scalar& dist)  const
{
	LineSegment2 segment(*this);
	Vector2 l = segment.b - segment.a;
	l.normalise();
	l *= dist;
	segment.b += l;
	return segment;
}

/**
 * @returns a new line segment. Of what, I don't know. Wasn't documented.
 */
LineSegment2 LineSegment2::prelongate(const Scalar& dist)  const
{
	LineSegment2 segment(*this);
	Vector2 l = segment.a - segment.b;
	l.normalise();
	l *= dist;
	segment.a += l;
	return segment;
}


//void dumpSegments(const char* prefix, const std::vector<LineSegment2> &segments)
//{
//	EZLOGGERVLSTREAM(axter::log_often) << prefix << "segments = [ // " << segments.size() << " segments" << endl;
//    for(size_t id = 0; id < segments.size(); id++)
//    {
//    	LineSegment2 seg = segments[id];
//    	cout << prefix << " [[" << seg.a << ", " << seg.b << "]], // " << id << endl;
//    }
//    EZLOGGERVLSTREAM(axter::log_often) << prefix << "]" << endl;
//    EZLOGGERVLSTREAM(axter::log_often) << "// color([1,0,0.4,1])loop_segments(segments,0.050000);" << endl;
//}

//void dumpInsets(const std::vector<SegmentTable> &insetsForLoops)
//{
//	for (unsigned int i=0; i < insetsForLoops.size(); i++)
//	{
//		const SegmentTable &insetTable =  insetsForLoops[i];
//		EZLOGGERVLSTREAM(axter::log_often) << "Loop " << i << ") " << insetTable.size() << " insets"<<endl;
//
//		for (unsigned int i=0; i <insetTable.size(); i++)
//		{
//			const std::vector<LineSegment2 >  &loop = insetTable[i];
//			EZLOGGERVLSTREAM(axter::log_often) << "   inset " << i << ") " << loop.size() << " segments" << endl;
//			dumpSegments("        ",loop);
//		}
//	}
//}
