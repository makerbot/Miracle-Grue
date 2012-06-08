#include "Edge.h"

using namespace mgl;
using namespace std;

#include "log.h"

Edge::Edge(index_t v0, index_t v1, index_t face)
	//:vertexIndices({v0,v1},
	:face0(face), face1(-1)
{
		vertexIndices[0] = v0;
		vertexIndices[1] = v1;

}

void Edge::lookUpIncidentFaces(int& f1, int &f2) const
{
	f1 = face0;
	f2 = face1;
}

int Edge::lookUpNeighbor(index_t face) const
{
	if(face0 == face)
		return face1;
	if (face1 == face)
		return face0;
	assert(0);
	return -1;
}

bool Edge::operator==(const Edge &other) const
{
	assert(&other != this);
	// Compare the values, and return a bool result.
	if(other.vertexIndices[0] == this->vertexIndices[0] && other.vertexIndices[1] == this->vertexIndices[1])
		return true;
	if(other.vertexIndices[1] == this->vertexIndices[0] && other.vertexIndices[0] == this->vertexIndices[1])
		return true;
	return false;
}

bool Edge::operator!=(const Edge &other) const
{
	return !(*this == other);
}

void Edge::connectFace(index_t face)
{
	if(face0 == face)
	{
		cerr << "BAD: edge connecting to the same face twice! " << std::endl;
		cerr << "  dump: " << this << std::endl;
		cerr << ((Edge&)*this) << std::endl;
		assert(0);

	}

	if(face1 == -1)
	{
		face1 = face;
	}
	else
	{
		cerr << "BAD: edge connected to face "<< face0  << " and face1 "<< face1<<  " trying to connecting to face " << face << std::endl;
		cerr << "  dump: " << this << std::endl;
		cerr << ((Edge&)*this) << std::endl;
		assert(0);
	}
}

std::ostream& mgl::operator<<(std::ostream& os, const Edge& e)
{
	os << " " << e.vertexIndices[0] << "\t" << e.vertexIndices[1] << "\t" << e.face0 << "\t" << e.face1;
	return os;
}
