/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/


#ifndef SLICY_H_
#define SLICY_H_

#include <ostream>
#include "BGL/BGLPoint3d.h"

namespace mgl // Miracle-Grue's geometry library
{

class Face // it's a triangle, really
{
public:
	index_t edgeIndices[3];
};

class Edge // it's a line.
{
	// vertices is plural for vertex (it's a point, really)
	index_t vertexIndices[2];	// the index of the vertices that make out the edge
	index_t face0;
	int face1;


public:
	friend std::ostream& operator <<(ostream &os,const Edge &pt);


	Edge(index_t v0, index_t v1, index_t face)
		//:vertexIndices({v0,v1},
		:face0(face), face1(-1)
	{
			vertexIndices[0] = v0;
			vertexIndices[1] = v1;

	}

	void lookUpIncidentFaces(int& f1, int &f2) const
	{
		f1 = face0;
		f2 = face1;
	}

	int lookUpNeighbor(index_t face) const
	{
		if(face0 == face)
			return face1;
		if (face1 == face)
			return face0;
		assert(0);
		return -1;
	}

	bool operator==(const Edge &other) const
	{
		assert(&other != this);
	    // Compare the values, and return a bool result.
		if(other.vertexIndices[0] == this->vertexIndices[0] && other.vertexIndices[1] == this->vertexIndices[1])
			return true;
		if(other.vertexIndices[1] == this->vertexIndices[0] && other.vertexIndices[0] == this->vertexIndices[1])
			return true;
		return false;
	}

	bool operator!=(const Edge &other) const
	{
	    return !(*this == other);
	}

	void connectFace(index_t face)
	{
		if(face0 == face)
		{
			cout << "BAD: edge connecting to the same face twice! " << endl;
			cout << "  dump: " << this << endl;
			cout << ((Edge&)*this) << endl;
			assert(0);

		}

		if(face1 == -1)
		{
			face1 = face;
		}
		else
		{
			cout << "BAD: edge connected to face "<< face0  << " and face1 "<< face1<<  " trying to connecting to face " << face << endl;
			cout << "  dump: " << this << endl;
			cout << ((Edge&)*this) << endl;
			assert(0);
		}
	}

};

std::ostream& operator<<(ostream& os, const Edge& e)
{
	os << " " << e.vertexIndices[0] << "\t" << e.vertexIndices[1] << "\t" << e.face0 << "\t" << e.face1;
	return os;
}



///
/// This class consumes triangles (3 coordinates) and creates a list
/// of vertices, edges, and faces.
/// The connection between them is then restored (mostly).
///


class Slicy
{
	std::vector<BGL::Point3d> vertices;
	std::vector<Edge> edges;
	std::vector<Face> faces;
	Scalar tolerence;

	friend std::ostream& operator <<(ostream &os,const Slicy &pt);

public:


	Slicy (Scalar tolerence)
		:tolerence(tolerence)
	{

	}

	index_t addTriangle(const BGL::Triangle3d &t)
	{
		index_t faceId = faces.size();

//		cout << "Slicy::addTriangle " << endl;
//		cout << "  v0 " << t.vertex1 << " v1" << t.vertex2 << " v3 " << t.vertex3 << endl;
//		cout << "  id:" << faceId << ": edge (v1,v2, f1,f2)" << endl;
		Face face;
		face.edgeIndices[0] = findOrCreateEdge(t.vertex1, t.vertex2, faceId);
//		cout << "   a) " << face.edge0 << "(" << edges[face.edge0] << ")" << endl;
		face.edgeIndices[1] = findOrCreateEdge(t.vertex2, t.vertex3, faceId);
//		cout << "   b) " << face.edge1 << "(" << edges[face.edge1] << ")" << endl;
		face.edgeIndices[2] = findOrCreateEdge(t.vertex3, t.vertex1, faceId);
//		cout << "   c) " << face.edge2 << "(" << edges[face.edge2] << ")" << endl;

		/*
		cout << "EDGE 0: index " << face.edge0 << " : " << edges[face.edge0] << endl;
		face.edge1 = findOrCreateEdge(t.vertex2, t.vertex3, faceId);
		cout << "EDGE 1: index " << face.edge0 << " edge (v1,v2, f1,f2): " << edges[face.edge0] << endl;
		cout << "EDGE 2: index " << face.edge0 << " edge (v1,v2, f1,f2): " << edges[face.edge0] << endl;
		 */
		faces.push_back(face);
		return faces.size() -1;
	}


	// given a face index, this method returns the cached
	void lookupIncidentFacesToFace(index_t faceId, int& face0, int& face1, int& face2) const
	{
		const Face& face = faces[faceId];

		const Edge &e0 = edges[face.edgeIndices[0] ];
		const Edge &e1 = edges[face.edgeIndices[1] ];
		const Edge &e2 = edges[face.edgeIndices[2] ];

		face0 = e0.lookUpNeighbor(faceId);
		face1 = e1.lookUpNeighbor(faceId);
		face2 = e2.lookUpNeighbor(faceId);

	}



	void dump(std::ostream& out) const
	{
		out << "Slicy" << endl;
		out << "  vertices: " << vertices.size() << endl;
		out << "  edges: " << edges.size() << endl;
		out << "  faces: " << faces.size() << endl;

		cout << endl;

		cout << "Vertices:" << endl;

		int x =0;
		for(vector<BGL::Point3d>::const_iterator i = vertices.begin(); i != vertices.end(); i++ )
		{
			cout << x << ": " << *i << endl;
			x ++;
		}

		cout << endl;
		cout << "Edges (vertex 1, vertex2, face 1, face2)" << endl;

		x =0;
		for(vector<Edge>::const_iterator i = edges.begin(); i != edges.end(); i++)
		{
			cout << x << ": " << *i << endl;
			x ++;
		}

	}
private:
	index_t findOrCreateEdge(const BGL::Point3d &coords0, const BGL::Point3d &coords1, size_t face)
	{
		index_t v0 = findOrCreateVertex(coords0);
		index_t v1 = findOrCreateVertex(coords1);

		Edge e(v0, v1, face);
		index_t edgeIndex;

		std::vector<Edge>::iterator it = find(edges.begin(), edges.end(), e);
		if(it == edges.end())
		{
			// cout << "NEW EDGE " << coords << endl;
			edges.push_back(e);
			edgeIndex = edges.size() -1;
		}
		else
		{
			it->connectFace(face);
			edgeIndex = std::distance(edges.begin(), it);
		}
		return edgeIndex;
	}


	index_t findOrCreateVertex(const BGL::Point3d &coords)
	{
		for(vector<BGL::Point3d>::iterator it = vertices.begin(); it != vertices.end(); it++)
		{
			const BGL::Point3d &p = (*it);
			Scalar dx = coords.x - p.x;
			Scalar dy = coords.y - p.y;
			Scalar dz = coords.z - p.z;

			Scalar dd =  dx * dx + dy * dy + dz * dz;
			if( dd < tolerence )
			{
				//cout << "Found VERTEX" << endl;
				index_t vertexIndex = std::distance(vertices.begin(), it);
				return vertexIndex;
			}
		}

		index_t vertexIndex;
		// cout << "NEW VERTEX " << coords << endl;
		vertices.push_back(coords);
		vertexIndex = vertices.size() -1;
		return vertexIndex;
	}
};

std::ostream& operator << (std::ostream &os, const Slicy &s)
{
	s.dump(os);
	return os;
}



#endif /* SLICY_H_ */

}
