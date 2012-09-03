
#include "meshy.h"
#include "mgl.h"

#include <iostream>
#include <algorithm>
#include <string>
#include <stdint.h>
#include <cstring>
#include <list>
#include <sstream>

#include "log.h"

namespace mgl {

using namespace std;
using namespace libthing;

#ifdef __BYTE_ORDER
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define I_AM_LITTLE_ENDIAN
#else
#if __BYTE_ORDER == __BIG_ENDIAN
#define I_AM_BIG_ENDIAN
#else
#error "Unknown byte order!"
#endif
#endif
#endif /* __BYTE_ORDER */

#ifdef I_AM_BIG_ENDIAN

static inline void convertFromLittleEndian32(uint8_t* bytes) {
	uint8_t tmp = bytes[0];
	bytes[0] = bytes[3];
	bytes[3] = tmp;
	tmp = bytes[1];
	bytes[1] = bytes[2];
	bytes[2] = tmp;
}

static inline void convertFromLittleEndian16(uint8_t* bytes) {
	uint8_t tmp = bytes[0];
	bytes[0] = bytes[1];
	bytes[1] = tmp;
}
#else

static inline void convertFromLittleEndian32(uint8_t*)// bytes)
{
}

static inline void convertFromLittleEndian16(uint8_t*)// bytes)
{
}
#endif

void StlWriter::open(const char* fileName, const char *solid){
	solidName = solid;
	out.open(fileName);
	if (!out) {
		std::stringstream ss;
		ss << "Can't open \"" << fileName << "\"";
		MeshyException problem(ss.str().c_str());
		throw(problem);
	}

	// bingo!
	out << std::scientific;
	out << "solid " << solidName << std::endl;
}
void StlWriter::writeTriangle(const libthing::Triangle3& t) {
	// normalize( (v1-v0) cross (v2 - v0) )
	// y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x

	libthing::Vector3 n = t.normal();
	out << " facet normal " << n[0] << " " << n[1] << " " << n[2] << std::endl;
	out << "  outer loop" << std::endl;
	out << "    vertex " << t[0].x << " " << t[0].y << " " << t[0].z << std::endl;
	out << "    vertex " << t[1].x << " " << t[1].y << " " << t[1].z << std::endl;
	out << "    vertex " << t[2].x << " " << t[2].y << " " << t[2].z << std::endl;
	out << "  endloop" << std::endl;
	out << " endfacet" << std::endl;
}
void StlWriter::close() {
		out << "endsolid " << solidName << std::endl;
		out.close();
	}

/// requires firstLayerSlice height, and general layer height

Meshy::Meshy() {}

const std::vector<Triangle3>& Meshy::readAllTriangles() const {
	return allTriangles;
}

const Limits& Meshy::readLimits() const {
	return limits;
}


//
// Adds buffered triangles to allTriangles
//

void Meshy::flushBuffer() {
	while(!bufferedTriangles.empty()){
		addTriangle(bufferedTriangles.front());
		bufferedTriangles.pop_front();
	}
}


//
// Adds a triangle to the buffer of triangles to be analyzed
//

void Meshy::bufferTriangle(libthing::Triangle3 t){
	bufferedTriangles.push_back(t);
}

//
// Adds a triangle to the global array and for each slice of interest
//

void Meshy::addTriangle(Triangle3 &t) {

	allTriangles.push_back(t);

	limits.grow(t[0]);
	limits.grow(t[1]);
	limits.grow(t[2]);
}

void Meshy::dump(std::ostream &out) {
	out << "dumping " << this << std::endl;
	out << "Nb of triangles: " << allTriangles.size() << std::endl;
//	size_t sliceCount = sliceTable.size();
//
//	out << "triangles per slice: (" << sliceCount << " slices)" << std::endl;
//	for (size_t i = 0; i < sliceCount; i++) {
//		TriangleIndices &trianglesForSlice = sliceTable[i];
//		//trianglesForSlice.push_back(newTriangleId);
//		out << "  slice " << i << " size: " << trianglesForSlice.size() << std::endl;
		//Log::often() << "adding triangle " << newTriangleId << " to layer " << i << std::endl;
//	}
}

//
// Perform slice updates on a triangle (this will move to new class)
//

size_t Meshy::triangleCount() {
	return allTriangles.size();
	Log::info() << "all triangle count" << allTriangles.size();
}

void Meshy::writeStlFile(const char* fileName) const {
	StlWriter out;
	out.open(fileName);
	size_t triCount = allTriangles.size();
	for (size_t i = 0; i < triCount; i++) {
		const Triangle3 &t = allTriangles[i];
		out.writeTriangle(t);
	}
	out.close();
	// Log::often() << fileName << " written!"<< std::endl;

}

//void Meshy::writeStlFileForLayer(unsigned int layerIndex, const char* fileName) const {
//
//	StlWriter out;
//	out.open(fileName);
//
//	const TriangleIndices &trianglesForSlice = sliceTable[layerIndex];
//	for (std::vector<index_t>::const_iterator i = trianglesForSlice.begin(); i != trianglesForSlice.end(); i++) {
//		index_t index = *i;
//		const Triangle3 &t = allTriangles[index];
//		out.writeTriangle(t);
//	}
//	out.close();
//	// Log::often() << fileName << " written!"<< std::endl;
//}


/// Loads an STL file into a mesh object, from a binary or ASCII stl file.
///
/// @param stlFilename target file to load into the specified mesh
///
/// @returns count of triangles loaded into this mesh by this call

size_t Meshy::readAsciiStlFile(const char* stlFilename) {
	struct vertexes_t {
		float nx, ny, nz;
		float x1, y1, z1;
		float x2, y2, z2;
		float x3, y3, z3;
		uint16_t attrBytes;
	};

	union {
		struct vertexes_t vertexes;
		uint8_t bytes[sizeof(vertexes_t)];
	} tridata;

	uint8_t buf[512];
	FILE *fHandle = fopen(stlFilename, "rb");
	if (!fHandle) {
		string msg = "Can't open \"";
		msg += stlFilename;
		msg += "\". Check that the file name is correct and that you have sufficient privileges to open it.";
		MeshyException problem(msg.c_str());
		throw(problem);
	}

	char* comment = fgets((char*) buf, sizeof(buf), fHandle);
	if (strncmp("solid ",comment,6)) {
		throw(ModelParseException(stlFilename,"ASCII file does not begin with solid keyword"));
	}
	// Comment is remainder of string. Discard for now.

	while (!feof(fHandle)) {
		// This read should return either 'endsolid' or 'facet'
		fscanf(fHandle, "%80s", buf);
		if (strncmp("endsolid",(const char*)buf,8) == 0) {
			break;
		}
		vertexes_t &v = tridata.vertexes;
		bool success = true;
		if (fscanf(fHandle, "%*s %f %f %f", &v.nx, &v.ny, &v.nz) < 3)
			success = false;
		if (fscanf(fHandle, "%*s %*s") < 0)
			success = false;
		if (fscanf(fHandle, "%*s %f %f %f", &v.x1, &v.y1, &v.z1) < 3)
			success = false;
		if (fscanf(fHandle, "%*s %f %f %f", &v.x2, &v.y2, &v.z2) < 3)
			success = false;
		if (fscanf(fHandle, "%*s %f %f %f", &v.x3, &v.y3, &v.z3) < 3)
			success = false;
		if (fscanf(fHandle, "%*s") < 0)
			success = false;
		if (fscanf(fHandle, "%*s") < 0)
			success = false;
		if (!success) {
			stringstream msg;
			msg << "Error reading face " << triangleCount();
			ModelParseException problem(stlFilename, msg.str().c_str());
			Log::info() << problem.what() << endl;
			Log::info() << buf << endl;
			throw(problem);
		}
		Triangle3 triangle(Vector3(v.x1, v.y1, v.z1), Vector3(v.x2, v.y2, v.z2), Vector3(v.x3, v.y3, v.z3));
		bufferTriangle(triangle);
	}
	fclose(fHandle);
	flushBuffer();
	return triangleCount();

}

size_t Meshy::readBinaryStlFile(const char* stlFilename) {
	struct vertexes_t {
		float nx, ny, nz;
		float x1, y1, z1;
		float x2, y2, z2;
		float x3, y3, z3;
		uint16_t attrBytes;
	};

	union {
		struct vertexes_t vertexes;
		uint8_t bytes[sizeof(vertexes_t)];
	} tridata;

	union {
		uint32_t intval;
		uint16_t shortval;
		uint8_t bytes[4];
	} intdata;

	uint8_t buf[512];
	FILE *fHandle = fopen(stlFilename, "rb");
	if (!fHandle) {
		string msg = "Can't open \"";
		msg += stlFilename;
		msg += "\". Check that the file name is correct and that you have sufficient privileges to open it.";
		MeshyException problem(msg.c_str());
		throw(problem);
	}

	// Binary STL file
	if (fread(buf, 1, 80, fHandle) < 80) {
		throw(ModelParseException(stlFilename,"Binary STL file is shorter than comment field"));
	}
	// Read in triangle count
	if (fread(intdata.bytes, 1, 4, fHandle) < 4) {
		throw(ModelParseException(stlFilename,"Cannot read triangle count in binary STL"));
	}
	convertFromLittleEndian32(intdata.bytes);
	uint32_t tricount = intdata.intval;
	int countdown = (int) tricount;
	while (!feof(fHandle) && countdown-- > 0) {
		if (fread(tridata.bytes, 1, 3 * 4 * 4 + 2, fHandle) < 3 * 4 * 4 + 2) {
			Log::info() << __FUNCTION__ << "BREAKING" << endl;
			break;
		}
		for (int i = 0; i < 3 * 4; i++) {
			convertFromLittleEndian32(tridata.bytes + i * 4);
		}
		convertFromLittleEndian16((uint8_t*) & tridata.vertexes.attrBytes);
		vertexes_t &v = tridata.vertexes;
		Vector3 pt1(v.x1, v.y1, v.z1);
		Vector3 pt2(v.x2, v.y2, v.z2);
		Vector3 pt3(v.x3, v.y3, v.z3);
		Triangle3 triangle(pt1, pt2, pt3);
		bufferTriangle(triangle);
	}

	// AGM: Regarding below: wtf? TODO replace this all soon.
	/// Throw removed to continue coding progress. We may not expect all
	/// triangles to load, depending on situation. Needs debugging/revision
	if (this->triangleCount() != tricount) {
		stringstream msg;
		msg << "Warning: triangle count err in \"";
		msg << stlFilename;
		msg << "\".  Expected: ";
		msg << tricount;
		msg << ", Read:";
		msg << triangleCount();
		Log::info() << msg.str();
		//			MeshyException problem(msg.c_str());
		//			throw (problem);
	}


	fclose(fHandle);
	flushBuffer();
	return this->triangleCount();

}

size_t Meshy::readStlFile(const char* stlFilename) {
	// NOTE: for stl legacy read-in reasons, we are using floats here,
	// instead of our own Scalar type
	size_t facecount = 0;

	try {
		facecount = readAsciiStlFile(stlFilename);
	} catch (ModelParseException& parseException) {
		facecount = readBinaryStlFile(stlFilename);
	}
	return facecount;
}

void Meshy::alignToPlate() {
	if (!tequals(limits.zMin, 0, 0.0000001)) {
		translate(Vector3(0, 0, -limits.zMin));
	}
}

void Meshy::translate(const Vector3 &change) {
	flushBuffer();
	vector<Triangle3> oldTriangles(allTriangles.begin(), allTriangles.end());

	allTriangles.clear();

	limits = Limits();

	for (vector<Triangle3>::iterator i = oldTriangles.begin();
		 i != oldTriangles.end(); i++) {
		Vector3 point1 = (*i)[0] + change;
		Vector3 point2 = (*i)[1] + change;
		Vector3 point3 = (*i)[2] + change;

		Triangle3 newTriangle(point1, point2, point3);
		bufferTriangle(newTriangle);
	}
	
	flushBuffer();
}

}


