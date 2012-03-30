#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cassert>





#ifdef WIN32

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
	int r =  SHCreateDirectoryExA( NULL, spath.c_str() , NULL );
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



int main( int argc, char* argv[] ){
  // Create the event manager and test controller
  CPPUNIT_NS::TestResult controller;

  // Add a listener that colllects test result
  CPPUNIT_NS::TestResultCollector result;
  controller.addListener( &result );

  // Add a listener that print dots as test run.
  CPPUNIT_NS::BriefTestProgressListener progress;
  controller.addListener( &progress );

  // Add the top suite to the test runner
  CPPUNIT_NS::TestRunner runner;
  runner.addTest( CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest() );
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write();

  return result.wasSuccessful() ? 0 : 1;
}


