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


// #define STRONG_CHECKING

namespace mgl
{

//
// Our basic numerical type. double for now;
//
typedef double Scalar;

// true if two Scalar values are approximally the same,
// using a hard coded tolerance
bool sameSame(Scalar a, Scalar b);

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

    void operator+=(const Vector2& v)
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


	bool sameSame(const Vector2 &p) const
	{
		Scalar dx = p.x - x;
		Scalar dy = p.y -y;
		return mgl::sameSame(0, dx*dx + dy*dy);
	}
};

std::ostream& operator <<(std::ostream &os,const Vector2 &pt);


// a line segment between 2 points
struct Segment
{
	Vector2 a,b;
};

// base class for exceptions
class Except
{
public:
	std::string error;
	Except(const char *msg)
	 :error(msg)
	{

	}
};




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


public:

	Triangle3(const Vector3& v0, const Vector3& v1, const Vector3& v2)
	:v0(v0),v1(v1),v2(v2)
	{

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
		Vector3 n = normal();
		Vector3 up(0,0,1);
		Vector3 d = n.crossProduct(up);
		return d;
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



typedef std::vector<Vector2> Polygon;
typedef std::vector<Polygon> Polygons;


class ExtruderSlice
{
public:

//	Polygons paths; // everybody!

	Polygons loops;  // outer perimeter loop
	Polygons infills;
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

	std::vector<ExtruderSlice > extruderSlices;

	double positionZ;
	index_t sliceIndex;

	SliceData (double z, index_t sliceIndex)
		:positionZ(z), sliceIndex(sliceIndex)
	{

	}


};

}

#endif
