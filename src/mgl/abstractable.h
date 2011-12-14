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

#include <sstream>


namespace mgl {

//
// <mondane> stuff to deal with 70's style File systems in the 21st century
//
//

class ClockAbstractor
{
public:
	std::string now() const
	{
		time_t t = time(0);   // get time now
		struct tm * now = localtime( & t );
		std::stringstream ss;
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

	std::string ExtractDirectory(const std::string& path) const
	{
		return path.substr(0, path.find_last_of(getPathSeparatorCharacter()) + 1);
	}

	std::string ExtractFilename(const std::string& path) const
	{
		return path.substr(path.find_last_of(getPathSeparatorCharacter()) + 1);
	}

	std::string ChangeExtension(const std::string& path, const std::string& ext) const
	{
		std::string filename = ExtractFilename(path);
		return ExtractDirectory(path)
				+ filename.substr(0, filename.find_last_of('.')) + ext;
	}

	std::string removeExtension(const std::string& path) const
	{
		std::string filename = ExtractFilename(path);
		return ExtractDirectory(path)
				+ filename.substr(0, filename.find_last_of('.'));
	}

};

// </mondane>

class MyComputer
{
public:
	ClockAbstractor clock;
	FileSystemAbstractor fileSystem;
};

}

#endif
