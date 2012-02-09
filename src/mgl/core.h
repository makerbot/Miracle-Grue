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
// #include <stdio.h>
// #define STRONG_CHECKING

#define dbg__ std::cout <<  __FUNCTION__ << "::" << __LINE__  << "*" << std::endl;
#define dbgs__(s) std::cout <<  __FUNCTION__ << "::" << __LINE__  << " > "<< s << std::endl;


namespace mgl
{

// base class for exceptions
class Messup
{
public:
	std::string error;
	Messup(const char *msg)
	 :error(msg)
	{

		std::cerr << std::endl << msg << std::endl;
		// fprintf(stderr, "%s", msg);
	}
};

//
// Our basic numerical type. double for now;
//
typedef double Scalar;
#define SCALAR_SQRT(s) sqrt(s)
#define SCALAR_ABS(s) abs(s)
// true if two Scalar values are approximally the same,
// using a hard coded tolerance
bool sameSame(Scalar a, Scalar b, Scalar tol); // = 1e-8


typedef unsigned int index_t;
typedef std::vector<index_t> TriangleIndices;
typedef std::vector<TriangleIndices> SliceTable;

class Vector2
{
public:
	Scalar x;
	Scalar y;

	Vector2(){}

	Vector2(Scalar x, Scalar y)
		:x(x), y(y)
	{}

    Scalar operator[](unsigned i) const
    {
        if (i == 0) return x;
        return y;
    }

    Scalar& operator[](unsigned i)
    {
        if (i == 0) return x;
        return y;
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

	bool sameSame(const Vector2 &p, Scalar tol) const
	{
		Scalar dx = p.x - x;
		Scalar dy = p.y -y;
		return mgl::sameSame(0, dx*dx + dy*dy, tol);
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
};

std::ostream& operator <<(std::ostream &os,const Vector2 &pt);

Scalar angleFromVector2s(const Vector2 &a, const Vector2 &b);
Scalar angleFromPoint2s(const Vector2 &i, const Vector2 &j, const Vector2 &k);


// a line segment between 2 points
class TriangleSegment2
{
public:
	Vector2 a,b; // the 2 points

	TriangleSegment2(){}

	TriangleSegment2(const TriangleSegment2& other)
	:a(other.a), b(other.b){}

	TriangleSegment2(const Vector2 &a, const Vector2 &b)
	:a(a), b(b){}

	Scalar squaredLength() const
	{
		Vector2 l = b-a;
		return l.squaredMagnitude();
	}

	Scalar length()
	{
		Scalar l = squaredLength();
		l = sqrt(l);
		return l;
	}
};

typedef std::vector< std::vector<TriangleSegment2 > > SegmentTable;


Vector2 rotate2d(const Vector2 &p, Scalar angle);


// 3d vector class... warning: may be used for points
class Vector3
{
public:
	Scalar x,y,z;

	Vector3(){}

	Vector3(Scalar x, Scalar y, Scalar z)
	            : x(x), y(y), z(z)
	{}

    Scalar operator[](unsigned i) const
    {
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }

    Scalar& operator[](unsigned i)
    {
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
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
		 return v2;
	}

	Vector3 operator[](unsigned int i) const
	{
		 if (i == 0) return v0;
		 if (i == 1) return v1;
		 return v2;
	}

	//
	// Normal vector using the right hand rule.
	// The STL convection is that it points "outside"
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

class LayerMess : public Messup {public: LayerMess(const char *msg)	 :Messup(msg) {	} };
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
	{
	}

	unsigned int zToLayerAbove(Scalar z) const
	{
		if(z < 0)
		{
			LayerMess mixup("Model with points below the z axis are not supported in this version. Please center your model on the build area");
			throw mixup;
		}

		if (z < firstLayerZ)
			return 0;

		Scalar tol = 0.00000000000001; // tolerance
		Scalar layer = (z+tol-firstLayerZ) / layerH;
		return ceil(layer);
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
// The Slice is a series of tubes
//
// tubes are plastic extrusions
class TubesInSlice
{
public:
	TubesInSlice(Scalar z)
		:z(z)
	{
	}

	Scalar z;
	std::vector<TriangleSegment2> infill;
	std::vector< std::vector<TriangleSegment2> > outlines;
};

typedef std::vector<Vector2> Polygon;
typedef std::vector<Polygon> Polygons;



class ExtruderSlice
{
public:

//	Polygons paths; // everybody!

	Polygons loops;  // outer perimeter loop
	Polygons infills;

	std::vector<Polygons> insets;
//
//	std::vector<std::vector<mgl::Polygons> > InnerShells; // multiple inner loops per loop
//	std::vector<mgl::Polygons> roofing;
//	std::vector<mgl::Polygons> flooring;



};

//
// The Slice data is contains polygons
// for each extruder, for a slice.
// there are multiple polygons for each extruder
class SliceData
{
public:
	// TubesInSlice tubes;

	std::vector<ExtruderSlice > extruderSlices;

	double z;
	index_t sliceIndex;

	SliceData (double z, index_t sliceIndex)
		:z(z), sliceIndex(sliceIndex)//, tubes(z)
	{

	}

};

}

#endif
