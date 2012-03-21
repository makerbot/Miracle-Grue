/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/


#ifndef CORE_H_
#define CORE_H_

#include <cmath>
#include <vector>
#include <iostream>
#include <cassert>

// WIN32 compatibility stuff
#ifdef WIN32

#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#define M_PI 3.14159265358979323846

#endif // WIN32

// #define STRONG_CHECKING

#define dbg__ std::cout <<  __FUNCTION__ << "::" << __LINE__  << "*" << std::endl;
#define dbgs__(s) std::cout <<  __FUNCTION__ << "::" << __LINE__  << " > "<< s << std::endl;


namespace mgl
{

/**
 * base class for all MGL Exceptions
 */
class Exception
{

public:
	std::string error;
	Exception(const char *msg) :error(msg)
	{
		//	std::cerr << std::endl << msg << std::endl;
		// fprintf(stderr, "%s", msg);
	}

};

//////////
// Scalar: Our basic numerical type. double for now;
///////////
typedef double Scalar;
#define SCALAR_SQRT(s) sqrt(s)
#define SCALAR_ABS(s) abs(s)
#define SCALAR_ACOS(s) acos(s)
#define SCALAR_SIN(s) sin(s)
#define SCALAR_COS(s) cos(s)

// See float.h for details on these
#define SCALAR_EPSILON DBL_EPSILON

/** (t)olerance (equals)
 * @returns true if two Scalar values are approximally the same using tolernce
 */
bool tequals(Scalar a, Scalar b, Scalar tol); // = 1e-8

//////////
// Scalar: End numeric type info
///////////

// Type used for indexes of triangles/etc for unique indexing
typedef unsigned int index_t;

/// Structure contains list of triangle 'id's, used to
/// reference which triangle in the master list is related.
typedef std::vector<index_t> TriangleIndices;

/// A list of all slices, where each slice
/// is just a list of triangles id's that are related to
/// that specified slice.
typedef std::vector<TriangleIndices> SliceTable;

/// Standard X/Y Vector value
class Vector2
{
public:
	Scalar x;
	Scalar y;

	Vector2():x(0),y(0){}

	Vector2(Scalar x, Scalar y)
		:x(x), y(y)
	{}

    Scalar operator[](unsigned i) const
    {
        if (i == 0) return x;
        if (i == 1) return y;
        throw Exception("index out of range in Scalar Vector2[]");
    }

    Scalar& operator[](unsigned i)
    {
        if (i == 0) return x;
        if (i == 1) return y;
        throw Exception("index out of range in Scalar& Vector2[]");
    }

    void operator +=(const Vector2& v)
    {
        x += v.x;
        y += v.y;
    }

    void operator -=(const Vector2& v)
    {
        x -= v.x;
        y -= v.y;
    }

	Vector2 operator+(const Vector2& v) const
	{
		return Vector2(x+v.x, y+v.y);
	}

	Vector2 operator-(const Vector2& v) const
	{
		return Vector2(x-v.x, y-v.y);
	}

    void operator*=(const Scalar value)
    {
        x *= value;
        y *= value;
    }

    Vector2 operator*(const Scalar value) const
    {
        return Vector2(x*value, y*value);
    }

    /// tolerance equals of this vector vs pased vector p
	bool tequals(const Vector2 &p, const Scalar tol) const
	{
		Scalar dx = p.x - x;
		Scalar dy = p.y -y;
		return mgl::tequals(0, dx*dx + dy*dy, tol);
	}

    // the eucledian length
    Scalar magnitude() const
    {
        return sqrt(squaredMagnitude());
    }

    // Gets the squared length of this vector.
    Scalar squaredMagnitude() const
    {
        return x*x+y*y;
    }

    // makes you normal. Normal is the perfect size (1)
    void normalise()
    {
        Scalar l = magnitude();
        if (l > 0)
        {
            (*this) *= ((Scalar)1)/l;
        }
        else
        {
        	assert(0);
        }
    }

    Vector2 unit() const
    {
        Vector2 result = *this;
        result.normalise();
        return result;
    }

    Scalar dotProduct(const Vector2 &vector) const
    {
    	return x*vector.x + y*vector.y;
    }

    //@returns an angle from 2 passed vectors
    // as 2 rays based at 0,0 in radians
    Scalar angleFromVector2s(const Vector2 &a, const Vector2 &b) const
    {
       	Scalar dot = a.dotProduct(b);
		Scalar cosTheta = dot / (a.magnitude() * b.magnitude());
		if (cosTheta >  1.0) cosTheta  = 1;
		if (cosTheta < -1.0) cosTheta = -1;
		Scalar theta = M_PI - SCALAR_ACOS(cosTheta);
		return theta;
    }

    // @ returns angle between Vector i-j and Vector j-k in radians
    //
    Scalar angleFromPoint2s(const Vector2 &i, const Vector2 &j, const Vector2 &k) const
    {
    	Vector2 a = i - j;
    	Vector2 b = j - k;
    	Scalar theta = angleFromVector2s(a,b);
    	return theta;
    }

    /// rotates a vector by ??? returns a new vector rotated
    // around 0,0
    //@ returns a new vector rotated around point 0,0
    Vector2 rotate2d(Scalar angle) const
	{
		// rotate point
		Scalar s = SCALAR_SIN(angle); // radians
		Scalar c = SCALAR_COS(angle);
		Vector2 rotated;
		rotated.x = x * c - y * s;
		rotated.y = x * s + y * c;
		return rotated;
	}
};





std::ostream& operator <<(std::ostream &os,const Vector2 &pt);


//// A line segment that also contains cut direction
//// and inset direction.
//class TriangleSegment2 :LineSegment
//{
//	Vector2 cutDirection;
//	Vector2 insetDirection;
//}


/// a line segment between 2 points.
class LineSegment2
{
public:
	Vector2 a,b; // the 2 points


	LineSegment2(){}

	LineSegment2(const LineSegment2& other)
	:a(other.a), b(other.b){}

	LineSegment2(const Vector2 &a, const Vector2 &b)
	:a(a), b(b){}

	LineSegment2 & operator= (const LineSegment2 & other)
	{
		if (this != &other)
		{
			a = other.a;
			b = other.b;

		}
		return *this;
	}

	Scalar squaredLength() const
	{
		Vector2 l = b-a;
		return l.squaredMagnitude();
	}

	Scalar length() const
	{
		Scalar l = squaredLength();
		l = sqrt(l);
		return l;
	}
};

/// List of Lists of line segments. Used to lookup
/// A SegmentTable may contain, for example, a perimeter
/// and hole(s) in that perimeter of a slice.
typedef std::vector< std::vector<LineSegment2 > > SegmentTable;

std::ostream& operator << (std::ostream &os, const LineSegment2 &s);


/// your basic XYZ vector table
/// 3d vector class... warning: may be used for points
class Vector3
{
public:
	Scalar x,y,z;

	Vector3():x(0),y(0),z(0){}

	Vector3(Scalar x, Scalar y, Scalar z)
	            : x(x), y(y), z(z)
	{}

    Scalar operator[](unsigned i) const
    {
        if (i == 0) return x;
        if (i == 1) return y;
        if (i == 2) return z;
        throw Exception("index out of range in Scalar Vector3[]");
    }

    Scalar& operator[](unsigned i)
    {
        if (i == 0) return x;
        if (i == 1) return y;
        if (i == 2) return z;
        throw Exception("index out of range in Scalar& Vector3[]");
    }

    void operator*=(const Scalar value)
    {
        x *= value;
        y *= value;
        z *= value;
    }

    Vector3 operator*(const Scalar value) const
    {
        return Vector3(x*value, y*value, z*value);
    }

    // Adds the given vector to this.
    void operator+=(const Vector3& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
    }


    // Returns the value of the given vector added to this.
    Vector3 operator+(const Vector3& v) const
    {
        return Vector3(x+v.x, y+v.y, z+v.z);
    }


    void operator-=(const Vector3& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
    }


    Vector3 operator-(const Vector3& v) const
    {
        return Vector3(x-v.x, y-v.y, z-v.z);
    }


    // Vector3 other matches this vector within tolerance tol
    bool tequals(const Vector3 &other, const Scalar tol) const
    {
    	return mgl::tequals(x, other.x,tol) &&
    			mgl::tequals(y, other.y, tol) &&
    			mgl::tequals(z, other.z, tol);
    }

    Vector3 crossProduct(const Vector3 &vector) const
    {
        return Vector3(y*vector.z-z*vector.y,
                       z*vector.x-x*vector.z,
                       x*vector.y-y*vector.x);
    }

    // performs a cross product,
    // stores the result in the object
    void crossProductUpdate(const Vector3 &vector)
    {
    	 *this = crossProduct(vector);
    }

    Scalar dotProduct(const Vector3 &vector) const
    {
    	return x*vector.x + y*vector.y + z*vector.z;
    }

    // the eucledian length
    Scalar magnitude() const
    {
        return sqrt(squaredMagnitude());
    }

    // Gets the squared length of this vector.
    Scalar squaredMagnitude() const
    {
        return x*x+y*y+z*z;
    }

    // makes you normal. Normal is the perfect size (1)
    void normalise()
    {
        Scalar l = magnitude();
        if (l > 0)
        {
            (*this) *= ((Scalar)1)/l;
        }
    }


    Vector3 unit() const
    {
        Vector3 result = *this;
        result.normalise();
        return result;
    }

};



std::ostream& operator<<(std::ostream& os, const Vector3& v);


// A 3D triangle, mostly made of 3 points
class Triangle3
{
	Vector3 v0, v1, v2;

	// calculated at load time
	Vector2 offsetDir; // direction to offset
	Vector3 cutDir;    // cutting direction

public:

	Triangle3(const Vector3& v0, const Vector3& v1, const Vector3& v2)
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
	bool cut(Scalar z, Vector3 &a, Vector3 &b) const;

	// accessor for the points
	Vector3& operator[](unsigned int i)
	{
		 if (i == 0) return v0;
		 if (i == 1) return v1;
		 if (i == 2) return v2;
        throw Exception("index out of range in Vector3& Triangle3[]");
	}

	Vector3 operator[](unsigned int i) const
	{
		 if (i == 0) return v0;
		 if (i == 1) return v1;
		 if (i == 2) return v2;
        throw Exception("index out of range in Vector3 Triangle3[]");
	}


	Triangle3& operator= (const Triangle3& other)
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
	bool tequals(const Triangle3 &other, Scalar tol) const
	{
		return v0.tequals(other.v0,tol) &&
				v1.tequals(other.v1,tol) &&
				v2.tequals(other.v2,tol);
	}


	// Normal vector using the right hand rule.
	// The STL convention is that it points "outside"
	// http://en.wikipedia.org/wiki/STL_(file_format)
	Vector3 normal() const
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
	Vector3 cutDirection() const
	{
		return cutDir;
	}

	//
	// Sorts the 3 points in assending order
	//
	void zSort(Vector3 &a, Vector3 &b, Vector3 &c ) const
	{

		if (v0[2] < v1[2]) {
			if (v1[2] < v2[2]) {
				//v0<v1<v2
				a = v0;
				b = v1;
				c = v2;
			} else {
				if (v0[2] < v2[2]) {
					//v0<v2<v1
					a = v0;
					b = v2;
					c = v1;
				} else {
					//v2<v0<v1
					a = v2;
					b = v0;
					c = v1;
				}
			}
		} else {
			if (v0[2] < v2[2]) {
				//v1<v0<v2
				a = v1;
				b = v0;
				c = v2;

			} else {
				if (v2[2] < v1[2]) {
					//v2<v1<v0
					a = v2;
					b = v1;
					c = v0;
				} else {
					//v1<v2<v0
					a = v1;
					b = v2;
					c = v0;
				}
			}
		}

	}

};

class LayerException : public Exception {
	public: LayerException(const char *msg)	 :Exception(msg) {	};
};

// A tape measure for layers, since layers have variable height.
// Class that relates height (a scalar) to layer index (unsigned int)
//
// This class assumes that the model's triangles are
// all above 0 (the z of each of the 3 vertices is >= 0.0).
// worse, the layers MUST start at 0. Lazy programmer!
// This is good enough for now, until the class "sees" every triangle
// during loading and recalcs layers on the fly.
//
class LayerMeasure
{
	Scalar firstLayerZ;
	Scalar layerH;

public:
	LayerMeasure(Scalar firstLayerZ, Scalar layerH)
		:firstLayerZ(firstLayerZ), layerH(layerH)
	{}

	unsigned int zToLayerAbove(Scalar z) const
	{
		if(z < 0)
		{
			LayerException mixup("Model with points below the z axis are not supported in this version. Please center your model on the build area");
			throw mixup;
		}

		if (z < firstLayerZ)
			return 0;

		Scalar tol = 0.00000000000001; // tolerance
		Scalar layer = (z+tol-firstLayerZ) / layerH;
		return (unsigned int)ceil(layer);
	}

	Scalar sliceIndexToHeight(unsigned int sliceIndex) const
	{
		return firstLayerZ + sliceIndex * layerH;
	}

	Scalar getLayerH() const
	{
		return layerH;
	}
};


//
////
//// The Slice is a series of tubes
////
//// tubes are plastic extrusions
//class TubesInSlice
//{
//public:
//	TubesInSlice(Scalar z)
//		:z(z)
//	{
//	}
//
//	Scalar z;
//	std::vector<LineSegment2d> infill;
//	std::vector< std::vector<LineSegment2d> > outlines;
//};

/// A polygon is an arbitarty collection of 2d points
typedef std::vector<Vector2> Polygon;

/// A vector of polygon objects
typedef std::vector<Polygon> Polygons;

std::ostream& operator<<(std::ostream& os, const Polygon& v);


} // namespace

#endif
