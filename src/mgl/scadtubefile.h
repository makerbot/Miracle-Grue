/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/


#ifndef SCAD_TUBEFILE_H_
#define SCAD_TUBEFILE_H_

#include <iostream>
#include <ostream>
#include <sstream>

#include "meshy.h"
#include "segment.h"


namespace mgl
{

class ScadMess : public Except
{
public:
	ScadMess(const char *msg)
	 :Except(msg)
	{

	}

};

//
// OpenSCAD file generator http://www.openscad.org/
//
class ScadTubeFile
{
	std::ofstream out;
public:
	ScadTubeFile(const char* filename, double layerH, double layerW)
	 :out(filename, std::ios::out)
	{
		if(!out)
		{
			stringstream ss;
			ss << "Can't open \"" << filename << "\"";
			cout << "ERROR: " << ss.str() << endl;
			ScadMess problem(ss.str().c_str());
			throw (problem);
		}

	    //out.setf(ios_base::floatfield, ios::floatfield);
		//out.precision(18);
	    out.setf(ios::fixed);


	    out << "// use min and max to see individual layers " << endl;
	    out << "min = 0;" << endl;
	    out << "max = 0;" << endl;
	    out << "triangles(min,max);" << endl;
	    out << "outlines(min,max);" << endl;
	    out << "extrusions(min,max);" << endl;
	    out << endl;
	    out << "stl_color = [0,1,0, 0.025];" << endl;
		out << "module out_line(x1, y1, z1, x2, y2, z2)" << endl;
		out << "{" << endl;
		out << "    tube(x1, y1, z1, x2, y2, z2, diameter1=0.4, diameter2=0, faces=4, thickness_over_width=1);" << endl;
		out << "}" << endl;

		out  << endl;
		out << "module extrusion(x1, y1, z1, x2, y2, z2)" << endl;
		out << "{" << endl;
		out << "    d = " << layerH << ";" << endl;
		out << "    f = 6;" << endl;
		out << "    t =  "  << layerH / layerW << ";"<< endl;
		out << "    corner(x1,y1,z1, diameter=d, faces=f, thickness_over_width =t );" << endl;
		out << "    tube(x1, y1, z1, x2, y2, z2, diameter1=d, diameter2=d, faces=f, thickness_over_width=t);" << endl;
		out << "}" << endl;

		out << "module corner(x, y, z, diameter, faces, thickness_over_width ){" << endl;
		out << "	translate([x, y, z])  scale([1,1,thickness_over_width]) sphere( r = diameter/2, $fn = faces );" << endl;
		out << "}" << endl;
		out  << endl;


	    out << "module tube(x1, y1, z1, x2, y2, z2, diameter1, diameter2, faces, thickness_over_width)" << endl;
	    out << "{" << endl;
	    out << "	length = sqrt( pow(x1 - x2, 2) + pow(y1 - y2, 2) + pow(z1 - z2, 2) );" << endl;
	    out << "	alpha = ( (y2 - y1 != 0) ? atan( (z2 - z1) / (y2 - y1) ) : 0 );" << endl;
	    out << " 	beta = 90 - ( (x2 - x1 != 0) ? atan( (z2 - z1) / (x2 - x1) ) :  0 );" << endl;
	    out << "	gamma =  ( (x2 - x1 != 0) ? atan( (y2 - y1) / (x2 - x1) ) : ( (y2 - y1 >= 0) ? 90 : -90 ) ) + ( (x2 - x1 >= 0) ? 0 : -180 );" << endl;
	    out << "	// echo(Length = length, Alpha = alpha, Beta = beta, Gamma = gamma);	" << endl;
	    out << "	translate([x1, y1, z1])" << endl;
	    out << "	rotate([ 0, beta, gamma])" << endl;
	    out << "		scale([thickness_over_width,1,1])" << endl;
	    out << "			rotate([0,0,90]) cylinder(h = length, r1 = diameter1/2, r2 = diameter2/2, center = false, $fn = faces );" << endl;
	    out << "}" << endl;

	}

//	void write(const char *str)
//	{
//		out << str;
//	}

	void writeStlModule(const char* moduleName, const char *stlName,  int slice)
	{
		out << endl;
		out << "module " << moduleName << slice << "()" << endl;
		out << "{" << endl;
		out << "    color(stl_color)import_stl(\"" << stlName<< slice << ".stl\");" << endl;
		out << "}" << endl;

	}

	void writeExtrusionsModule(const char* name, const std::vector<mgl::Segment> &segments, int slice, Scalar z)
	{
		writeTubesModule(name, "extrusion", segments, slice, z);
	}

	void writeOutlinesModule(const char* name, const std::vector<std::vector<Segment> > &loops, int slice, Scalar z)
	{
		writeMultiTubesModule(name, "out_line", loops, slice, z);
	}

	void writeTrianglesModule(const char* name, const Meshy &mesh, unsigned int layerIndex) //const std::vector<BGL::Triangle3d>  &allTriangles, const TriangleIndices &trianglesForSlice)
	{
		stringstream ss;
		ss.setf(ios::fixed);
		const TriangleIndices &trianglesForSlice = mesh.readSliceTable()[layerIndex];
		const std::vector<BGL::Triangle3d>  &allTriangles = mesh.readAllTriangles();

		ss << "module " << name << layerIndex << "(col=[1,0,0,1])" << endl;
		// , [0,2,1], [3,0,4], [1,2,5], [0,5,4], [0,1,5],  [5,2,4], [4,2,3],
		ss << "{" << endl;
		ss << "    color(col) polyhedron ( points = [";

		// ss << scientific;
		ss << dec; // set decimal format for floating point numbers

		for(int i=0; i< trianglesForSlice.size(); i++ )
		{
			//index_t index = *i;
			index_t index = trianglesForSlice[i];
			const BGL::Triangle3d &t = allTriangles[index];
			ss << "    [" << t.vertex1.x << ", " << t.vertex1.y << ", " << t.vertex1.z << "], ";
			ss <<     "[" << t.vertex2.x << ", " << t.vertex2.y << ", " << t.vertex2.z << "], ";
			ss <<     "[" << t.vertex3.x << ", " << t.vertex3.y << ", " << t.vertex3.z << "], // tri " << i << endl;
		}

		ss << "]," << endl;
		ss << "triangles = [" ;

		for (int i=0; i < trianglesForSlice.size(); i++)
		{
			int tri = i * 3;
			ss << "    [" << tri << ", " << tri+1 << ", " << tri + 2 << "], " << endl;
		}

		ss << "]);" << endl;
		ss << "}" << endl;
		ss << endl;
		out << ss.str();
	}

private:

	void writeTubesModule(const char* name, const char* tubeType,const std::vector<Segment> &segments, int slice, Scalar z)
	{

		// tube(x,y,z,  x,y,z, d,f,t);
		out << endl;
		out << "module " << name << slice << "()"<< endl;
		out << "{" << endl;
		for(int j=0; j<segments.size(); j++)
		{
			const Segment &segment = segments[j];
			out << "	"<< tubeType<< "(" << segment.a.x << ", " << segment.a.y << ", " << z << ", ";
			out << 				 segment.b.x << ", " << segment.b.y << ", " << z << ");"<<endl;
		}

		out << "}" << endl;
	}


	void writeMultiTubesModule(const char* name, const char* tubeType,const std::vector<std::vector<Segment> > &loops, int slice, Scalar z)
	{

		// tube(x,y,z,  x,y,z, d,f,t);
		out << endl;
		out << "module " << name << slice << "()"<< endl;
		out << "{" << endl;

		unsigned int loopCount = loops.size();

		for(int i=0; i<loopCount; i++)
		{
			out << "// loop " << i << endl;
			const std::vector<Segment> &segments = loops[i];

			unsigned int segmentCount = segments.size();

			Scalar green = 0.5 + 0.5 * (i+1 / (loopCount )) ;
			for(int j=0; j<segmentCount; j++)
			{
				Scalar blue = 0.5 + 0.5 * (i+1 / (segmentCount )) ;
				const Segment &segment = segments[j];
				out << "	"<< "color([0," << green << ", "<< blue <<", 1])" << endl;
				out << "	"<< tubeType<< "(" << segment.a.x << ", " << segment.a.y << ", " << z << ", ";
				out << 		 segment.b.x << ", " << segment.b.y << ", " << z << ");"<<endl;
			}
		}

		out << "}" << endl;
	}

public:
	void writeSwitcher(int count)
	{
		out << "module outlines(min=0, max=" << count-1 <<")" << endl;
		out << "{" << endl;
		for(int i=0; i< count; i++)
		{
			out << "	if(min <= "<< i <<" && max >=" << i << ")" << endl;
			out << "	{" << endl;
			out << "		out_"   << i << "();"<< endl;
			out << "	}" << endl;
		}
		out << "}"<< endl;
		out << endl;
		out << "module triangles(min=0, max=" << count-1<<")" << endl;
		out << "{" << endl;
		for(int i=0; i< count; i++)
		{
			out << "	if(min <= "<< i <<" && max >=" << i << ")" << endl;
			out << "	{" << endl;
			out << "		tri_"   << (int)i << "();"<< endl;
			out << "	}" << endl;
		}
		out << "}"<< endl;
		out << endl;
		out << "module extrusions(min=0, max=" << count-1<<")" << endl;
		out << "{" << endl;
		for(int i=0; i< count; i++)
		{
			out << "	if(min <= "<< i <<" && max >=" << i << ")" << endl;
			out << "	{" << endl;
			out << "		fill_" << i << "();"<< endl;
			out << "	}" << endl;
		}
		out << "}"<< endl;
		out << endl;

		out << "// try import instead of import_stl depending on your version of OpenSCAD" << endl;
//		out << "min=0;" << endl;
//		out << "max=" << count -1 << ";" << endl;
//		out << "triangles(min, max);" << endl;
//		out << "outline(min, max);" << endl;
//		out << "fill(min,max);" << endl;
	}

	~ScadTubeFile()
	{
		out.close();
	}
};

}
#endif
