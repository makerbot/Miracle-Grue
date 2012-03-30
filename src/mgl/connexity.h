/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/


#ifndef CONNEXITY_H_
#define CONNEXITY_H_

#include <ostream>
#include <algorithm>
#include <list>
#include <set>

#include "mgl.h"

#include "Edge.h"


namespace mgl // Miracle-Grue's geometry library
{

// it's a triangle, really
class Face
{
public:
	index_t edgeIndices[3];
	index_t vertexIndices[3];
};




// A point
class Vertex
{

public:
	// Vector3 point;
	Vector3 point;
	std::vector<index_t> faces;
};
std::ostream& operator<<(std::ostream& os, const Vertex& v);


index_t findOrCreateVertexIndex(std::vector<Vertex>& vertices,
								const Vector3 &coords,
								Scalar tolerence);


///
/// This class consumes triangles (3 coordinates) and creates a list
/// of vertices, edges, and faces.
/// The connection between them is then restored (mostly).
///
class Connexity
{
	std::vector<Vertex> vertices; // all vertices
	std::vector<Edge> edges;
	std::vector<Face> faces;
	Scalar tolerence;

	friend std::ostream& operator <<(std::ostream &os,const Connexity &pt);

public:


	Connexity(Scalar tolerence);

	const std::vector<Edge>& readEdges() const;

	const std::vector<Face>& readFaces() const;

	const std::vector<Vertex>& readVertices() const;

	index_t addTriangle(const Triangle3 &t);


	// given a face index, this method returns the cached
	void lookupIncidentFacesToFace(index_t faceId, int& face0, int& face1, int& face2) const;


	//
	// Adds all edges that cross the specified z
	//
	void fillEdgeList(Scalar z, std::list<index_t> & crossingEdges) const;

	void dump(std::ostream& out) const;


	// finds 2 neighboring edges
	std::pair<index_t, index_t> edgeToEdges(index_t edgeIndex) const;

	void getAllNeighbors(index_t startFaceIndex, std::set<index_t>& allNeighbors) const;

	index_t cutNextFace(const std::list<index_t> &facesLeft,
							Scalar z,
							index_t startFaceIndex,
							LineSegment2& cut) const;

	bool cutFace(Scalar z, const Face &face, LineSegment2& cut) const;

	void splitLoop(Scalar z, std::list<index_t> &facesLeft, std::list<LineSegment2> &loop) const;

private:


	index_t findOrCreateNewEdge(const Vector3 &coords0, const Vector3 &coords1, size_t face);

	index_t findOrCreateEdge(index_t v0, index_t v1, size_t face);

	index_t findOrCreateVertex(const Vector3 &coords);

};

std::ostream& operator << (std::ostream &os, const Connexity &s);


}

#endif
