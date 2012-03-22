#ifndef VECTOR3_H_
#define VECTOR3_H_ (1)

#include "Scalar.h"
#include "Exception.h"

namespace mgl {
/// your basic XYZ vector table
/// 3d vector class... warning: may be used for points
class Vector3
{
public:
	Scalar x,y,z;

	Vector3();

	Vector3(Scalar x, Scalar y, Scalar z);

	Scalar operator[](unsigned i) const;
    Scalar& operator[](unsigned i);

    void operator*=(const Scalar value);
    Vector3 operator*(const Scalar value) const;

    // Adds the given vector to this.
    void operator+=(const Vector3& v);

    // Returns the value of the given vector added to this.
    Vector3 operator+(const Vector3& v) const;

    void operator-=(const Vector3& v);

    Vector3 operator-(const Vector3& v) const;

    // Vector3 other matches this vector within tolerance tol
    bool tequals(const Vector3 &other, const Scalar tol) const;

    Vector3 crossProduct(const Vector3 &vector) const;

    // performs a cross product,
    // stores the result in the object
    void crossProductUpdate(const Vector3 &vector);
    Scalar dotProduct(const Vector3 &vector) const;
    // the eucledian length
    Scalar magnitude() const ;
    // Gets the squared length of this vector.
    Scalar squaredMagnitude() const;
    // makes you normal. Normal is the perfect size (1)
    void normalise();

    Vector3 unit() const;
};
} //end namespace mgl
#endif //VECTOR3_H_
