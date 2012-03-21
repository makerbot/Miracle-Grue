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
