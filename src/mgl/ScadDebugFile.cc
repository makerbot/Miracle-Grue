#include "ScadDebugFile.h"

using namespace std;
using namespace mgl;

#include "log.h"

ScadDebugFile::ScadDebugFile() :filename("")
	{}

void ScadDebugFile::open(const char* path)
{
	assert(isOpened() == false);
	filename = path;
	out.open(filename.c_str(), ios::out);
	if(!out.good())
	{
		stringstream ss;
		ss << "Can't open \"" << filename.c_str() << "\"";
		string tmp = ss.str();
        Log::info() << "ERROR: " << tmp << endl;
		ScadException problem(ss.str().c_str());
		throw (problem);
	}

	out.precision(5);
	out.setf(ios::fixed);


}

string ScadDebugFile::getScadFileName() const
{
	return filename;
}

bool ScadDebugFile::isOpened()
{
	unsigned int l = filename.length();
	return (l > 0);
}



ostream& ScadDebugFile::getOut()
{return out;}

void ScadDebugFile::writeHeader()
{
	out << "module corner(x, y, z, diameter, faces, thickness_over_width ){" << endl;
	out << "	translate([x, y, z])  scale([1,1,thickness_over_width]) sphere( r = diameter/2, $fn = faces );" << endl;
	out << "}" << endl;
	out << endl;
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

void ScadDebugFile::close()
{
	out.close();
	filename = "";
}



void ScadDebugFile::writeOutlines(const Polygons &loops, Scalar z, int slice)
{

	out << "module outlines_" << slice << "()" << endl;
	out << "{" << endl;
	out << "    points =[" << endl;
	for (size_t i=0; i < loops.size(); i++)
	{
		out << "               [" << endl;
		const Polygon& loop  = loops[i];
		for (size_t j=0; j < loop.size(); j++)
		{
			Scalar x = loop[j].x;
			Scalar y = loop[j].y;
			out << "                   [" << x << ", " <<  y << ", " << z << "]," << endl;
		}
		out << "                ]," << endl;

	}
	out << "              ];" << endl;

	out << "    segments =[" << endl;
	for (size_t i=0; i < loops.size(); i++)
	{
		out << "               [" << endl;
		const Polygon& loop  = loops[i];
		for (size_t j=0; j < loop.size()-1; j++)
		{
			int a = j;
			int b = j+1;
			out << "                  [" << a << ", " <<  b << "]," << endl;
		}
		out << "              ]," << endl;
	}
	out << "            ];" << endl;

	out << endl;
	for (size_t i=0; i < loops.size(); i++)
	{
		out << "    outline(points[" << i << "], segments[" << i << "] );" << endl;
	}
	out << endl;
	out << "}" << endl;

}

void ScadDebugFile::writePolygons(const char* moduleName,
					const char* implementation,
						const Polygons &polygons,
							Scalar z,
								int slice)
{

	//EZLOGGERVLSTREAM << "<writePolygons: " << polygons.size() << " polygons >"<< endl;

	out << "module " << moduleName << slice << "()" << endl;
	out << "{" << endl;
	out << "    points =[" << endl;

	for (size_t i=0; i < polygons.size(); i++)
	{
		out << "               [" << endl;
		const Polygon& poly  = polygons[i];

		//EZLOGGERVLSTREAM << "   Polygon " << i << ": " << poly.size() << " points "<< endl;

		for (size_t j=0; j < poly.size(); j++)
		{
			Scalar x = poly[j].x;
			Scalar y = poly[j].y;
			out << "                   [" << x << ", " <<  y << ", " << z << "]," << endl;
		}
		out << "                ]," << endl;

	}
	out << "              ];" << endl;


	out << "    segments =[" << endl;

	size_t polysCount = polygons.size();
	for (size_t i=0; i < polysCount; i++)
	{

		out << "               [" << endl;
		const Polygon& poly  = polygons[i];
		size_t polyCount =  poly.size();

		for (size_t j=0; j < polyCount-1; j++)
		{
			// EZLOGGERVLSTREAM << "  writePolygons: " << j << " polycount: " << polyCount << endl;
			int a = j;
			int b = j+1;
			out << "                  [" << a << ", " <<  b << "]," << endl;
		}
		out << "              ]," << endl;
	}
	out << "            ];" << endl;



	out << endl;
	for (size_t i=0; i < polygons.size(); i++)
	{
		out << "    " <<  implementation << "(points[" << i << "], segments[" << i << "] );" << endl;


	}
	out << endl;
	out << "}" << endl;

	// EZLOGGERVLSTREAM << "</writePolygons>" << endl;
}

Scalar ScadDebugFile::segment3(	ostream &out,
						const char* indent,
						const char* variableName,
						const vector<SegmentType> &segments,
						Scalar z,
						Scalar dz)
{
	out << indent << variableName<< " =[" << endl;
	for (size_t i=0; i < segments.size(); i++)
	{
		const SegmentType& segment  = segments[i];
		Scalar ax = segment.a[0];
		Scalar ay = segment.a[1];
		Scalar az = z;

		Scalar bx = segment.b[0];
		Scalar by = segment.b[1];
		Scalar bz = z;

		out << indent << indent;
		out << "[[" << ax << ", " <<  ay << ", " << az << "],";
		out << "[ " << bx << ", " <<  by << ", " << bz << "]],";
		out << endl;
		z += dz;
	}
	out << indent << indent <<  "];" << endl;
	return z;
}

Scalar ScadDebugFile::writeSegments3(	const char* name,
						const char* implementation,
						const vector<SegmentType> &segments,
						Scalar z,
						Scalar dz,
						int slice)
{
	out << "module " << name << slice << "()" << endl;
	out << "{" << endl;
	z = segment3(out, "    ", "segments", segments, z, dz);
	out << "    " <<  implementation << "(segments);" << endl;
	out << "}" << endl;
	out << endl;
	return z;
}

void ScadDebugFile::writeSegments2(	const char* name,
						const char* implementation,
						const vector<SegmentType> &segments,
						Scalar z,
						int slice)
{
	out << "module " << name << slice << "()" << endl;
	out << "{" << endl;
	out << "    segments =[" << endl;
	for (size_t i=0; i < segments.size(); i++)
	{
		const SegmentType& segment  = segments[i];
		Scalar ax = segment.a[0];
		Scalar ay = segment.a[1];
		Scalar bx = segment.b[0];
		Scalar by = segment.b[1];

		out << "                   ";
		out << "[[" << ax << ", " <<  ay <<  "],";
		out << "[ " << bx << ", " <<  by <<  "]],";
		out << endl;
	}

	out << "              ];" << endl;
	out << "    " <<  implementation << "(segments," << z <<");" << endl;
	out << "}" << endl;
	out << endl;
}

// writes a list of triangles into a polyhedron.
// It is used to display the triangles involved in a slice (layerIndex).

void ScadDebugFile::writeTrianglesModule(	const char* name,
							const vector<libthing::Triangle3>  &allTriangles,
							const TriangleIndices &trianglesForSlice,
							unsigned int layerIndex)
{
	stringstream ss;
	ss.setf(ios::fixed);

	ss << "module " << name << layerIndex << "(col=[1,0,0,1])" << endl;
	ss << "{" << endl;
	ss << "    color(col) polyhedron ( points = [";

	ss << dec; // set decimal format for floating point numbers

	for(size_t i=0; i< trianglesForSlice.size(); i++ )
	{

		index_t index = trianglesForSlice[i];
		const libthing::Triangle3 &t = allTriangles[index];
		ss << "    [" << t[0].x << ", " << t[0].y << ", " << t[0].z << "], ";
		ss <<     "[" << t[1].x << ", " << t[1].y << ", " << t[1].z << "], ";
		ss <<     "[" << t[2].x << ", " << t[2].y << ", " << t[2].z << "], // tri " << i << endl;
	}

	ss << "]," << endl;
	ss << "triangles = [" ;

	for (size_t i=0; i < trianglesForSlice.size(); i++)
	{
		int tri = i * 3;
		ss << "    [" << tri << ", " << tri+1 << ", " << tri + 2 << "], " << endl;
	}

	ss << "]);" << endl;
	ss << "}" << endl;
	ss << endl;
	out << ss.str();
}

/*
	void writeTrianglesModule(const char* name, const Meshy &mesh,
								unsigned int layerIndex)
	{
		const TriangleIndices &trianglesForSlice = mesh.readSliceTable()[layerIndex];
		const vector<Triangle3>  &allTriangles = mesh.readAllTriangles();
		writeTrianglesModule(name, allTriangles, trianglesForSlice, layerIndex);
	}
*/

void ScadDebugFile::writeMinMax(const char*name, const char* implementation, int count)
{
	out << "module "<< name << "(min=0, max=" << count-1 <<")" << endl;
	out << "{" << endl;
	for(int i=0; i< count; i++)
	{
		out << "	if(min <= "<< i <<" && max >=" << i << ")" << endl;
		out << "	{" << endl;
		out << "		" << implementation   << i << "();"<< endl;
		out << "	}" << endl;
	}
	out << "}" << endl;
}

ScadDebugFile::~ScadDebugFile()
{
	out.close();
}
