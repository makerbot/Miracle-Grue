#include <cassert>

#include "Vector2.h"
#include "Exception.h"
#include "mgl.h"

#ifndef M_PI 
#define M_PI 3.14159265358979323846
#endif 

namespace libthing {
/*
 * Standard X/Y Vector value for 2d vectors
 */
/// Default Constructor

Vector2::Vector2() : x(0), y(0) {
}

Vector2::Vector2(Scalar x, Scalar y) : x(x), y(y) {
}

Scalar Vector2::operator[](unsigned i) const {
	if (i == 0) return x;
	if (i == 1) return y;
	throw GeometryException("index out of range in Scalar Vector2[]");
}

Scalar& Vector2::operator[](unsigned i) {
	if (i == 0) return x;
	if (i == 1) return y;
	throw GeometryException("index out of range in Scalar& Vector2[]");
}

void Vector2::operator +=(const Vector2& v) {
	x += v.x;
	y += v.y;
}

void Vector2::operator -=(const Vector2& v) {
	x -= v.x;
	y -= v.y;
}

Vector2 Vector2::operator+(const Vector2& v) const {
	return Vector2(x + v.x, y + v.y);
}

Vector2 Vector2::operator-(const Vector2& v) const {
	return Vector2(x - v.x, y - v.y);
}

void Vector2::operator*=(const Scalar value) {
	x *= value;
	y *= value;
}

Vector2 Vector2::operator*(const Scalar value) const {
	return Vector2(x*value, y * value);
}

bool Vector2::operator==(const Vector2 &v) const {
	return x == v.x && y == v.y;
}

/// tolerance equals of this vector vs pased vector p

bool Vector2::tequals(const Vector2 &p, const Scalar tol) const {
	Scalar dx = p.x - x;
	Scalar dy = p.y - y;
	return libthing::tequals(0, dx * dx + dy*dy, tol);
}

// the eucledian length

Scalar Vector2::magnitude() const {
	return sqrt(squaredMagnitude());
}

/**
 * Gets the squared length of this vector.
 */
Scalar Vector2::squaredMagnitude() const {
	return x * x + y*y;
}

/**
 * Normalizes this Vector2
 * makes you normal. Normal is the perfect size (1=unit)
 */
void Vector2::normalise() {
	Scalar l = magnitude();
	if (l > 0) {
		(*this) *= Scalar(1) / l;
	} else {
		GeometryException mixup("Attempting to normalize Vector2 of 0 length");
		throw mixup;
	}
}

/**
 * @returns a new Vector2 which is the unit-vector of this vector
 */
Vector2 Vector2::unit() const {
	Vector2 result = *this;
	result.normalise();
	return result;
}

/**
 * @returns the dotProduct of this Vector2
 */
Scalar Vector2::dotProduct(const Vector2 &vector) const {
	return x * vector.x + y * vector.y;
}

Scalar Vector2::crossProduct(const Vector2& vector) const {
	//x = yz - zy
	//y = zx - xz
	//z = xy - yx
	return x * vector.y - y * vector.x;
}

//@returns an angle from 2 passed vectors
// as 2 rays based at 0,0 in radians

Scalar Vector2::angleFromVector2s(const Vector2 &a, const Vector2 &b) const {
	Scalar dot = a.dotProduct(b);
	Scalar cosTheta = dot / (a.magnitude() * b.magnitude());
	if (cosTheta > 1.0) cosTheta = 1;
	if (cosTheta < -1.0) cosTheta = -1;
	Scalar theta = M_PI - SCALAR_ACOS(cosTheta);
	return theta;
}

// @ returns angle between Vector i-j and Vector j-k in radians
//

Scalar Vector2::angleFromPoint2s(const Vector2 &i, const Vector2 &j, const Vector2 &k) const {
	Vector2 a = i - j;
	Vector2 b = j - k;
	Scalar theta = angleFromVector2s(a, b);
	return theta;
}

/// rotates a vector by ??? returns a new vector rotated
// around 0,0
//@ returns a new vector rotated around point 0,0

Vector2 Vector2::rotate2d(Scalar angle) const {
	// rotate point
	Scalar s = SCALAR_SIN(angle); // radians
	Scalar c = SCALAR_COS(angle);
	Vector2 rotated;
	rotated.x = x * c - y * s;
	rotated.y = x * s + y * c;
	return rotated;
}



//// A line segment that also contains cut direction
//// and inset direction.
//class TriangleSegment2 :LineSegment
//{
//	Vector2 cutDirection;
//	Vector2 insetDirection;
//}

}

