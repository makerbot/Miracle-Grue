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

index_t mgl::findOrCreateVertexIndex(std::vector<Vertex>& vertices ,
								const Vector3 &coords,
								Scalar tolerence)
{

	for(std::vector<Vertex>::iterator it = vertices.begin(); it != vertices.end(); it++)
	{
		//const Vector3 &p = (*it).point;
		Vector3 &p = (*it).point;
		Scalar dx = coords.x - p.x;
		Scalar dy = coords.y - p.y;
		Scalar dz = coords.z - p.z;

		Scalar dd =  dx * dx + dy * dy + dz * dz;
		if( dd < tolerence )
		{
			//std::cout << "Found VERTEX" << std::endl;
			index_t vertexIndex = std::distance(vertices.begin(), it);
			return vertexIndex;
		}
	}

	index_t vertexIndex;
	// std::cout << "NEW VERTEX " << coords << std::endl;
	Vertex vertex;
	vertex.point = coords;
	vertices.push_back(vertex);
	vertexIndex = vertices.size() -1;
	return vertexIndex;
}


std::ostream& mgl::operator<<(std::ostream& os, const Edge& e)
{
	os << " " << e.vertexIndices[0] << "\t" << e.vertexIndices[1] << "\t" << e.face0 << "\t" << e.face1;
	return os;
}

std::ostream& mgl::operator<<(std::ostream& os, const Vertex& v)
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

std::ostream& mgl::operator << (std::ostream &os, const Connexity &s)
{
	s.dump(os);
	return os;
}

