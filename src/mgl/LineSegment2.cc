#include "Vector2.h"
#include "LineSegment2.h"
#include "Exception.h"


using namespace std;

namespace mgl {

#ifndef SIGN
#define SIGN(x) ((x)>=0 ?  1  :  -1)
#endif

//#include "log.h"

LineSegment2::LineSegment2() {}

LineSegment2::LineSegment2(const LineSegment2& other)
		: a(other.a), b(other.b) {}

LineSegment2::LineSegment2(const Vector2 &a, const Vector2 &b)
		: a(a), b(b) {}

LineSegment2 & LineSegment2::operator=(const LineSegment2 & other) {
	if (this != &other) {
		a = other.a;
		b = other.b;

	}
	return *this;
}

Scalar LineSegment2::squaredLength() const {
	Vector2 l = b - a;
	return l.squaredMagnitude();
}

Scalar LineSegment2::length() const {
	Scalar l = squaredLength();
	l = sqrt(l);
	return l;
}

/**
 * @returns a new LineSegment2, elongated to be normalized to a unit vector
 */
LineSegment2 LineSegment2::elongate(const Scalar& dist) const {
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
LineSegment2 LineSegment2::prelongate(const Scalar& dist) const {
	LineSegment2 segment(*this);
	Vector2 l = segment.a - segment.b;
	l.normalise();
	l *= dist;
	segment.a += l;
	return segment;
}

bool LineSegment2::intersects(const LineSegment2& rhs) const {
	return mgl::intersects(*this, rhs);
}

std::ostream& operator <<(std::ostream &os,
		const LineSegment2& /*s*/) {
	//os << "[ " << s.a << ", " << s.b << "]";
	return os;
}

bool intersects(const LineSegment2& lhs, 
		const LineSegment2& rhs) {
	Vector2 lhsvec = lhs.b - lhs.a;
	Vector2 rhsvec = rhs.b - rhs.a;
	
	return SIGN((rhs.b - lhs.a).crossProduct(lhsvec)) != 
			SIGN((rhs.a - lhs.a).crossProduct(lhsvec)) && 
			SIGN((lhs.b - rhs.a).crossProduct(rhsvec)) != 
			SIGN((lhs.a - rhs.a).crossProduct(rhsvec));
}

//void dumpSegments(const char* prefix, const std::vector<LineSegment2> &segments)
//{
//	Log::often() << prefix << "segments = [ // " << segments.size() << " segments" << endl;
//    for(size_t id = 0; id < segments.size(); id++)
//    {
//    	LineSegment2 seg = segments[id];
//    	cout << prefix << " [[" << seg.a << ", " << seg.b << "]], // " << id << endl;
//    }
//    Log::often() << prefix << "]" << endl;
//    Log::often() << "// color([1,0,0.4,1])loop_segments(segments,0.050000);" << endl;
//}

//void dumpInsets(const std::vector<SegmentTable> &insetsForLoops)
//{
//	for (unsigned int i=0; i < insetsForLoops.size(); i++)
//	{
//		const SegmentTable &insetTable =  insetsForLoops[i];
//		Log::often() << "Loop " << i << ") " << insetTable.size() << " insets"<<endl;
//
//		for (unsigned int i=0; i <insetTable.size(); i++)
//		{
//			const std::vector<LineSegment2 >  &loop = insetTable[i];
//			Log::often() << "   inset " << i << ") " << loop.size() << " segments" << endl;
//			dumpSegments("        ",loop);
//		}
//	}
//}


}
