/**
 * Utility functions used by multiple unit tests
 */

#include "UnitTestUtils.h"

#include "mgl/ScadDebugFile.h"

using namespace mgl;
using namespace std;
using namespace libthing;



void clip(const std::vector<LineSegment2> &in,
			unsigned int min,
			unsigned int max,
			std::vector<LineSegment2> &out)
{
	int newSize = in.size() - (max -min);
	std::cout << "NEW SIZE " << newSize << std::endl;
	assert(newSize > 0);
	out.reserve(newSize);


	for(unsigned int i =0; i <in.size(); i++)
	{
		const LineSegment2 s = in[i];
		if(i == min-1)
		{
			i = max+1; // advance the cursor
			LineSegment2 s1 = in[i];
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

/// Creates a directory if it doesn't already exist
/// @return true if directory exists at the end of call, false otherwise
int verifyDir(const char *pathname)
{

	mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;
	int status = 0;

	struct stat st;
	if(stat(pathname,&st) != 0){
		mode_t process_mask = umask(0);
		int result_code = mkdir(pathname, mode);
		umask(process_mask);
		if(result_code != 0)
			status = -1 ; //creation fail
	}
	else if (!S_ISDIR(st.st_mode))
		status = -1;
	return status;
}

//
/// mkDebugPath - builds a directory patk for debugging
/// and unit testing data. The directory has extremely premissive
/// permissions in order to show up in the UI.

/// Algorithm takes the pessimistic view and works top-down to ensure
/// each directory in path exists, rather than optimistically creating
/// the last element and working backwards.
/// @param path: desired path for directory creation
void mkDebugPath(const char *path)
{
	char           *pp;
	char           *sp;
	int             status;
	char           copypath[128];
	strncpy(copypath, path,128);
	mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;

	status = 0;
	pp = copypath;
	while (status == 0 && (sp = strchr(pp, '/')) != 0)
	{
	    if (sp != pp)
	    {
	        *sp = '\0';
	        status = verifyDir((const char*)copypath);
	        *sp = '/';
	    }
	    pp = sp + 1;
	}
	if (status == 0)
	    status = verifyDir(path);
	//return (status);
	assert(status ==0);
}

#endif
