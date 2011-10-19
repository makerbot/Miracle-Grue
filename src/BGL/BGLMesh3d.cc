//
//  BGLMesh3D.cc
//  Part of the Belfry Geometry Library
//
//  Created by GM on 10/14/10.
//  Copyright 2010 Belfry Software. All rights reserved.
//

#include "BGLMesh3d.h"
#include "BGLPoint3d.h"
#include "BGLLine.h"
#include "BGLPath.h"
#include "BGLCompoundRegion.h"
#include "BGLTriangle3d.h"
#include <stdio.h>
#include <strings.h>

namespace BGL {


#include <sys/param.h>

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



int32_t Mesh3d::size()
{
    return triangles.size();
}



Point3d Mesh3d::centerPoint() const
{
    Scalar mx = (minX+maxX)/2.0f;
    Scalar my = (minY+maxY)/2.0f;
    Scalar mz = (minZ+maxZ)/2.0f;
    return Point3d(mx, my, mz);
}



void Mesh3d::recalculateBounds()
{
    Triangles3d::iterator it = triangles.begin();
    minX = minY = minZ = 9e9;
    maxX = maxY = maxZ = -9e9;
    for ( ; it != triangles.end(); it++) {
	Point3d &pt1 = it->vertex1;
	Point3d &pt2 = it->vertex2;
	Point3d &pt3 = it->vertex3;

	if (pt1.x < minX) minX = pt1.x;
	if (pt1.y < minY) minY = pt1.y;
	if (pt1.z < minZ) minZ = pt1.z;
	if (pt2.x < minX) minX = pt2.x;
	if (pt2.y < minY) minY = pt2.y;
	if (pt2.z < minZ) minZ = pt2.z;
	if (pt3.x < minX) minX = pt3.x;
	if (pt3.y < minY) minY = pt3.y;
	if (pt3.z < minZ) minZ = pt3.z;

	if (pt1.x > maxX) maxX = pt1.x;
	if (pt1.y > maxY) maxY = pt1.y;
	if (pt1.z > maxZ) maxZ = pt1.z;
	if (pt2.x > maxX) maxX = pt2.x;
	if (pt2.y > maxY) maxY = pt2.y;
	if (pt2.z > maxZ) maxZ = pt2.z;
	if (pt3.x > maxX) maxX = pt3.x;
	if (pt3.y > maxY) maxY = pt3.y;
	if (pt3.z > maxZ) maxZ = pt3.z;
    }
    if (minX == 9e9 || minY == 9e9 || minZ == 9e9) {
        minX = minY = minZ = maxX = maxY = maxZ = 0;
    }
}



void Mesh3d::translate(Scalar dx, Scalar dy, Scalar dz)
{
    Triangles3d::iterator it = triangles.begin();
    for ( ; it != triangles.end(); it++) {
        it->translate(dx,dy,dz);
    }
    minX += dx;
    minY += dy;
    minZ += dz;
    maxX += dx;
    maxY += dy;
    maxZ += dz;
}



void Mesh3d::translateToCenterOfPlatform()
{
    Scalar dx = -(maxX + minX) / 2.0;
    Scalar dy = -(maxY + minY) / 2.0;
    Scalar dz = -minZ;
    translate(dx,dy,dz);
}



void Mesh3d::scale(Scalar sx, Scalar sy, Scalar sz)
{
    Triangles3d::iterator it = triangles.begin();
    for ( ; it != triangles.end(); it++) {
        it->scale(Point3d(sx,sy,sz));
    }
    minX *= sx;
    minY *= sy;
    minZ *= sz;
    maxX *= sx;
    maxY *= sy;
    maxZ *= sz;
}



void Mesh3d::scale(Scalar sf)
{
    scale(sf, sf, sf);
}



void Mesh3d::rotateX(Scalar rad)
{
    Triangles3d::iterator it = triangles.begin();
    for ( ; it != triangles.end(); it++) {
        it->rotateX(centerPoint(), rad);
    }
    recalculateBounds();
}




void Mesh3d::rotateY(Scalar rad)
{
    Triangles3d::iterator it = triangles.begin();
    for ( ; it != triangles.end(); it++) {
        it->rotateY(centerPoint(), rad);
    }
    recalculateBounds();
}




void Mesh3d::rotateZ(Scalar rad)
{
    Triangles3d::iterator it = triangles.begin();
    for ( ; it != triangles.end(); it++) {
        it->rotateZ(centerPoint(), rad);
    }
    recalculateBounds();
}




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


int32_t Mesh3d::loadFromSTLFile(const char *fileName)
{
    struct vertexes_t {
        Scalar nx, ny, nz;
        Scalar x1, y1, z1;
        Scalar x2, y2, z2;
        Scalar x3, y3, z3;
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

    uint32_t facecount = 0;
    
    uint8_t buf[512];
    FILE *f = fopen(fileName, "rb");
    if (!f) {
	fprintf(stderr, "STL read failed to open\n");
	return 0;
    }
    
    if (fread(buf, 1, 5, f) < 5) {
	fprintf(stderr, "STL read failed read\n");
	return 0;
    }
    bool isBinary = true;
    if (!strncasecmp((const char*)buf, "solid", 5)) {
        isBinary = false;
    }
    if (isBinary) {
	// Binary STL file
	// Skip remainder of 80 character comment field
	if (fread(buf, 1, 75, f) < 75) {
	    fprintf(stderr, "STL read failed header read\n");
	    return 0;
	}
	// Read in triangle count
	if (fread(intdata.bytes, 1, 4, f) < 4) {
	    fprintf(stderr, "STL read failed face count read\n");
	    return 0;
	}
	convertFromLittleEndian32(intdata.bytes);
	uint32_t tricount = intdata.intval;
	while (!feof(f) && tricount-->0) {
	    if (fread(tridata.bytes, 1, 3*4*4+2, f) < 3*4*4+2) {
		break;
	    }
            for (int i = 0; i < 3*4; i++) {
		convertFromLittleEndian32(tridata.bytes+i*4);
            }
	    convertFromLittleEndian16((uint8_t*)&tridata.vertexes.attrBytes);

	    vertexes_t &v = tridata.vertexes;
            Point3d pt1(v.x1, v.y1, v.z1);
            Point3d pt2(v.x2, v.y2, v.z2);
            Point3d pt3(v.x3, v.y3, v.z3);
            triangles.push_back(Triangle3d(pt1, pt2, pt3));
	    facecount++;
        }
	fclose(f);
    } else {
        // ASCII STL file
	// Gobble remainder of solid name line.
	fgets((char*)buf, sizeof(buf), f);
	while (!feof(f)) {
	    fscanf(f, "%80s", buf);
	    if (!strcasecmp((char*)buf, "endsolid")) {
	        break;
	    }
	    vertexes_t &v = tridata.vertexes;
	    fscanf(f, "%*s %f %f %f", &v.nx, &v.ny, &v.nz);
	    fscanf(f, "%*s %*s");
	    fscanf(f, "%*s %f %f %f", &v.x1, &v.y1, &v.z1);
	    fscanf(f, "%*s %f %f %f", &v.x2, &v.y2, &v.z2);
	    fscanf(f, "%*s %f %f %f", &v.x3, &v.y3, &v.z3);
	    fscanf(f, "%*s");
	    fscanf(f, "%*s");

            Point3d pt1(v.x1, v.y1, v.z1);
            Point3d pt2(v.x2, v.y2, v.z2);
            Point3d pt3(v.x3, v.y3, v.z3);
            triangles.push_back(Triangle3d(pt1, pt2, pt3));
	    facecount++;
	}
	fclose(f);
    }
    recalculateBounds();
    return facecount;
}



CompoundRegion& Mesh3d::regionForSliceAtZ(Scalar Z, CompoundRegion &outReg)
{
    Lines lines;
    Triangles3d::iterator trit;
    for (trit = triangles.begin(); trit != triangles.end(); trit++) {
	Line ln;
	if (trit->sliceAtZ(Z, ln)) {
	    lines.push_back(ln);
        }
    }

    Paths paths;
    Path::assemblePathsFromSegments(lines, paths);
    Paths repairedPaths;
    Path::repairUnclosedPaths(paths, repairedPaths);
    CompoundRegion::assembleCompoundRegionFrom(repairedPaths, outReg);
    outReg.zLevel = Z;
    return outReg;
}


}


