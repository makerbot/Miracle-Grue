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


#include "BGL/BGLPoint.h"
#include "BGL/BGLTriangle3d.h"

// #define STRONG_CHECKING

namespace mgl
{


typedef unsigned int index_t;
typedef std::vector<index_t> TriangleIndices;
typedef std::vector<TriangleIndices> SliceTable;

// a line segment between 2 points
struct Segment
{
	BGL::Point a,b;
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


// true if two floating point values are approximally the same,
// using a hard coded tolerance
bool sameSame(double a, double b);


// 3d vector class... warning: may be used for points
class Vector3d
{
public:
	Scalar x,y,z;

	Vector3d(){}

	Vector3d(Scalar x, Scalar y, Scalar z)
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

    Vector3d operator*(const Scalar value) const
    {
        return Vector3d(x*value, y*value, z*value);
    }

    // Adds the given vector to this.
    void operator+=(const Vector3d& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
    }


    // Returns the value of the given vector added to this.
    Vector3d operator+(const Vector3d& v) const
    {
        return Vector3d(x+v.x, y+v.y, z+v.z);
    }


    void operator-=(const Vector3d& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
    }


    Vector3d operator-(const Vector3d& v) const
    {
        return Vector3d(x-v.x, y-v.y, z-v.z);
    }

    Vector3d crossProduct(const Vector3d &vector) const
    {
        return Vector3d(y*vector.z-z*vector.y,
                       z*vector.x-x*vector.z,
                       x*vector.y-y*vector.x);
    }

    // performs a cross product,
    // stores the result in the object
    void crossProductUpdate(const Vector3d &vector)
    {
    	 *this = crossProduct(vector);
    }

    Scalar dotProduct(const Vector3d &vector) const
    {
    	return x*vector.x + y*vector.y + z*vector.z;
    }

    // the eucledian length
    Scalar magnitude() const
    {
        return sqrt(squareMagnitude());
    }

    // Gets the squared length of this vector.
    Scalar squareMagnitude() const
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


    Vector3d unit() const
    {
        Vector3d result = *this;
        result.normalise();
        return result;
    }

};



std::ostream& operator<<(ostream& os, const Vector3d& v);


// A 3D triangle, mostly made of 3 points
class Triangle3
{
	Vector3d v0, v1, v2;


public:

	Triangle3(const Vector3d& v0, const Vector3d& v1, const Vector3d& v2)
	:v0(v0),v1(v1),v2(v2)
	{

	}

	Triangle3(const BGL::Triangle3d &t)
	:v0(t.vertex1.x, t.vertex1.y, t.vertex1.z),
	 v1(t.vertex2.x, t.vertex2.y, t.vertex2.z),
	 v2(t.vertex3.x, t.vertex3.y, t.vertex3.z)
	{

	}

	// slices this triangle into a segment that
	// flows from a to b (using the STL convention).
	bool cut(Scalar z, Vector3d &a, Vector3d &b) const;

	// accessor for the points
	Vector3d& operator[](unsigned int i)
	{
		 if (i == 0) return v0;
		 if (i == 1) return v1;
		 return v2;
	}

	Vector3d operator[](unsigned int i) const
	{
		 if (i == 0) return v0;
		 if (i == 1) return v1;
		 return v2;
	}

	//
	// Normal vector using the right hand rule.
	// The STL convection is that it points "outside"
	// http://en.wikipedia.org/wiki/STL_(file_format)
	Vector3d normal() const
	{
		Vector3d a = v1 - v0;
		Vector3d b = v2 - v0;

		Vector3d n = a.crossProduct(b);
		n.normalise();
		return n;
	}

	// Returns a vector that points in the
	// direction of the cut, using the
	// right hand normal.
	Vector3d cutDirection() const
	{
		Vector3d n = normal();
		Vector3d up(0,0,1);
		Vector3d d = n.crossProduct(up);
		return d;
	}

	//
	// Sorts the 3 points in assending order
	//
	void zSort(Vector3d &a, Vector3d &b, Vector3d &c ) const
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

struct Point2D
{
	Point2D(Scalar x, Scalar y)
		:x(x), y(y)
	{}
	Scalar x;
	Scalar y;

	bool sameSame(Point2D &p)const
	{
		Scalar dx = p.x - x;
		Scalar dy = p.y -y;
		return mgl::sameSame(0, dx*dx + dy*dy);
	}
};

typedef std::vector<Point2D> Polygon;
typedef std::vector<Polygon> ExtruderPaths;

}

#endif
