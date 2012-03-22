


/// a line segment between 2 points.
class LineSegment2
{

public:
	Vector2 a,b; // the 2 points

public;
	LineSegment2();

	LineSegment2(const LineSegment2& other);

	LineSegment2(const Vector2 &a, const Vector2 &b);

	LineSegment2 & operator= (const LineSegment2 & other);

	Scalar squaredLength() const;
	Scalar length() const;
};
