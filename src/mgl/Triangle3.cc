#include "Triangle3.h"
#include "segment.h"

using namespace mgl;
using namespace std;

Triangle3::Triangle3(const Vector3& v0, const Vector3& v1, const Vector3& v2)
:v0(v0),v1(v1),v2(v2)
{

	Vector3 n = normal();
	Vector3 up(0,0,1);
	Vector3 c;
	cutDir = n.crossProduct(up);

	offsetDir.x = -n.x;
	offsetDir.y = -n.y;
	if(offsetDir.squaredMagnitude() >0 )
		offsetDir.normalise();
}

// slices this triangle into a segment that
// flows from a to b (using the STL convention).
bool Triangle3::cut(Scalar z, Vector3 &a, Vector3 &b) const
{
	Vector3 dir = cutDirection();

	bool success = sliceTriangle(v0,v1,v2, z, a, b );

	Vector3 segmentDir = b - a;
	if(dir.dotProduct(segmentDir) < 0 )
	{
		Vector3 p(a);
		a = b;
		b = p;
	}
	return success;
}

// accessor for the points
Vector3& Triangle3::operator[](unsigned int i)
{
	 if (i == 0) return v0;
	 if (i == 1) return v1;
	 if (i == 2) return v2;
	throw Exception("index out of range in Vector3& Triangle3[]");
}

Vector3 Triangle3::operator[](unsigned int i) const
{
	 if (i == 0) return v0;
	 if (i == 1) return v1;
	 if (i == 2) return v2;
	throw Exception("index out of range in Vector3 Triangle3[]");
}


Triangle3& Triangle3::operator= (const Triangle3& other)
{
	if (this != &other)
	{
		v0 = other.v0;
		v1 = other.v1;
		v2 = other.v2;
		offsetDir = other.offsetDir;
		cutDir = other.cutDir;

	}
	return *this;
}

/// tolerance equals of this vector vs pased vector p
bool Triangle3::tequals(const Triangle3 &other, Scalar tol) const
{
	return v0.tequals(other.v0,tol) &&
			v1.tequals(other.v1,tol) &&
			v2.tequals(other.v2,tol);
}


// Normal vector using the right hand rule.
// The STL convention is that it points "outside"
// http://en.wikipedia.org/wiki/STL_(file_format)
Vector3 Triangle3::normal() const
{
	Vector3 a = v1 - v0;
	Vector3 b = v2 - v0;

	Vector3 n = a.crossProduct(b);
	n.normalise();
	return n;
}


// Returns a vector that points in the
// direction of the cut, using the
// right hand normal.
Vector3 Triangle3::cutDirection() const
{
	return cutDir;
}

//
// Sorts the 3 points in assending order
//
void Triangle3::zSort(Vector3 &a, Vector3 &b, Vector3 &c ) const
{
	if (v0[2] < v1[2]) {
		if (v1[2] < v2[2]) {
			//v0<v1<v2
			a = v0;	b = v1; c = v2;
		} else {
			if (v0[2] < v2[2]) {
				//v0<v2<v1
				a = v0; b = v2; c = v1;
			} else {
				//v2<v0<v1
				a = v2; b = v0; c = v1;
			}
		}
	} else {
		if (v0[2] < v2[2]) {
			//v1<v0<v2
			a = v1; b = v0; c = v2;

		} else {
			if (v2[2] < v1[2]) {
				//v2<v1<v0
				a = v2; b = v1; c = v0;
			} else {
				//v1<v2<v0
				a = v1; b = v2; c = v0;
			}
		}
	}
}

