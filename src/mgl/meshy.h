/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

 */


#ifndef MESHY_H_
#define MESHY_H_

#include <iomanip>
#include <set>
#include <fstream>
#include <list>

#ifdef OMPFF
#include <omp.h>
#endif

#include "segment.h"
#include "obj_limits.h"
#include "abstractable.h"
#include "mgl.h"



///
/// Meshy-ness: that property that links all these triangles together
///

namespace mgl // serious about triangles
{
//
// Exception class for meshy problems
//

class MeshyException : public Exception {
public:

	MeshyException(const char *msg)
	: Exception(msg) {

	}

};

// simple class that writes
// a simple text file STL

class StlWriter {
	//solid Default
	//  facet normal 1.435159e-01 2.351864e-02 9.893685e-01
	//    outer loop
	//      vertex -7.388980e-02 -2.377973e+01 6.062650e+01
	//      vertex -1.193778e-01 -2.400027e+01 6.063834e+01
	//      vertex -4.402440e-06 -2.490700e+01 6.064258e+01
	//    endloop
	//  endfacet
	//endsolid Default

	std::ofstream out;
	std::string solidName;

public:

	void open(const char* fileName, const char *solid = "Default");

	void writeTriangle(const libthing::Triangle3& t);

	void close();

};

/**
 *
 * A Mesh class
 */
class Meshy {
	mgl::Limits limits; /// Bounding box for the model
	std::vector<libthing::Triangle3> allTriangles; /// every triangle in the model.
	/// for each slice, a list of indicies, each index is a lookup into vector
	// allTriangles
	SliceTable sliceTable;
	
	std::list<libthing::Triangle3> bufferedTriangles; /// list of triangles that
	/// have been parsed from the file, but not yet analyzed and placed into
	/// allTriangles
	//bufferTriangles

	// Ze tape measure, for Z
	LayerMeasure zTapeMeasure;

public:


	/// requires firstLayerSlice height, and general layer height
	Meshy(Scalar firstSliceZ, Scalar layerH);
	const std::vector<libthing::Triangle3>& readAllTriangles() const;
	const Limits& readLimits() const;
	const LayerMeasure& readLayerMeasure() const;
	const SliceTable &readSliceTable() const;

	//
	// Adds a triangle to the global array and for each slice of interest
	//
	void bufferTriangle(libthing::Triangle3 t);
	void addTriangle(libthing::Triangle3 &t);
	void updateSlicesTriangle(size_t newTriangleId);


	void dump(std::ostream &out);

public:

	size_t triangleCount();
	void writeStlFile(const char* fileName) const;
	void writeStlFileForLayer(unsigned int layerIndex, const char* fileName) const;

	size_t readStlFile(const char* stlFilename);
	void analyzeTriangles();

	void alignToPlate();
	void translate(const libthing::Vector3 &change);
};


//void writeMeshyToStl(mgl::Meshy &meshy, const char* filename);

size_t readStlFile(mgl::Meshy &meshy, const char* filename);



// compile time enabled
// Multi threaded stuff
//
#ifdef OMPFF
// a lock class for multithreaded sync

class OmpGuard {
public:
	//Acquire the lock and store a pointer to it

	OmpGuard(omp_lock_t &lock)
	: lock_(&lock) {
		acquire();
	}

	void acquire() {
		omp_set_lock(lock_);
	}

	void release() {
		omp_unset_lock(lock_);
	}

	~OmpGuard() {
		release();
	}

private:
	omp_lock_t *lock_; // pointer to our lock

};
#endif

} // namespace

#endif
