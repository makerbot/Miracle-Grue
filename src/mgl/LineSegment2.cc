#include "Vector2.h"
#include "LineSegment2.h"

#include "Exception.h"

using namespace mgl;

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
LineSegment2 LineSegment2::elongate(Scalar dist)  const
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
LineSegment2 LineSegment2::prelongate( Scalar dist)  const
{
	LineSegment2 segment(*this);
	Vector2 l = segment.a - segment.b;
	l.normalise();
	l *= dist;
	segment.a += l;
	return segment;
}


