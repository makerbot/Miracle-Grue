
#ifndef VECTOR2_H_
#define VECTOR2_H_

#include <cmath>

#include "Scalar.h"
#include "Exception.h"

namespace mgl
{

/*
 * Standard X/Y Vector value for 2d vectors
 */
class Vector2
{
public:
	Scalar x,y;

	/// Default Constructor
	Vector2();
	Vector2(Scalar x, Scalar y);

    Scalar operator[](unsigned i) const;

    Scalar& operator[](unsigned i);

    void operator +=(const Vector2& v);

    void operator -=(const Vector2& v);

	Vector2 operator+(const Vector2& v) const;

	Vector2 operator-(const Vector2& v) const;

    void operator*=(const Scalar value);

    Vector2 operator*(const Scalar value) const;

	bool operator==(const Vector2& v) const;

    // the eucledian length
    Scalar magnitude() const;

    /**
     * Gets the squared length of this vector.
     */
    Scalar squaredMagnitude() const;

    /**
     * Normalizes this Vector2
     * makes you normal. Normal is the perfect size (1=unit)
     */
    void normalise();

    /**
     * @returns a new Vector2 which is the unit-vector of this vector
     */
    Vector2 unit() const;
    /**
     * @returns the dotProduct of this Vector2
     */
    Scalar dotProduct(const Vector2 &vector) const;
	Scalar crossProduct(const Vector2 &vector) const;
    //@returns an angle from 2 passed vectors
    // as 2 rays based at 0,0 in radians
    Scalar angleFromVector2s(const Vector2 &a, const Vector2 &b) const;
    // @ returns angle between Vector i-j and Vector j-k in radians
    //
    Scalar angleFromPoint2s(const Vector2 &i, const Vector2 &j, const Vector2 &k) const;
    /// rotates a vector by ??? returns a new vector rotated
    // around 0,0
    //@ returns a new vector rotated around point 0,0
    Vector2 rotate2d(Scalar angle) const;
};

} /* close namespace mgl */

//// A line segment that also contains cut direction
//// and inset direction.
//class TriangleSegment2 :LineSegment
//{
//	Vector2 cutDirection;
//	Vector2 insetDirection;
//}

#endif /*VECTOR2_H_*/
