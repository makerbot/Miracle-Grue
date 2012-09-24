#ifndef TRIANGLE_3
#define TRIANGLE_3 (1)

#include "Vector2.h"
#include "Vector3.h"

namespace mgl {


// A 3D triangle, mostly made of 3 points
class Triangle3
{
	// Vertex points of the triangle
	Vector3 v0, v1, v2;

	// calculated at load time
	Vector2 offsetDir; // direction to offset
	Vector3 cutDir;    // cutting direction

public:
	Triangle3();
	Triangle3(const Vector3& v0, const Vector3& v1, const Vector3& v2);

	// slices this triangle into a segment that
	// flows from a to b (using the STL convention).
	bool cut(Scalar z, Vector3 &a, Vector3 &b) const;

	// accessor for the points
	Vector3& operator[](unsigned int i);

	Vector3 operator[](unsigned int i) const;

	Triangle3& operator= (const Triangle3& other);

    /// tolerance equals of this vector vs passed vector p
	bool tequals(const Triangle3 &other, const Scalar tol) const;

	// Normal vector using the right hand rule.
	// The STL convention is that it points "outside"
	// http://en.wikipedia.org/wiki/STL_(file_format)
	Vector3 normal() const;

	// Returns a vector that points in the
	// direction of the cut, using the
	// right hand normal.
	Vector3 cutDirection() const;
	//
	// Sorts the 3 points in assending order
	//
	void zSort(Vector3 &a, Vector3 &b, Vector3 &c ) const;

	bool sliceTriangle( Scalar& Z, Vector3& a, Vector3& b) const;
};


// Triangle3 with vertex normals
class  VertexNormTriangle3: public Triangle3
{
protected:
	// Vertex-normals of each vertex, v0,v1,v2
	Vector3 n0, n1, n2;

public:
	/// Constructions a Face Normal Triangle from
	// 3 point of the triangle, a vertex-normal for each specific point
	VertexNormTriangle3();
	VertexNormTriangle3(
			const Vector3& v0, const Vector3& v1, const Vector3& v2,
			const Vector3& n0, const Vector3& n1, const Vector3& n2);

	void setVertexNormal(size_t vertex, Vector3& norm);
};

// Triangle3 with vertex normals
class  FaceNormTriangle3: public Triangle3
{
protected:
	// face normal for whole triangle
	Vector3 f0;

public:
	/// Constructions a Face Normal Triangle from
	// 3 point of the triangle, with a face-normal vector
	FaceNormTriangle3(
			const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector3& f0);

	void setNormal(Vector3 faceNormal){ f0 = faceNormal; }

	Vector3 normal() const ;

};


} //end namespace mgl

#endif //TRIANGLE_3
