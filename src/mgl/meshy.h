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



// returns the minimum and maximum z for the 3 vertices of a triangle
void minMaxZ(const BGL::Triangle3d &t, Scalar &min,  Scalar &max );



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
// Exception class
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
// a text file STL
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
			stringstream ss;
			ss << "Can't open \"" << fileName << "\"";
			MeshyMess problem(ss.str().c_str());
			throw (problem);
		}

		// bingo!
		out << scientific;
		out << "solid " << solidName << endl;

	}

	void writeTriangle(const BGL::Triangle3d& t)
	{
		// normalize( (v1-v0) cross (v2 - v0) )
		// y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x

		double n0=0;
		double n1=0;
		double n2=0;

		out << " facet normal " << n0 << " " << n1 << " " << n2 << std::endl;
		out << "  outer loop"<< std::endl;
		out << "    vertex " << t.vertex1.x << " " << t.vertex1.y << " " << t.vertex1.z << std::endl;
		out << "    vertex " << t.vertex2.x << " " << t.vertex2.y << " " << t.vertex2.z << std::endl;
		out << "    vertex " << t.vertex3.x << " " << t.vertex3.y << " " << t.vertex3.z << std::endl;
		out << "  end loop" << std::endl;
		out << " end facet" << std::endl;
	}

	void close()
	{
		out << "end solid " << solidName << endl;
		out.close();
	}

};



class Meshy
{

	mgl::Limits limits;

	std::vector<BGL::Triangle3d>  allTriangles;
	SliceTable sliceTable;
	LayerMeasure zTapeMeasure; // Ze tape measure, for Z

public:


	Meshy(Scalar firstSliceZ, Scalar layerH)
		:zTapeMeasure(firstSliceZ, layerH)
	{
	}

	const std::vector<BGL::Triangle3d> &readAllTriangles() const
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
	void addTriangle(BGL::Triangle3d &t)
	{
		Scalar min;
		Scalar max;
		minMaxZ(t, min, max);

		unsigned int minSliceIndex = this->zTapeMeasure.zToLayerAbove(min);
		unsigned int maxSliceIndex = this->zTapeMeasure.zToLayerAbove(max);
		if (maxSliceIndex - minSliceIndex > 1)
			maxSliceIndex --;

		// cout << "Min max index = [" <<  minSliceIndex << ", "<< maxSliceIndex << "]"<< endl;
		// cout << "Max index =" <<  maxSliceIndex << endl;
		unsigned int currentSliceCount = sliceTable.size();
		if (maxSliceIndex >= currentSliceCount)
		{
			unsigned int newSize = maxSliceIndex+1;
			sliceTable.resize(newSize); // make room for potentially new slices
//			cout << "- new slice count: " << sliceTable.size() << endl;
		}

		allTriangles.push_back(t);

		size_t newTriangleId = allTriangles.size() -1;

		// cout << "adding triangle " << newTriangleId << " to layer " << minSliceIndex  << " to " << maxSliceIndex << endl;
		for (int i= minSliceIndex; i<= maxSliceIndex; i++)
		{
			TriangleIndices &trianglesForSlice = sliceTable[i];
			trianglesForSlice.push_back(newTriangleId);
//			cout << "   !adding triangle " << newTriangleId << " to layer " << i  << " (size = " << trianglesForSlice.size() << ")" << endl;
		}

		limits.grow(t.vertex1);
		limits.grow(t.vertex2);
		limits.grow(t.vertex3);
	}


	void dump(std::ostream &out)
	{
		out << "dumping " << this << endl;
		out << "Nb of triangles: " << allTriangles.size() << endl;
		size_t sliceCount = sliceTable.size();

		out << "triangles per slice: (" << sliceCount << " slices)" << endl;
		for (int i= 0; i< sliceCount; i++)
		{
			TriangleIndices &trianglesForSlice = sliceTable[i];
			//trianglesForSlice.push_back(newTriangleId);
			cout << "  slice " << i << " size: " << trianglesForSlice.size() << endl;
			//cout << "adding triangle " << newTriangleId << " to layer " << i << endl;
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
			const BGL::Triangle3d &t = allTriangles[index];
			out.writeTriangle(t);
		}
		out.close();
		// cout << fileName << " written!"<< endl;
	}

};

/*
class Triangular
{
	Scalar botZ;
	Scalar topZ;
	Scalar midX, midY, midZ;
	Scalar dxTop, dxYTop;
	Scalar dxBot, dyBot;

	bool midFirst;
public:

};

*/


size_t loadMeshyFromStl(mgl::Meshy &meshy, const char* filename);


void pathology( std::vector<Segment> &outlineSegments,
				const Limits& limits,
				double z,
				double tubeSpacing ,
				// double angle,
				std::vector<Segment> &tubes);



#ifdef OMPFF
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

void sliceAndPath(	Meshy &mesh,
					double layerW,
					double tubeSpacing,
					double angle,
					const char* scadFile,
					std::vector< std::vector<Segment> > &allTubes);

} // namespace

#endif
