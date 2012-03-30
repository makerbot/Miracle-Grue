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
#define ABSTRACTABLE_H_ (1)


#include <ctime>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/stat.h>

namespace mgl {




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

	char getPathSeparatorCharacter() const;

	::std::string ExtractDirectory(const char *directoryPath) const;
	::std::string ExtractFilename(const char* filename) const;
	::std::string ChangeExtension(const char* filename, const char* extension) const;
	::std::string removeExtension(const char *filename) const;

	::std::string pathJoin(std::string path, std::string filename) const;

	int guarenteeDirectoryExists(const char* dirPath,
				mode_t mode =
						(__S_IREAD|__S_IWRITE |S_IRGRP|S_IWGRP |S_IROTH) );
};


class MyComputer
{
public:
	ClockAbstractor clock;
	FileSystemAbstractor fileSystem;
};


//
// ASCII art
//
class ProgressBar
{
	unsigned int total;
	unsigned int delta;
	unsigned int progress;
	unsigned int ticks;
	MyComputer myPc;

public:

	ProgressBar(unsigned int count);

	void reset(unsigned int count);

	void tick();
};

}

#endif
