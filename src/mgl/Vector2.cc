

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




//// A line segment that also contains cut direction
//// and inset direction.
//class TriangleSegment2 :LineSegment
//{
//	Vector2 cutDirection;
//	Vector2 insetDirection;
//}
