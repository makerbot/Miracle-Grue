#ifndef MGL_EDGE_H
#define MGL_EDGE_H (1)

#include "mgl.h"

namespace mgl{

// 3 edges in a Face
class Edge // it's a line.
{
public:
	size_t face0;
	int face1;

	// vertices is plural for vertex (it's a point, really)
public:
	size_t vertexIndices[2];	// the index of the vertices that make out the edge

	friend std::ostream& operator <<(std::ostream &os,const Edge &pt);


	Edge(size_t v0, size_t v1, size_t face);
	void lookUpIncidentFaces(int& f1, int &f2) const;

	int lookUpNeighbor(size_t face) const;

	bool operator==(const Edge &other) const;

	bool operator!=(const Edge &other) const;

	void connectFace(size_t face);

};

std::ostream& operator<<(std::ostream& os, const mgl::Edge& e);

} //end namespace

#endif /*MGL_EDGE_H*/
