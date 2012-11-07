#include <stdexcept>
#include "Exception.h"
#include "Triangle3.h"
#include "Scalar.h"


namespace libthing {


Triangle3::Triangle3():v0(0.0,0.0,0.0), v1(0.0,0.0,0.0), v2(0.0,0.0,0.0)
{
	// this spae intentionally left blank
}

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

	bool success = sliceTriangle( z, a, b );

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
	throw GeometryException("index out of range in Vector3& Triangle3[]");
}

Vector3 Triangle3::operator[](unsigned int i) const
{
	 if (i == 0) return v0;
	 if (i == 1) return v1;
	 if (i == 2) return v2;
	throw GeometryException("index out of range in Vector3 Triangle3[]");
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
bool Triangle3::tequals(const Triangle3 &other, const Scalar tol) const
{
	return v0.tequals(other.v0, tol) &&
			v1.tequals(other.v1, tol) &&
			v2.tequals(other.v2, tol);
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


bool Triangle3::sliceTriangle(Scalar& Z, Vector3 &a, Vector3 &b) const
{
	Scalar tol = 1e-6;

	Scalar u, px, py, v, qx, qy;
	if (v0.z > Z && v1.z > Z && v2.z > Z)
	{
		// Triangle is above Z level.
		return false;
	}
	if (v0.z < Z && v1.z < Z && v2.z < Z)
	{
		// Triangle is below Z level.
		return false;
	}
	if (libthing::tequals(v0.z, Z, tol) )
	{
		if (libthing::tequals(v1.z,Z, tol) )
		{
			if (libthing::tequals(v2.z,Z, tol) )
			{
				// flat face.  Ignore.
				return false;
			}
//			lnref = Line(Point(v0), Point(v1));
			a.x = v0.x;
			a.y = v0.y;
			a.z = Z;
			b.x = v1.x;
			b.y = v1.y;
			b.z = Z;
			return true;
		}
		if (libthing::tequals(v2.z,Z, tol) )
		{
			// lnref = Line(Point(v0), Point(v2));
			a.x = v0.x;
			a.y = v0.y;
			a.z = Z;

			b.x = v2.x;
			b.y = v2.y;
			b.z = Z;
			return true;
		}
		if ((v1.z > Z && v2.z > Z) || (v1.z < Z && v2.z < Z))
		{
			// only touches v0 tip.  Ignore.
			return false;
		}
		u = (Z-v1.z)/(v2.z-v1.z);
		px =  v1.x+u*(v2.x-v1.x);
		py =  v1.y+u*(v2.y-v1.y);
		// lnref = Line(Point(v0), Point(px,py));
		a.x = v0.x;
		a.y = v0.y;
		a.z = Z;
		b.x = px;
		b.y = py;
		b.z = Z;
		return true;
	}
	else if (libthing::tequals(v1.z, Z, tol) )
	{
		if (libthing::tequals(v2.z,Z, tol) )
		{
			// lnref = Line(Point(v1), Point(v2));
			a.x = v1.x;
			a.y = v1.y;
			a.z = Z;
			b.x = v2.x;
			b.y = v2.y;
			b.z = Z;
			return true;
		}
		if ((v0.z > Z && v2.z > Z) || (v0.z < Z && v2.z < Z))
		{
			// only touches v1 tip.  Ignore.
			return false;
		}
		u = (Z-v0.z)/(v2.z-v0.z);
		px =  v0.x+u*(v2.x-v0.x);
		py =  v0.y+u*(v2.y-v0.y);
		// lnref = Line(Point(v1), Point(px,py));
		a.x = v1.x;
		a.y = v1.y;
		a.z = Z;
		b.x = px;
		b.y = py;
		b.z = Z;
		return true;
	}
	else if (libthing::tequals(v2.z, Z, tol) )
	{
		if ((v0.z > Z && v1.z > Z) || (v0.z < Z && v1.z < Z))
		{
			// only touches v2 tip.  Ignore.
			return false;
		}
		u = (Z-v0.z)/(v1.z-v0.z);
		px =  v0.x+u*(v1.x-v0.x);
		py =  v0.y+u*(v1.y-v0.y);
		// lnref = Line(Point(v2), Point(px,py));
		a.x = v2.x;
		a.y = v2.y;
		a.z = Z;
		b.x = px;
		b.y = py;
		b.z = Z;
		return true;
	}
	else if ((v0.z > Z && v1.z > Z) || (v0.z < Z && v1.z < Z))
	{
		u = (Z-v2.z)/(v0.z-v2.z);
		px =  v2.x+u*(v0.x-v2.x);
		py =  v2.y+u*(v0.y-v2.y);
		v = (Z-v2.z)/(v1.z-v2.z);
		qx =  v2.x+v*(v1.x-v2.x);
		qy =  v2.y+v*(v1.y-v2.y);
		// lnref = Line(Point(px,py), Point(qx,qy));
		a.x = px;
		a.y = py;
		a.z = Z;
		b.x = qx;
		b.y = qy;
		b.z = Z;
		return true;
	}
	else if ((v0.z > Z && v2.z > Z) || (v0.z < Z && v2.z < Z))
	{
		u = (Z-v1.z)/(v0.z-v1.z);
		px =  v1.x+u*(v0.x-v1.x);
		py =  v1.y+u*(v0.y-v1.y);
		v = (Z-v1.z)/(v2.z-v1.z);
		qx =  v1.x+v*(v2.x-v1.x);
		qy =  v1.y+v*(v2.y-v1.y);
		// lnref = Line(Point(px,py), Point(qx,qy));
		a.x = px;
		a.y = py;
		a.z = Z;
		b.x = qx;
		b.y = qy;
		b.z = Z;
		return true;
	}
	else if ((v1.z > Z && v2.z > Z) || (v1.z < Z && v2.z < Z))
	{
		u = (Z-v0.z)/(v1.z-v0.z);//
		px =  v0.x+u*(v1.x-v0.x);
		py =  v0.y+u*(v1.y-v0.y);
		v = (Z-v0.z)/(v2.z-v0.z);
		qx =  v0.x+v*(v2.x-v0.x);
		qy =  v0.y+v*(v2.y-v0.y);
		// lnref = Line(Point(px,py), Point(qx,qy));
		a.x = px;
		a.y = py;
		a.z = Z;
		b.x = qx;
		b.y = qy;
		b.z = Z;
		return true;
	}
	return false;
}


VertexNormTriangle3::VertexNormTriangle3(): 
	Triangle3(), n0(), n1(), n2()
{
	/// this space intentionally left blank
}


VertexNormTriangle3::VertexNormTriangle3(
		const Vector3& v0, const Vector3& v1, const Vector3& v2,
		const Vector3& n0, const Vector3& n1, const Vector3& n2):
		Triangle3(v0,v1,v2),n0(n0), n1(n1), n2(n2)
{
	/// this space intentionally left blank
}

void VertexNormTriangle3::setVertexNormal(index_t vertex, Vector3& norm)
{
	if(vertex == 0) n0 = norm;
	else if(vertex == 1) n1 = norm;
	else if(vertex == 2) n2 = norm;
	else { 
		throw std::out_of_range("setVertexNormal out of range");
	}
}

FaceNormTriangle3::FaceNormTriangle3(
		const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector3& f0):
		Triangle3(v0,v1,v2),f0(f0)
{
	/// this space intentionally left blank
}

Vector3  FaceNormTriangle3::normal() const
{
	return f0;
}

}
