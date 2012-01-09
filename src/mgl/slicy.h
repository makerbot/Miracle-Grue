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
#include "core.h"

namespace mgl // Miracle-Grue's geometry library
{

class Face // it's a triangle, really
{
public:
	index_t edgeIndices[3];
	index_t vertexIndices[3];
};

class Edge // it's a line.
{
public:
	index_t face0;
	int face1;


	// vertices is plural for vertex (it's a point, really)
	index_t vertexIndices[2];	// the index of the vertices that make out the edge

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

class Vertex
{

public:
	BGL::Point3d point;
	std::vector<index_t> faces;
};

std::ostream& operator<<(ostream& os, const Vertex& v)
{
	os << " " << v.point << "\t[ ";
	for (int i=0; i< v.faces.size(); i++)
	{
		if (i>0)  os << ", ";
		os << v.faces[i];
	}
	os << "]";
	return os;
}

index_t findOrCreateVertexIndex(std::vector<Vertex>& vertices ,const BGL::Point3d &coords, Scalar tolerence)
{

	for(vector<Vertex>::iterator it = vertices.begin(); it != vertices.end(); it++)
	{
		const BGL::Point3d &p = (*it).point;
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
	Vertex vertex;
	vertex.point = coords;
	vertices.push_back(vertex);
	vertexIndex = vertices.size() -1;
	return vertexIndex;
}


///
/// This class consumes triangles (3 coordinates) and creates a list
/// of vertices, edges, and faces.
/// The connection between them is then restored (mostly).
///
class Slicy
{
	std::vector<Vertex> vertices; // all vertices
	std::vector<Edge> edges;
	std::vector<Face> faces;
	Scalar tolerence;

	friend std::ostream& operator <<(ostream &os,const Slicy &pt);

public:


	Slicy (Scalar tolerence)
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

	index_t addTriangle(const BGL::Triangle3d &t)
	{
		index_t faceId = faces.size();

//		cout << "Slicy::addTriangle " << endl;
//		cout << "  v0 " << t.vertex1 << " v1" << t.vertex2 << " v3 " << t.vertex3 << endl;
//		cout << "  id:" << faceId << ": edge (v1,v2, f1,f2)" << endl;

		index_t v0 = findOrCreateVertex(t.vertex1);
		index_t v1 = findOrCreateVertex(t.vertex2);
		index_t v2 = findOrCreateVertex(t.vertex3);

		Face face;
		face.edgeIndices[0] = findOrCreateEdge(v0, v1, faceId);
//		cout << "   a) " << face.edge0 << "(" << edges[face.edge0] << ")" << endl;
		face.edgeIndices[1] = findOrCreateEdge(v1, v2, faceId);
//		cout << "   b) " << face.edge1 << "(" << edges[face.edge1] << ")" << endl;
		face.edgeIndices[2] = findOrCreateEdge(v2, v0, faceId);
//		cout << "   c) " << face.edge2 << "(" << edges[face.edge2] << ")" << endl;

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

			const BGL::Point3d &p0 = vertices[v0].point;
			const BGL::Point3d &p1 = vertices[v1].point;

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
		out << "Slicy" << endl;
		out << "  vertices: coords and face list" << vertices.size() << endl;
		out << "  edges: " << edges.size() << endl;
		out << "  faces: " << faces.size() << endl;

		cout << endl;

		cout << "Vertices:" << endl;

		int x =0;
		for(vector<Vertex>::const_iterator i = vertices.begin(); i != vertices.end(); i++ )
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
		const vector<index_t>& neighbors0 = vertices[ face.vertexIndices[0]].faces;
		const vector<index_t>& neighbors1 = vertices[ face.vertexIndices[1]].faces;
		const vector<index_t>& neighbors2 = vertices[ face.vertexIndices[2]].faces;


		for(int i=0; i< neighbors0.size(); i++)
		{
			index_t faceId = neighbors0[i];
			if (faceId >=0  && faceId != startFaceIndex)
			{
				allNeighbors.insert(faceId);
			}
		}
		for(int i=0; i< neighbors1.size(); i++)
		{
			index_t faceId = neighbors1[i];
			if (faceId >=0  && faceId != startFaceIndex)
			{
				allNeighbors.insert(faceId);
			}
		}
		for(int i=0; i< neighbors2.size(); i++)
		{
			index_t faceId = neighbors2[i];
			if (faceId >=0  && faceId != startFaceIndex)
			{
				allNeighbors.insert(faceId);
			}
		}

		//cout << "All Neighbors of face:" << startFaceIndex <<":" << neighbors0.size() << ", " << neighbors1.size() << ", " << neighbors2.size() << endl;
		//for(std::set<index_t>::iterator i= allNeighbors.begin(); i != allNeighbors.end(); i++)
		//{
		//	cout << " >" << *i << endl;
		//}
	}

	index_t cutNextFace(const std::list<index_t> &facesLeft, Scalar z, index_t startFaceIndex, Segment& cut) const
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
			//		cout << " " << faceIndex << " CUTS it!" << endl;
					return faceIndex;
				}
			}

		}

		return -1;
	}


	bool cutFace(Scalar z, const Face &face, Segment& cut) const
	{
		//	bool mgl::sliceTriangle(const BGL::Point3d& vertex1,
		//						 const BGL::Point3d& vertex2,
		//							const BGL::Point3d& vertex3,
		//							   Scalar Z, BGL::Point &a,
		//							      BGL::Point &b)

		const Vertex& v0 = vertices[face.vertexIndices[0]];
		const Vertex& v1 = vertices[face.vertexIndices[1]];
		const Vertex& v2 = vertices[face.vertexIndices[2]];

		Vector3d a(v0.point.x, v0.point.y, v0.point.z);
		Vector3d b(v1.point.x, v1.point.y, v1.point.z);
		Vector3d c(v2.point.x, v2.point.y, v2.point.z);
		Triangle3 triangle(a,b,c);

		Vector3d dir = triangle.cutDirection();
		bool success = sliceTriangle(v0.point, v1.point, v2.point, z, cut.a, cut.b );

		Vector3d segmentDir( cut.b.x - cut.a.x, cut.b.y - cut.a.y, z);

		if(dir.dotProduct(segmentDir) < 0 )
		{
			cout << "INVERTED SEGMENT DETECTED" << endl;
			BGL::Point p = cut.a;
			cut.a = cut.b;
			cut.b = p;
		}

		return success;
	}

	void splitLoop(Scalar z, std::list<index_t> &facesLeft, std::list<Segment> &loop) const
	{
		assert(loop.size() == 0);
		assert(facesLeft.size() > 0);

		bool firstCutFound = false;
		Segment cut;

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
			cout << "Current face index:" <<  faceIndex << endl;
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


	index_t findOrCreateNewEdge(const BGL::Point3d &coords0, const BGL::Point3d &coords1, size_t face)
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
		return findOrCreateVertexIndex(vertices, coords, tolerence);
	}
};

std::ostream& operator << (std::ostream &os, const Slicy &s)
{
	s.dump(os);
	return os;
}



#endif /* SLICY_H_ */

}
