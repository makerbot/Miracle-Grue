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

#include "BGL/config.h"
#include "BGL/BGLTriangle3d.h"
#include "BGL/BGLMesh3d.h"
#include "limits.h"
///
/// Meshyness is what links all these triangles together
///

namespace mgl
{

typedef unsigned int index_t;
typedef std::vector<index_t> TriangleIndices;
typedef std::vector<TriangleIndices> TrianglesInSlices;


// returns the minimum and maximum z for the 3 vertices of a triangle
void minMaxZ(const BGL::Triangle3d &t, Scalar &min,  Scalar &max )
{
	// find minimum
	min = t.vertex1.z;
	if(t.vertex2.z < min)
		min = t.vertex2.z ;

	if(t.vertex3.z < min)
		min = t.vertex3.z ;

	// find maximum
	max = t.vertex1.z;
	if( t.vertex2.z > max)
		max = t.vertex2.z;

	if (t.vertex3.z > max)
		max = t.vertex3.z;
}




class MeshyMess
{

public:
	std::string error;
	MeshyMess(const char *msg)
	 :error(msg)
	{

	}

};


class Meshy
{

	Scalar sliceHeight;
	mgl::Limits limits;

	std::vector<BGL::Triangle3d>  allTriangles;
	TrianglesInSlices sliceTable;
public:

	const std::vector<BGL::Triangle3d> &readAllTriangles()
	{
		return allTriangles;
	}

	const Limits& readLimits() const
	{
		return limits;
	}

	Scalar readSliceHeight() const
	{
		return sliceHeight;
	}

	const TrianglesInSlices &readSliceTable() const
	{
		return sliceTable;
	}

	Meshy(Scalar sliceHeight)
		:sliceHeight(sliceHeight)
	{
	}

	//
	// Adds a triangle to the global array and for each slice of interest
	//
	void addTriangle(BGL::Triangle3d &t)
	{
		Scalar min;
		Scalar max;
		minMaxZ(t, min, max);

		Scalar mini = (min+0.5) / sliceHeight;
		Scalar maxi =  (max+0.5) / sliceHeight;
		int minSliceIndex = floor( mini);
		int maxSliceIndex = ceil(maxi);

//		cout << "------" << endl;
//		cout << "t " << t.vertex1 << ", " << t.vertex2 << ", " << t.vertex3 << endl;
//		cout << "Minimum   =" << min << endl;
//		cout << "Maximum   =" << max << endl;
//		cout << "mini      =" << mini << endl;
//		cout << "maxi      =" << maxi << endl;
//		cout << "Min index =" <<  minSliceIndex << endl;
//		cout << "Max index =" <<  maxSliceIndex << endl;


		if (maxSliceIndex > sliceTable.size() )
		{
			sliceTable.resize(maxSliceIndex); // make room for potentially new slices
			// cout << "new slices: " << sliceTable.size() << endl;
		}

		allTriangles.push_back(t);

		size_t newTriangleId = allTriangles.size() -1;

		// cout << "adding triangle " << newTriangleId << " to layer " << minSliceIndex  << " to " << maxSliceIndex << endl;
		for (int i= minSliceIndex; i< maxSliceIndex; i++)
		{
			TriangleIndices &trianglesForSlice = sliceTable[i];
			trianglesForSlice.push_back(newTriangleId);
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


	void writeTriangle(std::ostream &out, const BGL::Triangle3d& t) const
	{
		//  normalize( (v1-v0) cross (v2 - v0) )

		// normalize
		// y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x

		double n0=0;
		double n1=0;
		double n2=0;

		out << " facet normal " << n0 << " " << n1 << " " << n2 << endl;
		out << "  outer loop"<< endl;
		out << "    vertex " << t.vertex1.x << " " << t.vertex1.y << " " << t.vertex1.z << endl;
		out << "    vertex " << t.vertex2.x << " " << t.vertex2.y << " " << t.vertex2.z << endl;
		out << "    vertex " << t.vertex3.x << " " << t.vertex3.y << " " << t.vertex3.z << endl;
		out << "  end loop" << endl;
		out << " end facet" << endl;
	}

public:
	void writeStlFileForLayer(unsigned int layerIndex, const char* fileName) const
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

		ofstream out(fileName);
		if(!out) {
			stringstream ss;
			ss << "Can't open \"" << fileName << "\"";
			MeshyMess problem(ss.str().c_str());
			throw (problem);
		}
		stringstream ss;
		ss << setfill ('0') << setw(10);
		ss <<  "slice_" << layerIndex;
		string solidName = ss.str();
		// bingo!
		out << "solid Slice_" << layerIndex << endl;
		Scalar n0, n1, n2;
		out << scientific;
		const TriangleIndices &trianglesForSlice = sliceTable[layerIndex];
		for(std::vector<index_t>::const_iterator i = trianglesForSlice.begin(); i!= trianglesForSlice.end(); i++)
		{
			index_t index = *i;
			const BGL::Triangle3d &t = allTriangles[index];
			writeTriangle(out, t);
		}

		out << "end solid " << solidName;
		out.close();
		// cout << fileName << " written!"<< endl;
	}

};


size_t LoadMeshyFromStl(Meshy &meshy, const char* filename)
{

	BGL::Mesh3d mesh;
	int count = mesh.loadFromSTLFile(filename);
	for (BGL::Triangles3d::iterator i= mesh.triangles.begin(); i != mesh.triangles.end(); i++)
	{
		BGL::Triangle3d &t = *i;
		meshy.addTriangle(t);
	}
	return (size_t) count;
}

}

#endif
