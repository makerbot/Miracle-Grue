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

