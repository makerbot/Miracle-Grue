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

namespace mgl // Miracle-Grue's geometry library
{

// it's a triangle, really
class Face
{
public:
	index_t edgeIndices[3];
	index_t vertexIndices[3];
};

// 3 edges in a Face
class Edge // it's a line.
{
public:
	index_t face0;
	int face1;

	// vertices is plural for vertex (it's a point, really)
	index_t vertexIndices[2];	// the index of the vertices that make out the edge

	friend std::ostream& operator <<(std::ostream &os,const Edge &pt);


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
			std::cout << "BAD: edge connecting to the same face twice! " << std::endl;
			std::cout << "  dump: " << this << std::endl;
			std::cout << ((Edge&)*this) << std::endl;
			assert(0);

		}

		if(face1 == -1)
		{
			face1 = face;
		}
		else
		{
			std::cout << "BAD: edge connected to face "<< face0  << " and face1 "<< face1<<  " trying to connecting to face " << face << std::endl;
			std::cout << "  dump: " << this << std::endl;
			std::cout << ((Edge&)*this) << std::endl;
			assert(0);
		}
	}


};

std::ostream& operator<<(std::ostream& os, const Edge& e);



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


	Connexity(Scalar tolerence)
		:tolerence(tolerence)
	{

	}

	const std::vector<Edge>& readEdges() const
	{
		return edges;
	}

	const std::vector<Face>& readFaces() const
	{
		return faces;
	}

	const std::vector<Vertex>& readVertices() const
	{
		return vertices;
	}

	index_t addTriangle(const Triangle3 &t)
	{
		index_t faceId = faces.size();

//		std::cout << "Slicy::addTriangle " << std::endl;
//		std::cout << "  v0 " << t.vertex1 << " v1" << t.vertex2 << " v3 " << t.vertex3 << std::endl;
//		std::cout << "  id:" << faceId << ": edge (v1,v2, f1,f2)" << std::endl;

		index_t v0 = findOrCreateVertex(t[0]);
		index_t v1 = findOrCreateVertex(t[1]);
		index_t v2 = findOrCreateVertex(t[2]);

		Face face;
		face.edgeIndices[0] = findOrCreateEdge(v0, v1, faceId);
//		std::cout << "   a) " << face.edge0 << "(" << edges[face.edge0] << ")" << std::endl;
		face.edgeIndices[1] = findOrCreateEdge(v1, v2, faceId);
//		std::cout << "   b) " << face.edge1 << "(" << edges[face.edge1] << ")" << std::endl;
		face.edgeIndices[2] = findOrCreateEdge(v2, v0, faceId);
//		std::cout << "   c) " << face.edge2 << "(" << edges[face.edge2] << ")" << std::endl;

		face.vertexIndices[0] = v0;
		face.vertexIndices[1] = v1;
		face.vertexIndices[2] = v2;

		// Update list of neighboring faces
		faces.push_back(face);
		vertices[v0].faces.push_back(faceId);
		vertices[v1].faces.push_back(faceId);
		vertices[v2].faces.push_back(faceId);




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



	//
	// Adds all edges that cross the specified z
	//
	void fillEdgeList(Scalar z, std::list<index_t> & crossingEdges) const
	{
		assert(crossingEdges.size() == 0);
		for (index_t i=0; i < edges.size(); i++)
		{
			const Edge &e= edges[i];
			index_t v0 = e.vertexIndices[0];
			index_t v1 = e.vertexIndices[1];

			const Vector3 &p0 = vertices[v0].point;
			const Vector3 &p1 = vertices[v1].point;

			Scalar min = p0.z;
			Scalar max = p1.z;
			if(min > max)
			{
				min = p1.z;
				max = p0.z;
			}
			// The z less or equal to max while z strictly larger than min
			// Prevents, in the author's opinion, the possibility of having
			// 2 edges for the same point. It is also better for the case of
			// a very flat 3d object with a height that is precisely equal to
			// the first layer height
			if ( (max-min > 0) && (z > min)  && (z <= max) )
			{
				crossingEdges.push_back(i);
			}
		}
	}

	void dump(std::ostream& out) const
	{
		out << "Slicy" << std::endl;
		out << "  vertices: coords and face list" << vertices.size() << std::endl;
		out << "  edges: " << edges.size() << std::endl;
		out << "  faces: " << faces.size() << std::endl;

		std::cout << std::endl;

		std::cout << "Vertices:" << std::endl;

		int x =0;
		for(std::vector<Vertex>::const_iterator i = vertices.begin(); i != vertices.end(); i++ )
		{
			std::cout << x << ": " << *i << std::endl;
			x ++;
		}

		std::cout << std::endl;
		std::cout << "Edges (vertex 1, vertex2, face 1, face2)" << std::endl;

		x =0;
		for(std::vector<Edge>::const_iterator i = edges.begin(); i != edges.end(); i++)
		{
			std::cout << x << ": " << *i << std::endl;
			x ++;
		}
	}

	// finds 2 neighboring edges
	std::pair<index_t, index_t> edgeToEdges(index_t edgeIndex) const
	{
		std::pair<index_t, index_t> ret;
		const Edge &startEdge = edges[edgeIndex];
		index_t faceIndex = startEdge.face0;
		const Face &face = faces[faceIndex];

		unsigned int it = 0;
		ret.first = face.edgeIndices[it];
		it++;
		if(ret.first == edgeIndex)
		{
			ret.first = face.edgeIndices[it];
			it++;
		}
		ret.second = face.edgeIndices[it];
		if(ret.second == edgeIndex)
		{
			it++;
			ret.second = face.edgeIndices[it];
		}
		return ret;
	}

	void getAllNeighbors(index_t startFaceIndex, std::set<index_t>& allNeighbors) const
	{
		const Face &face = faces[startFaceIndex];
		const std::vector<index_t>& neighbors0 = vertices[ face.vertexIndices[0]].faces;
		const std::vector<index_t>& neighbors1 = vertices[ face.vertexIndices[1]].faces;
		const std::vector<index_t>& neighbors2 = vertices[ face.vertexIndices[2]].faces;


		for(size_t i=0; i< neighbors0.size(); i++)
		{
			index_t faceId = neighbors0[i];
			if (faceId >=0  && faceId != startFaceIndex)
			{
				allNeighbors.insert(faceId);
			}
		}
		for(size_t i=0; i< neighbors1.size(); i++)
		{
			index_t faceId = neighbors1[i];
			if (faceId >=0  && faceId != startFaceIndex)
			{
				allNeighbors.insert(faceId);
			}
		}
		for(size_t i=0; i< neighbors2.size(); i++)
		{
			index_t faceId = neighbors2[i];
			if (faceId >=0  && faceId != startFaceIndex)
			{
				allNeighbors.insert(faceId);
			}
		}

		//std::cout << "All Neighbors of face:" << startFaceIndex <<":" << neighbors0.size() << ", " << neighbors1.size() << ", " << neighbors2.size() << std::endl;
		//for(std::set<index_t>::iterator i= allNeighbors.begin(); i != allNeighbors.end(); i++)
		//{
		//	std::cout << " >" << *i << std::endl;
		//}
	}

	index_t cutNextFace(const std::list<index_t> &facesLeft,
							Scalar z,
							index_t startFaceIndex,
							LineSegment2d& cut) const
	{
		std::set<index_t> allNeighbors;
		getAllNeighbors(startFaceIndex, allNeighbors);

		for(std::set<index_t>::iterator i= allNeighbors.begin(); i != allNeighbors.end(); i++)
		{
			index_t faceIndex = *i;
			// use it only if its in the list of triangles left
			if(find(facesLeft.begin(), facesLeft.end(), faceIndex) != facesLeft.end())
			{
				const Face& face = faces[faceIndex];
				if( cutFace(z, face, cut))
				{
			//		std::cout << " " << faceIndex << " CUTS it!" << std::endl;
					return faceIndex;
				}
			}

		}

		return -1;
	}


	bool cutFace(Scalar z, const Face &face, LineSegment2d& cut) const
	{


		const Vertex& v0 = vertices[face.vertexIndices[0]];
		const Vertex& v1 = vertices[face.vertexIndices[1]];
		const Vertex& v2 = vertices[face.vertexIndices[2]];

		Vector3 a(v0.point.x, v0.point.y, v0.point.z);
		Vector3 b(v1.point.x, v1.point.y, v1.point.z);
		Vector3 c(v2.point.x, v2.point.y, v2.point.z);
		Triangle3 triangle(a,b,c);

		bool success = triangle.cut( z, a, b);

		cut.a.x = a.x;
		cut.a.y = a.y;
		cut.b.x = b.x;
		cut.b.y = b.y;

		return success;
	}

	void splitLoop(Scalar z, std::list<index_t> &facesLeft, std::list<LineSegment2d> &loop) const
	{
		assert(loop.size() == 0);
		assert(facesLeft.size() > 0);

		bool firstCutFound = false;
		LineSegment2d cut;

		index_t faceIndex;
		while (!firstCutFound)
		{
			if(facesLeft.size() ==0) return;

			faceIndex = *facesLeft.begin();
			facesLeft.remove(faceIndex);
			const Face &face = faces[faceIndex];
			firstCutFound = cutFace(z, face, cut);

		}

		// a cut is found
		loop.push_back(cut);

		bool loopEnd = false;
		while(!loopEnd)
		{
			faceIndex = *facesLeft.begin();
			facesLeft.remove(faceIndex);
			std::cout << "Current face index:" <<  faceIndex << std::endl;
			faceIndex = cutNextFace(facesLeft, z, faceIndex, cut);
			if(faceIndex >= 0)
			{
				facesLeft.remove(faceIndex);
				loop.push_back(cut);
			}
			else
			{
				loopEnd = true;
			}
			if(facesLeft.size() == 0)
			{
				loopEnd = true;
			}
		}
	}



private:


	index_t findOrCreateNewEdge(const Vector3 &coords0, const Vector3 &coords1, size_t face)
	{
		index_t v0 = findOrCreateVertex(coords0);
		index_t v1 = findOrCreateVertex(coords1);
		findOrCreateEdge(v0, v1, face);
	}

	index_t findOrCreateEdge(index_t v0, index_t v1, size_t face)
	{

		Edge e(v0, v1, face);
		index_t edgeIndex;

		std::vector<Edge>::iterator it = find(edges.begin(), edges.end(), e);
		if(it == edges.end())
		{
			// std::cout << "NEW EDGE " << coords << std::endl;
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

	index_t findOrCreateVertex(const Vector3 &coords)
	{
		return findOrCreateVertexIndex(vertices, coords, tolerence);
	}
};
std::ostream& operator << (std::ostream &os, const Connexity &s);


}

#endif
