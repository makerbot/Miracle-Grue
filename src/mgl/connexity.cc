/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/

#include "connexity.h"

using namespace mgl;
using namespace std;


#include "log.h"
#include "mgl.h"

index_t mgl::findOrCreateVertexIndex(std::vector<Vertex>& vertices ,
								const Point3Type &coords,
								Scalar tolerence)
{

	for(std::vector<Vertex>::iterator it = vertices.begin(); it != vertices.end(); it++)
	{
		//const Vector3 &p = (*it).point;
		Point3Type &p = (*it).point;
		Scalar dx = coords.x - p.x;
		Scalar dy = coords.y - p.y;
		Scalar dz = coords.z - p.z;

		Scalar dd =  dx * dx + dy * dy + dz * dz;
		if( dd < tolerence )
		{
			Log::info() << "Found VERTEX" << std::endl;
			index_t vertexIndex = std::distance(vertices.begin(), it);
			return vertexIndex;
		}
	}

	index_t vertexIndex;
	Log::info() << "NEW VERTEX " << coords << std::endl;
	Vertex vertex;
	vertex.point = coords;
	vertices.push_back(vertex);
	vertexIndex = vertices.size() -1;
	return vertexIndex;
}



Connexity::Connexity(Scalar tolerence)
	:tolerence(tolerence)
{

}


const std::vector<Edge>& Connexity::readEdges() const
{
	return edges;
}

const std::vector<Face>& Connexity::readFaces() const
{
	return faces;
}

const std::vector<Vertex>& Connexity::readVertices() const
{
	return vertices;
}


index_t Connexity::addTriangle(const Triangle3 &t)
{
	index_t faceId = faces.size();

		Log::finest() << "Slicy::addTriangle " << std::endl;
		Log::finest() << "  v0 " << t[0] << " v1" << t[1] << " v3 " << t[2] << std::endl;
		Log::finest() << "  id:" << faceId << ": edge (v1,v2, f1,f2)" << std::endl;

	index_t v0 = findOrCreateVertex(t[0]);
	index_t v1 = findOrCreateVertex(t[1]);
	index_t v2 = findOrCreateVertex(t[2]);

	Face face;
	face.edgeIndices[0] = findOrCreateEdge(v0, v1, faceId);
	//Log::finest() << "   a) " << face.edge0 << "(" << edges[face.edge0] << ")" << std::endl;
	face.edgeIndices[1] = findOrCreateEdge(v1, v2, faceId);
	//Log::finest() << "   b) " << face.edge1 << "(" << edges[face.edge1] << ")" << std::endl;
	face.edgeIndices[2] = findOrCreateEdge(v2, v0, faceId);
	//Log::finest() << "   c) " << face.edge2 << "(" << edges[face.edge2] << ")" << std::endl;

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
void Connexity::lookupIncidentFacesToFace(index_t faceId, int& face0, int& face1, int& face2) const
{
	const Face& face = faces[faceId];

	const Edge &e0 = edges[face.edgeIndices[0] ];
	const Edge &e1 = edges[face.edgeIndices[1] ];
	const Edge &e2 = edges[face.edgeIndices[2] ];

	face0 = e0.lookUpNeighbor(faceId);
	face1 = e1.lookUpNeighbor(faceId);
	face2 = e2.lookUpNeighbor(faceId);

}

void Connexity::fillEdgeList(Scalar z, std::list<index_t> & crossingEdges) const
{
	assert(crossingEdges.size() == 0);
	for (index_t i=0; i < edges.size(); i++)
	{
		const Edge &e= edges[i];
		index_t v0 = e.vertexIndices[0];
		index_t v1 = e.vertexIndices[1];

		const Point3Type &p0 = vertices[v0].point;
		const Point3Type &p1 = vertices[v1].point;

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


void Connexity::dump(std::ostream& out) const
{
	out << "Slicy" << std::endl;
	out << "  vertices: coords and face list" << vertices.size() << std::endl;
	out << "  edges: " << edges.size() << std::endl;
	out << "  faces: " << faces.size() << std::endl;

	Log::info() << std::endl;

	Log::info() << "Vertices:" << std::endl;

	int x =0;
	for(std::vector<Vertex>::const_iterator i = vertices.begin(); i != vertices.end(); i++ )
	{
		Log::info() << x << ": " << *i << std::endl;
		x ++;
	}

	Log::info() << std::endl;
	Log::info() << "Edges (vertex 1, vertex2, face 1, face2)" << std::endl;

	x =0;
	for(std::vector<Edge>::const_iterator i = edges.begin(); i != edges.end(); i++)
	{
		Log::info() << x << ": " << *i << std::endl;
		x ++;
	}
}



// finds 2 neighboring edges
std::pair<index_t, index_t> Connexity::edgeToEdges(index_t edgeIndex) const
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


void Connexity::getAllNeighbors(index_t startFaceIndex, std::set<index_t>& allNeighbors) const
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

	Log::info() << "All Neighbors of face:" << startFaceIndex <<":" << neighbors0.size() << ", " << neighbors1.size() << ", " << neighbors2.size() << std::endl;
	for(std::set<index_t>::iterator i= allNeighbors.begin(); i != allNeighbors.end(); i++)
	{
		Log::info() << " >" << *i << std::endl;
	}
}

index_t Connexity::cutNextFace(const std::list<index_t> &facesLeft,
						Scalar z,
						index_t startFaceIndex,
						Segment2Type& cut) const
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
				Log::info() << " " << faceIndex << " CUTS it!" << std::endl;
				return faceIndex;
			}
		}

	}

	return -1;
}


bool Connexity::cutFace(Scalar z, const Face &face, Segment2Type& cut) const
{


	const Vertex& v0 = vertices[face.vertexIndices[0]];
	const Vertex& v1 = vertices[face.vertexIndices[1]];
	const Vertex& v2 = vertices[face.vertexIndices[2]];

	Point3Type a(v0.point.x, v0.point.y, v0.point.z);
	Point3Type b(v1.point.x, v1.point.y, v1.point.z);
	Point3Type c(v2.point.x, v2.point.y, v2.point.z);
	Triangle3 triangle(a,b,c);

	bool success = triangle.cut( z, a, b);

	cut.a.x = a.x;
	cut.a.y = a.y;
	cut.b.x = b.x;
	cut.b.y = b.y;

	return success;
}


void Connexity::splitLoop(Scalar z, std::list<index_t> &facesLeft, std::list<Segment2Type> &loop) const
{
	assert(loop.size() == 0);
	assert(facesLeft.size() > 0);

	bool firstCutFound = false;
	Segment2Type cut;

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
		Log::info() << "Current face index:" <<  faceIndex << std::endl;
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


index_t Connexity::findOrCreateNewEdge(const Point3Type &coords0, 
		const Point3Type &coords1, size_t face) {
	index_t v0 = findOrCreateVertex(coords0);
	index_t v1 = findOrCreateVertex(coords1);
	findOrCreateEdge(v0, v1, face);
	return 0;
}

index_t Connexity::findOrCreateEdge(index_t v0, index_t v1, size_t face)
{

	Edge e(v0, v1, face);
	index_t edgeIndex;

	std::vector<Edge>::iterator it = find(edges.begin(), edges.end(), e);
	if(it == edges.end())
	{
		//Log::finest() << "NEW EDGE " << edges << std::endl;
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

index_t Connexity::findOrCreateVertex(const Point3Type &coords)
{
	return findOrCreateVertexIndex(vertices, coords, tolerence);
}


std::ostream& mgl::operator<<(std::ostream& os, const Vertex& v)
{
	os << " " << v.point << "\t[ ";
	for (size_t i=0; i< v.faces.size(); i++)
	{
		if (i>0)  os << ", ";
		os << v.faces[i];
	}
	os << "]";
	return os;
}

std::ostream& mgl::operator << (std::ostream &os, const Connexity &s)
{
	s.dump(os);
	return os;
}

