/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/
#include "abstractable.h"



#ifdef WIN32
#include <Shlobj.h>
#include <direct.h>
#include <stdio.h>

using namespace mgl;
using namespace std;

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}


int FileSystemAbstractor::mkpath(const char *path)
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
	return r;
}

#else

#include <sys/stat.h>
#include <iostream>
#include <cstring>

using namespace mgl;
using namespace std;

/// Creates a directory if it doesn't already exist
/// @return true if directory exists at the end of call, false otherwise
int verifyDir(const char *pathname)
{

	mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;
	int status = 0;
	//std::cout << pathname << std::endl;
	//TODO: move to abstractable someday
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

//	typedef struct stat Stat;
/// mkpath - ensure all directories in path exist
/// Algorithm takes the pessimistic view and works top-down to ensure
/// each directory in path exists, rather than optimistically creating
/// the last element and working backwards.
int FileSystemAbstractor::mkpath(const char *path)
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
	        /* Neither root nor double slash in path */
	        *sp = '\0';
	        status = verifyDir((const char*)copypath);
	        *sp = '/';
	    }
	    pp = sp + 1;
	}
	if (status == 0)
	    status = verifyDir(path);
	//FREE(copypath);
	return (status);
}

#endif

	







