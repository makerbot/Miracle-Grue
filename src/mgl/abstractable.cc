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

#include "log.h"

#ifdef WIN32
#include <Windows.h>
#endif


std::ostream &MyComputer::log()
{
    return cout;
}


int FileSystemAbstractor::guarenteeDirectoryExists(const char* )//pathname  )
{

#ifdef QT_CORE_LIB
    Log::often() << "not supported on QT" << endl;
    return -1;
#else
    int status = 0;

#ifdef WIN32
        DWORD attrib = GetFileAttributes(pathname);

        if (attrib == INVALID_FILE_ATTRIBUTES) {
            BOOL result = CreateDirectoryA(pathname, NULL);
            if (!result) 
                status = -1; //creation fail
        }
        else if (!(attrib & FILE_ATTRIBUTE_DIRECTORY)) 
            status = -1;

#else //WIN32
        // mode_t does not work under QT
        mode_t mode =  (S_IREAD|S_IWRITE |S_IRGRP|S_IWGRP |S_IROTH);

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
#endif //!WIN32
#endif //!QT_CORE_LIB

}

string FileSystemAbstractor::pathJoin(string path, string filename) const
{
#ifdef WIN32
    return path  + "\\" + filename;
#else
	return path  + "/" + filename;
#endif
}

char FileSystemAbstractor::getPathSeparatorCharacter() const
{
	return '/'; // Linux & Mac, works on Windows most times
}

string FileSystemAbstractor::ExtractDirectory(const char *directoryPath) const
{
	const string path(directoryPath);
	return path.substr(0, path.find_last_of(getPathSeparatorCharacter()) + 1);
}

string FileSystemAbstractor::ExtractFilename(const char* filename) const
{
	std::string path(filename);
	return path.substr(path.find_last_of(getPathSeparatorCharacter()) + 1);
}

string FileSystemAbstractor::ChangeExtension(const char* filename, const char* extension) const
{
	const string path(filename);
	const string ext(extension);
	std::string filenameStr = ExtractFilename(path.c_str());
	return ExtractDirectory(path.c_str())
			+ filenameStr.substr(0, filenameStr.find_last_of('.')) + ext;
}

string FileSystemAbstractor::removeExtension(const char *filename) const
{
	const string path(filename);
	string filenameStr = ExtractFilename(path.c_str());
	return ExtractDirectory(path.c_str())
			+ filenameStr.substr(0, filenameStr.find_last_of('.'));
}

ProgressLog::ProgressLog(unsigned int count)
    :ProgressBar(count,"")
{
        reset(count);
    Log::often() << ":";

}


void ProgressLog::onTick(const char* taskName, unsigned int count, unsigned int ticks)
{
	if (ticks == 0) {
		this->deltaTicks = 0;
		this->deltaProgress = 0;
		this->delta = count / 10;
		cout << taskName;
	}

	if (deltaTicks >= this->delta)
	{
		deltaProgress++;
		cout << " [" << deltaProgress * 10 << "%] ";
		cout.flush();
        Log::often() << " [" << deltaProgress * 10 << "%] ";
		this->deltaTicks = 0;

	}
	if ( ticks >= count -1  ) {

		string now = myPc.clock.now();
        Log::often() << now;
		cout << now << endl;
	}
	deltaTicks++;
}






