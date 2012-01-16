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

#include "segment.h"
#include "limits.h"
#include "abstractable.h"


#ifdef OMPFF
#include <omp.h>
#endif

///
/// Meshy-ness: that property that links all these triangles together
///

namespace mgl // serious about triangles
{




class LayerMess : public Except
{
public:
	LayerMess(const char *msg)
	 :Except(msg)
	{

	}

};


// Helper class that gives relates height to layer id
//
// This class assumes that the model's triangles are
// all above 0 (the z of each of the 3 vertices is >= 0.0).
// worse, the layers MUST start at 0. Lazy programmer!
// This is good enough for now, until the class "sees" every triangle
// during loading and recalcs layers on the fly.
//
class LayerMeasure
{
	Scalar firstLayerZ;
	Scalar layerH;

public:
	LayerMeasure(Scalar firstLayerZ, Scalar layerH)
		:firstLayerZ(firstLayerZ), layerH(layerH)
	{
	}

	unsigned int zToLayerAbove(Scalar z) const
	{
		if(z<=0)
		{
			LayerMess("Model with points below the z axis are not supported in this version. Please center your model on the build area");
		}

		if (z < firstLayerZ)
			return 0;

		Scalar tol = 0.00000000000001; // tolerance
		Scalar layer = (z+tol-firstLayerZ) / layerH;
		return ceil(layer);
	}

	Scalar sliceIndexToHeight(unsigned int sliceIndex) const
	{
		return firstLayerZ + sliceIndex * layerH;
	}

	Scalar getLayerH() const
	{
		return layerH;
	}
};


//
// Exception class for meshy problems
//
class MeshyMess : public Except
{
public:
	MeshyMess(const char *msg)
	 :Except(msg)
	{
		fprintf(stderr, "%s", msg);
	}

};

// simple class that writes
// a simple text file STL
class StlWriter
{

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
	void open(const char* fileName, const char *solid="Default")

	{
		solidName = solid;
		out.open(fileName);
		if(!out)
		{
			std::stringstream ss;
			ss << "Can't open \"" << fileName << "\"";
			MeshyMess problem(ss.str().c_str());
			throw (problem);
		}

		// bingo!
		out << std::scientific;
		out << "solid " << solidName << std::endl;

	}

	void writeTriangle(const Triangle3& t)
	{
		// normalize( (v1-v0) cross (v2 - v0) )
		// y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x

		Vector3 n = t.normal();
		out << " facet normal " << n[0] << " " << n[1] << " " << n[2] << std::endl;
		out << "  outer loop"<< std::endl;
		out << "    vertex " << t[0].x << " " << t[0].y << " " << t[0].z << std::endl;
		out << "    vertex " << t[1].x << " " << t[1].y << " " << t[1].z << std::endl;
		out << "    vertex " << t[2].x << " " << t[2].y << " " << t[2].z << std::endl;
		out << "  end loop" << std::endl;
		out << " end facet" << std::endl;
	}

	void close()
	{
		out << "end solid " << solidName << std::endl;
		out.close();
	}

};


// A Mesh class, but sort of messy
// This one has triangles, and a slice table.
class Meshy
{

	mgl::Limits limits;
	std::vector<Triangle3>  allTriangles;
	SliceTable sliceTable;
	LayerMeasure zTapeMeasure; // Ze tape measure, for Z

public:


	Meshy(Scalar firstSliceZ, Scalar layerH)
		:zTapeMeasure(firstSliceZ, layerH)
	{
	}

	const std::vector<Triangle3> &readAllTriangles() const
	{
		return allTriangles;
	}

	const Limits& readLimits() const
	{
		return limits;
	}

	const LayerMeasure& readLayerMeasure()
	{
		return zTapeMeasure;
	}

	const SliceTable &readSliceTable() const
	{
		return sliceTable;
	}


	//
	// Adds a triangle to the global array and for each slice of interest
	//
	void addTriangle(Triangle3 &t)
	{
		Scalar min;
		Scalar max;

		Vector3 a, b, c;
		t.zSort(a,b,c);

		unsigned int minSliceIndex = this->zTapeMeasure.zToLayerAbove(a.z);
		unsigned int maxSliceIndex = this->zTapeMeasure.zToLayerAbove(c.z);
		if (maxSliceIndex - minSliceIndex > 1)
			maxSliceIndex --;

		// cout << "Min max index = [" <<  minSliceIndex << ", "<< maxSliceIndex << "]"<< std::endl;
		// cout << "Max index =" <<  maxSliceIndex << std::endl;
		unsigned int currentSliceCount = sliceTable.size();
		if (maxSliceIndex >= currentSliceCount)
		{
			unsigned int newSize = maxSliceIndex+1;
			sliceTable.resize(newSize); // make room for potentially new slices
//			cout << "- new slice count: " << sliceTable.size() << std::endl;
		}

		allTriangles.push_back(t);

		size_t newTriangleId = allTriangles.size() -1;

		// cout << "adding triangle " << newTriangleId << " to layer " << minSliceIndex  << " to " << maxSliceIndex << std::endl;
		for (int i= minSliceIndex; i<= maxSliceIndex; i++)
		{
			TriangleIndices &trianglesForSlice = sliceTable[i];
			trianglesForSlice.push_back(newTriangleId);
//			cout << "   !adding triangle " << newTriangleId << " to layer " << i  << " (size = " << trianglesForSlice.size() << ")" << std::endl;
		}

		limits.grow(t[0]);
		limits.grow(t[1]);
		limits.grow(t[2]);
	}


	void dump(std::ostream &out)
	{
		out << "dumping " << this << std::endl;
		out << "Nb of triangles: " << allTriangles.size() << std::endl;
		size_t sliceCount = sliceTable.size();

		out << "triangles per slice: (" << sliceCount << " slices)" << std::endl;
		for (int i= 0; i< sliceCount; i++)
		{
			TriangleIndices &trianglesForSlice = sliceTable[i];
			//trianglesForSlice.push_back(newTriangleId);
			out << "  slice " << i << " size: " << trianglesForSlice.size() << std::endl;
			//cout << "adding triangle " << newTriangleId << " to layer " << i << std::endl;
		}
	}


public:


	void writeStlFileForLayer(unsigned int layerIndex, const char* fileName) const
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

};




size_t loadMeshyFromStl(mgl::Meshy &meshy, const char* filename);

//
// Pathology: the operation that generate tool paths from triangle cuts
//
void pathology( std::vector<std::vector<Segment> > &outlineSegments,
				const Limits& limits,
				double z,
				double tubeSpacing ,
				// double angle,
				std::vector<Segment> &tubes);


// compile time enabled
// Multi threaded stuff
//
#ifdef OMPFF
// a lock class for multithreaded sync
class OmpGuard {
public:
    //Acquire the lock and store a pointer to it
	OmpGuard (omp_lock_t &lock)
	:lock_ (&lock)
	{
		acquire();
	}
    void acquire ()
    {
    	omp_set_lock (lock_);
    }

    void release ()
    {
    	omp_unset_lock (lock_);
    }

    ~OmpGuard ()
    {
    	release();
    }

private:
    omp_lock_t *lock_;  // pointer to our lock

};
#endif

//
// The Slice is a series of tubes
//
// tubes are plastic extrusions
class TubesInSlice
{
public:
	TubesInSlice(Scalar z)
		:z(z)
	{
	}

	Scalar z;
	std::vector<Segment> infill;
	std::vector< std::vector<Segment> > outlines;
};

// little function that does everything...
// loads an stl, makes slices, returns extrusions
// and an OpenSCAD file for visualization
void sliceAndPath(	Meshy &mesh,
					double layerW,
					double tubeSpacing,
					double angle,
					const char* scadFile,
					std::vector< TubesInSlice > &allTubes);

} // namespace

#endif
