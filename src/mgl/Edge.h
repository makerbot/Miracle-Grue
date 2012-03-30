#ifndef MGL_EDGE_H
#define MGL_EDGE_H (1)

#include "mgl.h"

namespace mgl{

// 3 edges in a Face
class Edge // it's a line.
{
public:
	index_t face0;
	int face1;

	// vertices is plural for vertex (it's a point, really)
public:
	index_t vertexIndices[2];	// the index of the vertices that make out the edge

	friend std::ostream& operator <<(std::ostream &os,const Edge &pt);


	Edge(index_t v0, index_t v1, index_t face);
	void lookUpIncidentFaces(int& f1, int &f2) const;

	int lookUpNeighbor(index_t face) const;

	bool operator==(const Edge &other) const;

	bool operator!=(const Edge &other) const;

	void connectFace(index_t face);

};

std::ostream& operator<<(std::ostream& os, const mgl::Edge& e);

} //end namespace

#endif /*MGL_EDGE_H*/
