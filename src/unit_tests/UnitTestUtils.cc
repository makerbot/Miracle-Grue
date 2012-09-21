/**
 * Utility functions used by multiple unit tests
 */

#include "UnitTestUtils.h"

#include "mgl/ScadDebugFile.h"

using namespace mgl;
using namespace std;



void clip(const std::vector<SegmentType> &in,
			unsigned int min,
			unsigned int max,
			std::vector<SegmentType> &out)
{
	int newSize = in.size() - (max -min);
	std::cout << "NEW SIZE " << newSize << std::endl;
	assert(newSize > 0);
	out.reserve(newSize);


	for(unsigned int i =0; i <in.size(); i++)
	{
		const SegmentType s = in[i];
		if(i == min-1)
		{
			i = max+1; // advance the cursor
			SegmentType s1 = in[i];
			s1.a = s.b;
			out.push_back(s);
			out.push_back(s1);
		}
		else
		{
			out.push_back(s);
		}

	}

	std::cout << "clipped segments" << std::endl;
	mgl::ScadDebugFile::segment3(std::cout,"","segments", out, 0, 0.1);

};


#ifdef WIN32

#include <Windows.h>
#include <Shlobj.h>
#include <direct.h>
#include <stdio.h>
#include "UnitTestUtils.h"

using namespace std;

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}


void mkDebugPath(const char *path)
{
	 char cCurrentPath[FILENAME_MAX];
	_getcwd( cCurrentPath, sizeof(cCurrentPath) / sizeof(TCHAR) );
	string spath = cCurrentPath;

	string extended_path = path;
	replaceAll(extended_path, "/", "\\");

	spath += "\\";
	spath += extended_path;
	int r =  CreateDirectoryA( spath.c_str(), NULL );
	if(r == ERROR_ALREADY_EXISTS)
		r = 0;
//	return r;
	assert(r==0);
}

#else

#include <sys/stat.h>
#include <iostream>
#include <cstring>
#include "UnitTestUtils.h"

using namespace std;


#endif
