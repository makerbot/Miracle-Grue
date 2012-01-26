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
//	double layerH;
//	double layerW;
public:
	ScadTubeFile()
	{}

	void open(const char* filename, double layerH, double layerW,  unsigned int max)
	{
		out.open(filename, std::ios::out);
		if(!out.good())
		{
			std::stringstream ss;
			ss << "Can't open \"" << filename << "\"";
			std::cout << "ERROR: " << ss.str() << std::endl;
			ScadMess problem(ss.str().c_str());
			throw (problem);
		}

	    //out.setf(ios_base::floatfield, ios::floatfield);
		//out.precision(18);
	    out.setf(std::ios::fixed);

	    out << "// use min and max to see individual layers " << std::endl;
	    out << "min = 0;" << std::endl;
	    out << "max = " << max << ";" << std::endl;
	    out << "// triangles(min,max);" << std::endl;
	    out << "outlines(min,max);" << std::endl;
	    out << "// extrusions(min,max);" << std::endl;
	    out << std::endl;
	    out << "stl_color = [0,1,0, 0.025];" << std::endl;

		out << "module out_line(x1, y1, z1, x2, y2, z2)" << std::endl;
		out << "{" << std::endl;
		out << "    tube(x1, y1, z1, x2, y2, z2, diameter1=0.4, diameter2=0, faces=4, thickness_over_width=1);" << std::endl;
		out << "}" << std::endl;

		out  << std::endl;
		out << "module extrusion(x1, y1, z1, x2, y2, z2)" << std::endl;
		out << "{" << std::endl;
		out << "    d = " << layerH << ";" << std::endl;
		out << "    f = 6;" << std::endl;
		out << "    t =  "  << layerH / layerW << ";"<< std::endl;
		out << "    corner(x1,y1,z1, diameter=d, faces=f, thickness_over_width =t );" << std::endl;
		out << "    tube(x1, y1, z1, x2, y2, z2, diameter1=d, diameter2=d, faces=f, thickness_over_width=t);" << std::endl;
		out << "}" << std::endl;

		out << std::endl;
		out << "module outline(points, paths)" << std::endl;
		out << "{" << std::endl;
		out << "    for (p= paths)" << std::endl;
		out << "    {" << std::endl;
		out << "       out_line(points[p[0]][0],points[p[0]][1],points[p[0]][2],points[p[1]][0],points[p[1]][1],points[p[1]][2] );" << std::endl;
		out << "    }" << std::endl;
		out << "}" << std::endl;
		out << std::endl;

		out << std::endl;
		out << "module infill(points, paths)" << std::endl;
		out << "{" << std::endl;
		out << "     for (p= paths)" << std::endl;
		out << "    {" << std::endl;
		out << "        extrusion(points[p[0]][0],points[p[0]][1],points[p[0]][2], points[p[1]][0],points[p[1]][1],points[p[1]][2] );" << std::endl;
		out << "    }" << std::endl;
		out << "}" << std::endl;
		out << std::endl;

		out << "module corner(x, y, z, diameter, faces, thickness_over_width ){" << std::endl;
		out << "	translate([x, y, z])  scale([1,1,thickness_over_width]) sphere( r = diameter/2, $fn = faces );" << std::endl;
		out << "}" << std::endl;
		out  << std::endl;

	    out << "module tube(x1, y1, z1, x2, y2, z2, diameter1, diameter2, faces, thickness_over_width)" << std::endl;
	    out << "{" << std::endl;
	    out << "	length = sqrt( pow(x1 - x2, 2) + pow(y1 - y2, 2) + pow(z1 - z2, 2) );" << std::endl;
	    out << "	alpha = ( (y2 - y1 != 0) ? atan( (z2 - z1) / (y2 - y1) ) : 0 );" << std::endl;
	    out << " 	beta = 90 - ( (x2 - x1 != 0) ? atan( (z2 - z1) / (x2 - x1) ) :  0 );" << std::endl;
	    out << "	gamma =  ( (x2 - x1 != 0) ? atan( (y2 - y1) / (x2 - x1) ) : ( (y2 - y1 >= 0) ? 90 : -90 ) ) + ( (x2 - x1 >= 0) ? 0 : -180 );" << std::endl;
	    out << "	// echo(Length = length, Alpha = alpha, Beta = beta, Gamma = gamma);	" << std::endl;
	    out << "	translate([x1, y1, z1])" << std::endl;
	    out << "	rotate([ 0, beta, gamma])" << std::endl;
	    out << "		scale([thickness_over_width,1,1])" << std::endl;
	    out << "			rotate([0,0,90]) cylinder(h = length, r1 = diameter1/2, r2 = diameter2/2, center = false, $fn = faces );" << std::endl;
	    out << "}" << std::endl;

	    out << std::endl;
	    out << "module outline_segments(segments)" << std::endl;
	    out << "{" << std::endl;
	    out << "    for(seg = segments)" << std::endl;
	    out << "    {" << std::endl;
	    out << "        out_line(seg[0][0], seg[0][1], seg[0][2], seg[1][0], seg[1][1], seg[1][2]);" << std::endl;
	    out << "    }" << std::endl;
	    out << "}" << std::endl;
	    out << std::endl;
	}

	std::ofstream &getOut(){return out;}


	void close()
	{
		out.close();
	}



	void writeOutlines(const Polygons &loops, Scalar z, int slice)
	{

		out << "module outlines_" << slice << "()" << std::endl;
		out << "{" << std::endl;
		out << "    points =[" << std::endl;
		for (int i=0; i < loops.size(); i++)
		{
			out << "               [" << std::endl;
			const Polygon& loop  = loops[i];
			for (int j=0; j < loop.size(); j++)
			{
				Scalar x = loop[j].x;
				Scalar y = loop[j].y;
				out << "                   [" << x << ", " <<  y << ", " << z << "]," << std::endl;
			}
			out << "                ]," << std::endl;

		}
		out << "              ];" << std::endl;

		out << "    segments =[" << std::endl;
		for (int i=0; i < loops.size(); i++)
		{
			out << "               [" << std::endl;
			const Polygon& loop  = loops[i];
			for (int j=0; j < loop.size()-1; j++)
			{
				int a = j;
				int b = j+1;
				out << "                  [" << a << ", " <<  b << "]," << std::endl;
			}
			out << "              ]," << std::endl;
		}
		out << "            ];" << std::endl;

		out << std::endl;
		for (int i=0; i < loops.size(); i++)
		{
			out << "    outline(points[" << i << "], segments[" << i << "] );" << std::endl;
		}
		out << std::endl;
		out << "}" << std::endl;

	}

	void writePolygons(const char* moduleName,
						const char* implementation,
							const Polygons &polygons,
								Scalar z,
									int slice)
	{
		out << "module " << moduleName << slice << "()" << std::endl;
		out << "{" << std::endl;
		out << "    points =[" << std::endl;
		for (int i=0; i < polygons.size(); i++)
		{
			out << "               [" << std::endl;
			const Polygon& poly  = polygons[i];
			for (int j=0; j < poly.size(); j++)
			{
				Scalar x = poly[j].x;
				Scalar y = poly[j].y;
				out << "                   [" << x << ", " <<  y << ", " << z << "]," << std::endl;
			}
			out << "                ]," << std::endl;

		}
		out << "              ];" << std::endl;

		out << "    segments =[" << std::endl;
		for (int i=0; i < polygons.size(); i++)
		{
			out << "               [" << std::endl;
			const Polygon& poly  = polygons[i];
			for (int j=0; j < poly.size()-1; j++)
			{
				int a = j;
				int b = j+1;
				out << "                  [" << a << ", " <<  b << "]," << std::endl;
			}
			out << "              ]," << std::endl;
		}
		out << "            ];" << std::endl;

		out << std::endl;
		for (int i=0; i < polygons.size(); i++)
		{
			out << "    " <<  implementation << "(points[" << i << "], segments[" << i << "] );" << std::endl;
		}
		out << std::endl;
		out << "}" << std::endl;

	}

	void writeSegments(const char* name,
						const char* implementation,
							const std::vector<LineSegment2> &segments,
							Scalar z,
							int slice)
	{
		out << "module " << name << slice << "()" << std::endl;
		out << "{" << std::endl;
		out << "    segments =[" << std::endl;
		for (int i=0; i < segments.size(); i++)
		{
			const LineSegment2& segment  = segments[i];
			Scalar ax = segment.a[0];
			Scalar ay = segment.a[1];
			Scalar bx = segment.b[0];
			Scalar by = segment.b[1];

			out << "                   ";
			out << "[[" << ax << ", " <<  ay << ", " << z << "],";
			out << "[" << bx << ", " <<  by << ", " << z << "]],";
			out << std::endl;
		}

		out << "              ];" << std::endl;
		out << "    " <<  implementation << "(segments);" << std::endl;
		out << "}" << std::endl;
		out << std::endl;
	}

	void writeTrianglesModule(const char* name, const Meshy &mesh,
								unsigned int layerIndex) //const std::vector<BGL::Triangle3d>  &allTriangles, const TriangleIndices &trianglesForSlice)
	{
		std::stringstream ss;
		ss.setf(std::ios::fixed);
		const TriangleIndices &trianglesForSlice = mesh.readSliceTable()[layerIndex];
		const std::vector<Triangle3>  &allTriangles = mesh.readAllTriangles();

		ss << "module " << name << layerIndex << "(col=[1,0,0,1])" << std::endl;
		// , [0,2,1], [3,0,4], [1,2,5], [0,5,4], [0,1,5],  [5,2,4], [4,2,3],
		ss << "{" << std::endl;
		ss << "    color(col) polyhedron ( points = [";

		// ss << scientific;
		ss << std::dec; // set decimal format for floating point numbers

		for(int i=0; i< trianglesForSlice.size(); i++ )
		{
			//index_t index = *i;
			index_t index = trianglesForSlice[i];
			const Triangle3 &t = allTriangles[index];
			ss << "    [" << t[0].x << ", " << t[0].y << ", " << t[0].z << "], ";
			ss <<     "[" << t[1].x << ", " << t[1].y << ", " << t[1].z << "], ";
			ss <<     "[" << t[2].x << ", " << t[2].y << ", " << t[2].z << "], // tri " << i << std::endl;
		}

		ss << "]," << std::endl;
		ss << "triangles = [" ;

		for (int i=0; i < trianglesForSlice.size(); i++)
		{
			int tri = i * 3;
			ss << "    [" << tri << ", " << tri+1 << ", " << tri + 2 << "], " << std::endl;
		}

		ss << "]);" << std::endl;
		ss << "}" << std::endl;
		ss << std::endl;
		out << ss.str();
	}

private:
/*

	void writeStlModule(const char* moduleName, const char *stlName,  int slice)
	{
		out << std::endl;
		out << "module " << moduleName << slice << "()" << std::endl;
		out << "{" << std::endl;
		out << "    color(stl_color)import_stl(\"" << stlName<< slice << ".stl\");" << std::endl;
		out << "}" << std::endl;

	}


  	void writeExtrusionsModule(const char* name, const std::vector<mgl::Segment> &segments, int slice, Scalar z)
	{
		writeTubesModule(name, "extrusion", segments, slice, z);
	}

	void writeOutlinesModule(const char* name, const std::vector<std::vector<Segment> > &loops, int slice, Scalar z)
	{
		writeMultiTubesModule(name, "out_line", loops, slice, z);
	}

	void writeTubesModule(const char* name, const char* tubeType,const std::vector<Segment> &segments, int slice, Scalar z)
	{

		// tube(x,y,z,  x,y,z, d,f,t);
		out << std::endl;
		out << "module " << name << slice << "()"<< std::endl;
		out << "{" << std::endl;
		for(int j=0; j<segments.size(); j++)
		{
			const Segment &segment = segments[j];
			out << "	"<< tubeType<< "(" << segment.a.x << ", " << segment.a.y << ", " << z << ", ";
			out << 				 segment.b.x << ", " << segment.b.y << ", " << z << ");"<< std::endl;
		}

		out << "}" << std::endl;
	}


	void writeMultiTubesModule(const char* name, const char* tubeType,const std::vector<std::vector<Segment> > &loops, int slice, Scalar z)
	{

		// tube(x,y,z,  x,y,z, d,f,t);
		out << std::endl;
		out << "module " << name << slice << "()"<< std::endl;
		out << "{" << std::endl;

		unsigned int loopCount = loops.size();

		for(int i=0; i<loopCount; i++)
		{
			out << "// loop " << i << std::endl;
			const std::vector<Segment> &segments = loops[i];

			unsigned int segmentCount = segments.size();

			Scalar green = 0.5 + 0.5 * (i+1 / (loopCount )) ;
			for(int j=0; j<segmentCount; j++)
			{
				Scalar blue = 0.5 + 0.5 * (i+1 / (segmentCount )) ;
				const Segment &segment = segments[j];
				out << "	"<< "color([0," << green << ", "<< blue <<", 1])" << std::endl;
				out << "	"<< tubeType<< "(" << segment.a.x << ", " << segment.a.y << ", " << z << ", ";
				out << 		 segment.b.x << ", " << segment.b.y << ", " << z << ");"<< std::endl;
			}
		}

		out << "}" << std::endl;
	}

*/

public:
	void writeSwitcher(int count)
	{
		out << "module outlines(min=0, max=" << count-1 <<")" << std::endl;
		out << "{" << std::endl;
		for(int i=0; i< count; i++)
		{
			out << "	if(min <= "<< i <<" && max >=" << i << ")" << std::endl;
			out << "	{" << std::endl;
			out << "		outlines_"   << i << "();"<< std::endl;
			out << "	}" << std::endl;
		}
		out << "}"<< std::endl;
		out << std::endl;
		out << "module triangles(min=0, max=" << count-1<<")" << std::endl;
		out << "{" << std::endl;
		for(int i=0; i< count; i++)
		{
			out << "	if(min <= "<< i <<" && max >=" << i << ")" << std::endl;
			out << "	{" << std::endl;
			out << "		tri_"   << (int)i << "();"<< std::endl;
			out << "	}" << std::endl;
		}
		out << "}"<< std::endl;
		out << std::endl;
		out << "module extrusions(min=0, max=" << count-1<<")" << std::endl;
		out << "{" << std::endl;
		for(int i=0; i< count; i++)
		{
			out << "	if(min <= "<< i <<" && max >=" << i << ")" << std::endl;
			out << "	{" << std::endl;
			out << "		infill_" << i << "();"<< std::endl;
			out << "	}" << std::endl;
		}
		out << "}"<< std::endl;
		out << std::endl;

		out << "// try import instead of import_stl depending on your version of OpenSCAD" << std::endl;

	}

	~ScadTubeFile()
	{
		out.close();
	}
};

}
#endif
