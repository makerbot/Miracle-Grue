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
