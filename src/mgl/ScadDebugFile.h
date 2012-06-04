/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/


#ifndef SCAD_TUBEFILE_H_
#define SCAD_TUBEFILE_H_ (1)

#include <iostream>
#include <ostream>
#include <fstream>
#include <sstream>

//#include "meshy.h"
#include "abstractable.h"
#include "obj_limits.h"
#include "mgl.h"


namespace mgl
{

class ScadException : public Exception
{
public:
	ScadException(const char *msg)
	 :Exception(msg)
	{

	}

};


//
// OpenSCAD file generator http://www.openscad.org/
//
class ScadDebugFile
{
	std::ofstream out;
//	double layerH;
//	double layerW;

	std::string filename;

public:
	ScadDebugFile();
	void open(const char* path);

	std::string getScadFileName() const;

	bool isOpened();


	std::ostream &getOut();

	void writeHeader();

	void close();



	void writeOutlines(const Polygons &loops, Scalar z, int slice);

	void writePolygons(const char* moduleName,
			const char* implementation,
			const Polygons &polygons,
			Scalar z, int slice);

	static Scalar segment3(	std::ostream &out,
							const char* indent,
							const char* variableName,
							const std::vector<libthing::LineSegment2> &segments,
							Scalar z,
							Scalar dz);

	Scalar writeSegments3(	const char* name,
							const char* implementation,
							const std::vector<libthing::LineSegment2> &segments,
							Scalar z,
							Scalar dz,
							int slice);


	void writeSegments2(	const char* name,
							const char* implementation,
							const std::vector<libthing::LineSegment2> &segments,
							Scalar z,
							int slice);

	// writes a list of triangles into a polyhedron.
	// It is used to display the triangles involved in a slice (layerIndex).

	void writeTrianglesModule(	const char* name,
								const std::vector<libthing::Triangle3>  &allTriangles,
								const TriangleIndices &trianglesForSlice,
								unsigned int layerIndex);
	/*
	void writeTrianglesModule(const char* name, const Meshy &mesh,
								unsigned int layerIndex)
	{
		const TriangleIndices &trianglesForSlice = mesh.readSliceTable()[layerIndex];
		const std::vector<Triangle3>  &allTriangles = mesh.readAllTriangles();
		writeTrianglesModule(name, allTriangles, trianglesForSlice, layerIndex);
	}
*/

public:

	void writeMinMax(const char*name, const char* implementation, int count);
	~ScadDebugFile();
};

}
#endif
