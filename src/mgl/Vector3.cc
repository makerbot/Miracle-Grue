#include "Vector3.h"
#include "libthing/Scalar.h"

namespace mgl {

using namespace std;

Vector3::Vector3():x(0),y(0),z(0){}

Vector3::Vector3(Scalar x, Scalar y, Scalar z)
			: x(x), y(y), z(z)
{}

Scalar Vector3::operator[](unsigned i) const
{
	if (i == 0) return x;
	if (i == 1) return y;
	if (i == 2) return z;
	throw GeometryException("index out of range in Scalar Vector3[]");
}

Scalar& Vector3::operator[](unsigned i)
{
	if (i == 0) return x;
	if (i == 1) return y;
	if (i == 2) return z;
	throw GeometryException("index out of range in Scalar& Vector3[]");
}

void Vector3::operator*=(const Scalar value)
{
	x *= value;
	y *= value;
	z *= value;
}

Vector3 Vector3::operator*(const Scalar value) const
{
	return Vector3(x*value, y*value, z*value);
}

// Adds the given vector to this.
void Vector3::operator+=(const Vector3& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
}


// Returns the value of the given vector added to this.
Vector3 Vector3::operator+(const Vector3& v) const
{
	return Vector3(x+v.x, y+v.y, z+v.z);
}


void Vector3::operator-=(const Vector3& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
}


Vector3 Vector3::operator-(const Vector3& v) const
{
	return Vector3(x-v.x, y-v.y, z-v.z);
}


Vector3 Vector3::crossProduct(const Vector3 &vector) const
{
	return Vector3(y*vector.z-z*vector.y,
				   z*vector.x-x*vector.z,
				   x*vector.y-y*vector.x);
}

// performs a cross product,
// stores the result in the object
void Vector3::crossProductUpdate(const Vector3 &vector)
{
	 *this = crossProduct(vector);
}

Scalar Vector3::dotProduct(const Vector3 &vector) const
{
	return x*vector.x + y*vector.y + z*vector.z;
}

// the eucledian length
Scalar Vector3::magnitude() const
{
	return sqrt(squaredMagnitude());
}

// Gets the squared length of this vector.
Scalar Vector3::squaredMagnitude() const
{
	return x*x+y*y+z*z;
}

// makes you normal. Normal is the perfect size (1)
void Vector3::normalise()
{
	Scalar l = magnitude();
	if (l > 0)
	{
		(*this) *= ((Scalar)1)/l;
	}
}

Vector3 Vector3::getNormal() const
{
	Scalar l = magnitude();
	if (l != 0)
		return Vector3(x * l, y*l, z*l);
	return Vector3(0.0,0.0,0.0);
}


Vector3 Vector3::unit() const
{
	Vector3 result = *this;
	result.normalise();
	return result;
}

}
