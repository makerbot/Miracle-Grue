/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

**/
#include "abstractable.h"


using namespace mgl;
using namespace std;




char FileSystemAbstractor::getPathSeparatorCharacter() const
{
	return '/'; // Linux & Mac, works on Windows most times
}

::std::string FileSystemAbstractor::ExtractDirectory(const char *directoryPath) const
{
	const ::std::string path(directoryPath);
	return path.substr(0, path.find_last_of(getPathSeparatorCharacter()) + 1);
}

::std::string FileSystemAbstractor::ExtractFilename(const char* filename) const
{
	std::string path(filename);
	return path.substr(path.find_last_of(getPathSeparatorCharacter()) + 1);
}

::std::string FileSystemAbstractor::ChangeExtension(const char* filename, const char* extension) const
{
	const ::std::string path(filename);
	const ::std::string ext(extension);
	std::string filenameStr = ExtractFilename(path.c_str());
	return ExtractDirectory(path.c_str())
			+ filenameStr.substr(0, filenameStr.find_last_of('.')) + ext;
}

::std::string FileSystemAbstractor::removeExtension(const char *filename) const
{
	const ::std::string path(filename);
	::std::string filenameStr = ExtractFilename(path.c_str());
	return ExtractDirectory(path.c_str())
			+ filenameStr.substr(0, filenameStr.find_last_of('.'));
}

ProgressBar::ProgressBar(unsigned int count)
:total(0), delta(0), progress(0), ticks(0)
{
	reset(count);
	cout << ":";
}


void ProgressBar::reset(unsigned int count)
{
	ticks=0;
	total = count;
	progress = 0;
	delta = count /10;
}


void ProgressBar::tick()
{
	total --;
	ticks ++;
	if (ticks >= delta)
	{
		ticks = 0;
		progress ++;
		cout << " [" << progress * 10<< "%] ";
		cout.flush();

	}
	if (total ==0)
	{
		// ::std::cout << "" << ::std::endl;
		std::cout << myPc.clock.now() << std::endl;
	}
}






