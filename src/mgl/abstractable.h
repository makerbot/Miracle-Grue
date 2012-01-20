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
#include <sstream>


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
		: total(count), progress(0), delta(count/10)
	{
		std::cout << ":";
	}

	void tick()
	{
		total --;
		ticks ++;
		if (ticks >= delta)
		{
			ticks = 0;
			progress ++;
			std::cout << ".";
			std::cout.flush();
		}
		if (total ==0)
		{
			std::cout << "*" << std::endl;
		}
	}
};

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

/*
class Timer
{
	unsigned int t0;

public:

	Timer()
	{
		start();
	}

	void start()
	{
		t0 = clock();
	}

	double ellapsed()
	{
		unsigned int t = clock() - t0;
		return t / 10000.0;
	}

	static void init()
	{
		srand( time(NULL) );
	}
};
*/

class MyComputer
{
public:
	ClockAbstractor clock;
	FileSystemAbstractor fileSystem;
};

}

#endif
