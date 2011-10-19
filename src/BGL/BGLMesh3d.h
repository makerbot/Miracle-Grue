//
//  BGLMesh3D.h
//  Part of the Belfry Geometry Library
//
//  Created by GM on 10/14/10.
//  Copyright 2010 Belfry Software. All rights reserved.
//


#ifndef BGL_MESH3D_H
#define BGL_MESH3D_H

#include "config.h"
#include "BGLPoint3d.h"
#include "BGLTriangle3d.h"

namespace BGL {

class CompoundRegion;

class Mesh3d {
public:
    Triangles3d triangles;
    Scalar minX, maxX;
    Scalar minY, maxY;
    Scalar minZ, maxZ;

    Mesh3d() : triangles(), minX(9e9), maxX(-9e9), minY(9e9), maxY(-9e9), minZ(9e9), maxZ(-9e9) {}
    Mesh3d(const Mesh3d& x) : triangles(x.triangles), minX(x.minX), maxX(x.maxX), minY(x.minY), maxY(x.maxY), minZ(x.minZ), maxZ(x.maxZ) {}

    int32_t size();
    Point3d centerPoint() const;
    void recalculateBounds();

    void translateToCenterOfPlatform();
    void translate(Scalar dx, Scalar dy, Scalar dz);
    void scale(Scalar sf);
    void scale(Scalar sx, Scalar sy, Scalar sz);
    void rotateX(Scalar rad);
    void rotateY(Scalar rad);
    void rotateZ(Scalar rad);

    int32_t loadFromSTLFile(const char *fileName);
    CompoundRegion& regionForSliceAtZ(Scalar Z, CompoundRegion &outReg);
};

}

#endif

