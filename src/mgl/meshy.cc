
#include "meshy.h"
#include "mgl.h"

#include<iostream>
#include<stdint.h>
#include<cstring>

using namespace mgl;
using namespace std;



#ifdef __BYTE_ORDER
# if __BYTE_ORDER == __LITTLE_ENDIAN
#  define I_AM_LITTLE_ENDIAN
# else
#  if __BYTE_ORDER == __BIG_ENDIAN
#   define I_AM_BIG_ENDIAN
#  else
#error "Unknown byte order!"
#  endif
# endif
#endif /* __BYTE_ORDER */

#ifdef I_AM_BIG_ENDIAN
static inline void convertFromLittleEndian32(uint8_t* bytes)
{
    uint8_t tmp = bytes[0];
    bytes[0] = bytes[3];
    bytes[3] = tmp;
    tmp = bytes[1];
    bytes[1] = bytes[2];
    bytes[2] = tmp;
}
static inline void convertFromLittleEndian16(uint8_t* bytes)
{
    uint8_t tmp = bytes[0];
    bytes[0] = bytes[1];
    bytes[1] = tmp;
}
#else
static inline void convertFromLittleEndian32(uint8_t* bytes)
{
}

static inline void convertFromLittleEndian16(uint8_t* bytes)
{
}
#endif



	/// requires firstLayerSlice height, and general layer height
Meshy::Meshy(Scalar firstSliceZ, Scalar layerH)
		:zTapeMeasure(firstSliceZ, layerH)
	{ 	}

const std::vector<Triangle3>& Meshy::readAllTriangles() const
{
	return allTriangles;
}

const Limits& Meshy::readLimits() const
{
	return limits;
}

const LayerMeasure& Meshy::readLayerMeasure() const
{
	return zTapeMeasure;
}

const SliceTable& Meshy::readSliceTable() const
{
	return sliceTable;
}


//
// Adds a triangle to the global array and for each slice of interest
//
void Meshy::addTriangle(Triangle3 &t)
{

	Vector3 a, b, c;
	t.zSort(a,b,c);

	unsigned int minSliceIndex = this->zTapeMeasure.zToLayerAbove(a.z);
	if(minSliceIndex > 0)
		minSliceIndex --;

	unsigned int maxSliceIndex = this->zTapeMeasure.zToLayerAbove(c.z);
	if (maxSliceIndex - minSliceIndex > 1)
		maxSliceIndex --;

//		std::cout << "Min max index = [" <<  minSliceIndex << ", "<< maxSliceIndex << "]"<< std::endl;
//		std::cout << "Max index =" <<  maxSliceIndex << std::endl;
	unsigned int currentSliceCount = sliceTable.size();
	if (maxSliceIndex >= currentSliceCount)
	{
		unsigned int newSize = maxSliceIndex+1;
		sliceTable.resize(newSize); // make room for potentially new slices
//			std::cout << "- new slice count: " << sliceTable.size() << std::endl;
	}

	allTriangles.push_back(t);

	size_t newTriangleId = allTriangles.size() -1;

//		 std::cout << "adding triangle " << newTriangleId << " to layer " << minSliceIndex  << " to " << maxSliceIndex << std::endl;
	for (size_t i= minSliceIndex; i<= maxSliceIndex; i++)
	{
		TriangleIndices &trianglesForSlice = sliceTable[i];
		trianglesForSlice.push_back(newTriangleId);
//			std::cout << "   !adding triangle " << newTriangleId << " to layer " << i  << " (size = " << trianglesForSlice.size() << ")" << std::endl;
	}

	limits.grow(t[0]);
	limits.grow(t[1]);
	limits.grow(t[2]);


}


void Meshy::dump(std::ostream &out)
{
	out << "dumping " << this << std::endl;
	out << "Nb of triangles: " << allTriangles.size() << std::endl;
	size_t sliceCount = sliceTable.size();

	out << "triangles per slice: (" << sliceCount << " slices)" << std::endl;
	for (size_t i= 0; i< sliceCount; i++)
	{
		TriangleIndices &trianglesForSlice = sliceTable[i];
		//trianglesForSlice.push_back(newTriangleId);
		out << "  slice " << i << " size: " << trianglesForSlice.size() << std::endl;
		//cout << "adding triangle " << newTriangleId << " to layer " << i << std::endl;
	}
}



size_t Meshy::triangleCount() {
	return allTriangles.size();
	std::cout << "all triangle count" << allTriangles.size();
}

void Meshy::writeStlFile(const char* fileName) const
{
	StlWriter out;
	out.open(fileName);
	size_t triCount = allTriangles.size();
	for (size_t i= 0; i < triCount; i++)
	{
		const Triangle3 &t = allTriangles[i];
		out.writeTriangle(t);
	}
	out.close();
	// cout << fileName << " written!"<< std::endl;

}

void Meshy::writeStlFileForLayer(unsigned int layerIndex, const char* fileName) const
{

	StlWriter out;
	out.open(fileName);

	const TriangleIndices &trianglesForSlice = sliceTable[layerIndex];
	for(std::vector<index_t>::const_iterator i = trianglesForSlice.begin(); i!= trianglesForSlice.end(); i++)
	{
		index_t index = *i;
		const Triangle3 &t = allTriangles[index];
		out.writeTriangle(t);
	}
	out.close();
	// cout << fileName << " written!"<< std::endl;
}

/// Loads an STL file into a mesh object, from a binary or ASCII stl file.
///
/// @param stlFilename target file to load into the specified mesh
///
/// @returns count of triangles loaded into this mesh by this call
size_t Meshy::readStlFile(const char* stlFilename)
{
	// NOTE: for stl legacy read-in reasons, we are using floats here,
	// instead of our own Scalar type
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

	union
	{
		uint32_t intval;
		uint16_t shortval;
		uint8_t bytes[4];
	} intdata;

	size_t facecount = 0;

	uint8_t buf[512];
	FILE *fHandle = fopen(stlFilename, "rb");
	if (!fHandle)
	{
		string msg = "Can't open \"";
		msg += stlFilename;
		msg += "\". Check that the file name is correct and that you have sufficient privileges to open it.";
		MeshyException problem(msg.c_str());
		throw (problem);
	}

	if (fread(buf, 1, 5, fHandle) < 5) {
		string msg = "\"";
		msg += stlFilename;
		msg += "\" is empty!";
		MeshyException problem(msg.c_str());
		throw (problem);
	}
	bool isBinary = true;
	if (!strncasecmp((const char*) buf, "solid", 5)) {
		isBinary = false;
	}
	if (isBinary) {
		// Binary STL file
		// Skip remainder of 80 character comment field
		if (fread(buf, 1, 75, fHandle) < 75) {
			string msg = "\"";
			msg += stlFilename;
			msg += "\" is not a valid stl file";
			MeshyException problem(msg.c_str());
			throw (problem);
		}
		// Read in triangle count
		if (fread(intdata.bytes, 1, 4, fHandle) < 4) {
			string msg = "\"";
			msg += stlFilename;
			msg += "\" is not a valid stl file";
			MeshyException problem(msg.c_str());
			throw (problem);
		}
		convertFromLittleEndian32(intdata.bytes);
		uint32_t tricount = intdata.intval;
		int countdown = (int)tricount;
		while (!feof(fHandle) && countdown-- > 0) {
			if (fread(tridata.bytes, 1, 3 * 4 * 4 + 2, fHandle) < 3 * 4 * 4 + 2) {
				std::cout << __FUNCTION__ << "BREAKING" << endl;
				break;
			}
			for (int i = 0; i < 3 * 4; i++) {
				convertFromLittleEndian32(tridata.bytes + i * 4);
			}
			convertFromLittleEndian16((uint8_t*) &tridata.vertexes.attrBytes);

			vertexes_t &v = tridata.vertexes;
			Vector3 pt1(v.x1, v.y1, v.z1);
			Vector3 pt2(v.x2, v.y2, v.z2);
			Vector3 pt3(v.x3, v.y3, v.z3);

			Triangle3 triangle(pt1, pt2, pt3);
			this->addTriangle(triangle);

			facecount++;
		}

		/// Throw removed to continue coding progress. We may not expect all
		/// triangles to load, depending on situation. Needs debugging/revision
		if(this->triangleCount() != tricount) {
			string msg = "Warning: triangle count err in \"";
			msg += stlFilename;
			msg += "\".  Expected: ";
			msg += stringify((size_t)tricount);
			msg += ", Read:";
			msg += stringify(this->triangleCount());
			msg += ", faced:";
			msg += stringify(facecount);
			std::cout << msg;
//			MeshyException problem(msg.c_str());
//			throw (problem);
		}


	} else {
		// ASCII STL file
		// Gobble remainder of solid name line.
		fgets((char*) buf, sizeof(buf), fHandle);
		while (!feof(fHandle)) {
			fscanf(fHandle, "%80s", buf);
			if (!strcasecmp((char*) buf, "endsolid")) {
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
			if (fscanf(fHandle, "%*s")< 0)
				success = false;
			if (fscanf(fHandle, "%*s")< 0)
				success = false;
			if(!success)
			{
				stringstream msg;
				msg << "Error reading face " << facecount << " in file \"" << stlFilename << "\"";
				MeshyException problem(msg.str().c_str());
				cout << msg.str().c_str()<< endl;
				cout << buf << endl;
				throw(problem);
			}
			Triangle3 triangle(Vector3(v.x1, v.y1, v.z1),	Vector3(v.x2, v.y2, v.z2),	Vector3(v.x3, v.y3, v.z3));
			this->addTriangle(triangle);

			facecount++;
		}
	}
	fclose(fHandle);
	return this->triangleCount();

}
