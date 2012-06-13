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
#if defined WIN32 && ! defined __GNUC__
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

    int guarenteeDirectoryExists(const char* dirPath);
	int guarenteeDirectoryExistsRecursive(const char* dirPath);
	bool fileReadable(const char *filename) const;

	::std::string getDataFile(const char *filename) const;
	::std::string getConfigFile(const char *filename) const;
private:
	::std::string getUserDataFile(const char *filename) const;
	::std::string getSystemDataFile(const char *filename) const;
	::std::string getUserConfigFile(const char *filename) const;
	::std::string getSystemConfigFile(const char *filename) const;
};


class MyComputer
{
public:
	ClockAbstractor clock;
	FileSystemAbstractor fileSystem;

    static std::ostream &log();

};




//
// ASCII art
//

class ProgressBar
{

    unsigned int count;
    unsigned int ticks;

protected:
    std::string task;

 public:
    ProgressBar(unsigned int count=0, const char* taskName="")
    {
        reset(count, taskName);
    }

    virtual ~ProgressBar(){};

    void reset(unsigned int count, const char* taskName = "")
    {
        ticks = 0;
        this->count = count;
        task = taskName;
    }

    void tick()
    {
        onTick(task.c_str(), count, ticks);
        ticks++;
    }

    virtual void onTick(const char* taskName, unsigned int size, unsigned int it)=0;

};


class ProgressLog : public ProgressBar
{
public:
	MyComputer myPc;
    unsigned int delta;
    unsigned int deltaTicks;
    unsigned int deltaProgress;

        ProgressLog(unsigned int count=0);
        void onTick(const char* taskName, unsigned int count, unsigned int tick);

};

/// used as a base class to provide progress bar support
///
/// This is used for top level operations that take time (Pather, Gcoder, Slicer)
/// and need to report progress
class Progressive
{

	ProgressBar *progress;
public:

	Progressive(ProgressBar *progress = NULL)
    {
        setProgress(progress);
    }

    void setProgress(ProgressBar *progress)
    {
        this->progress = progress;
    }

protected:
    void initProgress(const char* title, unsigned int ticks)
    {
        if(progress)
        {
            progress->reset(ticks, title);
        }
    }
    void tick()
    {
        if(progress)
        {
            progress->tick();
        }
    }

};


}

#endif
