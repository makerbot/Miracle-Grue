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
#include "segment.h"


namespace mgl
{

class ScadTubeFile
{
	std::ofstream out;
public:
	ScadTubeFile(const char* filename)
	 :out(filename, std::ios::out)
	{
		if(!out)
		{
			stringstream ss;
			ss << "Can't open \"" << filename << "\"";
			cout << "ERROR: " << ss.str() << endl;
			MeshyMess problem(ss.str().c_str());
			throw (problem);
		}

	    //out.setf(ios_base::floatfield, ios::floatfield);
		//out.precision(18);
	    out.setf(ios::fixed);

		//out << "use<tube.scad>" << endl << endl;
		out << "d = 0.35;" << endl;
		out << "f = 6;" << endl;
		out << "t = 0.6;" << endl << endl;
		out  << endl;
		out << "module tube(x1, y1, z1, x2, y2, z2, diameter, faces, thickness_over_width)" << endl;
		out << "{" << endl;
		out << "	" << endl;
		out << "	length = sqrt( pow(x1 - x2, 2) + pow(y1 - y2, 2) + pow(z1 - z2, 2) );" << endl;
		out << "	alpha = ( (y2 - y1 != 0) ? atan( (z2 - z1) / (y2 - y1) ) : 0 );" << endl;
		out << "	beta = 90 - ( (x2 - x1 != 0) ? atan( (z2 - z1) / (x2 - x1) ) :  0 );" << endl;
		out << "	gamma =  ( (x2 - x1 != 0) ? atan( (y2 - y1) / (x2 - x1) ) : ( (y2 - y1 >= 0) ? 90 : -90 ) ) + ( (x2 - x1 >= 0) ? 0 : -180 );" << endl;
		out << "	// echo(Length = length, Alpha = alpha, Beta = beta, Gamma = gamma);	" << endl;
		out << "	translate([x1, y1, z1])" << endl;
		out << "	rotate([ 0, beta, gamma])" << endl;
		out << "		scale([thickness_over_width,1,1])" << endl;
		out << "			rotate([0,0,90]) cylinder(h = length, r = diameter/2, center = false, $fn = faces );" << endl;
		out << "}" << endl;
		out << "" << endl;
		out << "module corner(x, y, z, diameter){" << endl;
		out << "	translate([x, y, z])" << endl;
		out << "	sphere( r = diameter/2 );" << endl;
		out << "}" << endl;
		out << "d = 0.35; // tube diameter" << endl;
		out  << endl;




	}
	void writeStlModule(const char* name, const char *stlName,  int slice)
	{
		out << endl;
		out << "module " << name << slice << "()" << endl;
		out << "{" << endl;
		out << "    import_stl(\"" << stlName<< "\");" << endl;
		out << "}" << endl;

	}

	void writeTubesModule(const char* name, const std::vector<Segment> &segments, int slice, Scalar z)
	{
		// tube(x,y,z,  x,y,z, d,f,t);
		out << endl;
		out << "module " << name << slice << "()"<< endl;
		out << "{" << endl;
		for(int i=0; i<segments.size(); i++)
		{
			const Segment &segment = segments[i];
			out << "	tube(" << segment.a.x << ", " << segment.a.y << ", " << z << ", ";
			out << 				 segment.b.x << ", " << segment.b.y << ", " << z << ", d, f, t);"<<endl;
		}
		out << "}" << endl;
	}

	void writeSwitcher(int count)
	{
		out << "module outline(min=0, max=" << count-1 <<")" << endl;
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
			out << "		stl_"   << (int)i << "();"<< endl;
			out << "	}" << endl;
		}
		out << "}"<< endl;
		out << endl;
		out << "module fill(min=0, max=" << count-1<<")" << endl;
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
	}

	~ScadTubeFile()
	{
		out << "// try import instead of import_stl depending on your version of OpenSCAD" << endl;
		out << "triangles();" << endl;
		out << "outline();" << endl;
		out << "fill();" << endl;

		out.close();
	}
};

}
#endif
