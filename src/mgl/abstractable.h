/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/



//
//  Platform abstraction layer... things and stuff that didn't fit
//  anywhere else.
//
//  the mundane and superfluous live here

#ifndef ABSTRACTABLE_H_
#define ABSTRACTABLE_H_


#include <ctime>
#include <iostream>
#include <sstream>

#include <sys/stat.h>
#include <string.h>

#include "mgl.h"
namespace mgl {




//
// ASCII art
//
class ProgressBar
{
	unsigned int total;
	unsigned int delta;
	unsigned int progress;
	unsigned int ticks;
public:
	ProgressBar(unsigned int count)
	{
		reset(count);
		::std::cout << ":";
	}

	void reset(unsigned int count)
	{
		total = count;
		progress = 0;
		delta = count /10;
	}

	void tick()
	{
		total --;
		ticks ++;
		if (ticks >= delta)
		{
			ticks = 0;
			progress ++;
			::std::cout << ".";
			::std::cout.flush();
		}
		if (total ==0)
		{
			::std::cout << "*" << ::std::endl;
		}
	}
};

class ClockAbstractor
{
public:
	::std::string now() const
	{
		time_t t = time(0);   // get time now
#ifdef WIN32
#pragma warning(disable:4996)
#endif
		struct tm * now = localtime( & t );
		::std::stringstream ss;
		ss << (now->tm_year + 1900) << '-' << (now->tm_mon + 1) << '-' <<  now->tm_mday << " "
		 <<  now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec;
		return ss.str();
	}
};

class FileSystemAbstractor
{
public:

	char getPathSeparatorCharacter() const
	{
		return '/'; // Linux & Mac, works on Windows most times
	}

	::std::string ExtractDirectory(const char *directoryPath) const
	{
		const ::std::string path(directoryPath);
		return path.substr(0, path.find_last_of(getPathSeparatorCharacter()) + 1);
	}

	::std::string ExtractFilename(const char* filename) const
	{
		std::string path(filename);
		return path.substr(path.find_last_of(getPathSeparatorCharacter()) + 1);
	}

	::std::string ChangeExtension(const char* filename, const char* extension) const
	{
		const ::std::string path(filename);
		const ::std::string ext(extension);
		std::string filenameStr = ExtractFilename(path.c_str());
		return ExtractDirectory(path.c_str())
				+ filenameStr.substr(0, filenameStr.find_last_of('.')) + ext;
	}

	::std::string removeExtension(const char *filename) const
	{
		const ::std::string path(filename);
		::std::string filenameStr = ExtractFilename(path.c_str());
		return ExtractDirectory(path.c_str())
				+ filenameStr.substr(0, filenameStr.find_last_of('.'));
	}

	/**
	 * Creates a directory if it doesn't already exist
	 * @return true if directory exists at the end of call, false otherwise
	 */
	int verifyDir(const char *pathname, mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO) const
	{
		int status = 0;
		std::cout << pathname << std::endl;
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



	typedef struct stat Stat;

	/**
	** mkpath - ensure all directories in path exist
	** Algorithm takes the pessimistic view and works top-down to ensure
	** each directory in path exists, rather than optimistically creating
	** the last element and working backwards.
	*/
	int mkpath(const char *path, mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO)
	{
	    char           *pp;
	    char           *sp;
	    int             status;
	    char           copypath[128];
		strncpy(copypath, path,128);

	    status = 0;
	    pp = copypath;
	    while (status == 0 && (sp = strchr(pp, '/')) != 0)
	    {
	        if (sp != pp)
	        {
	            /* Neither root nor double slash in path */
	            *sp = '\0';
	            status = verifyDir((const char*)copypath, mode);
	            *sp = '/';
	        }
	        pp = sp + 1;
	    }
	    if (status == 0)
	        status = verifyDir(path, mode);
	    //FREE(copypath);
	    return (status);
	}






};


class MyComputer
{
public:
	ClockAbstractor clock;
	FileSystemAbstractor fileSystem;

};

}

#endif
