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

#ifdef __APPLE__
#include <CFBundle.h>
#endif 

#ifdef WIN32
#include <Windows.h>
#include <Shlobj.h>
#else
#include <sys/types.h>
#include <pwd.h>
#endif

#include <fstream>

std::ostream &MyComputer::log()
{
    return cout;
}


int FileSystemAbstractor::guarenteeDirectoryExists(const char* pathname)
{
    int status = 0;

#ifdef WIN32
        DWORD attrib = GetFileAttributesA(pathname);

        if (attrib == INVALID_FILE_ATTRIBUTES) {
            BOOL result = CreateDirectoryA(pathname, NULL);
            if (!result) 
                status = -1; //creation fail
        }
        else if (!(attrib & FILE_ATTRIBUTE_DIRECTORY)) 
            status = -1;
		return status;

#else //WIN32
        // mode_t does not work under QT
        mode_t mode =  (S_IREAD|S_IWRITE
						|S_IRWXU
						|S_IRGRP|S_IXGRP
						|S_IROTH|S_IXOTH);

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
}

int FileSystemAbstractor::guarenteeDirectoryExistsRecursive(const char* dirPath){
	/* Assume dirPath is delimited by getPathSeparatorCharacter() */
	string dirString(dirPath);
	size_t pos = 0;
	int err;
	while((pos = dirString.find(getPathSeparatorCharacter(), pos+1)) != 
			string::npos){
		string dirSub = dirString.substr(0, pos);
		err = guarenteeDirectoryExists(dirSub.c_str());
		if(err)
			return err;
	}
	return guarenteeDirectoryExists(dirPath);
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
#ifdef WIN32
    return '\\';
#else
    return '/'; // Linux & Mac, works on Windows most times
#endif
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

bool FileSystemAbstractor::fileReadable(const char *filename) const {
	ifstream testfile(filename, ifstream::in);
	bool readable = !testfile.fail();
	testfile.close();

	return readable;
}

string FileSystemAbstractor::getDataFile(const char *filename) const {
	string found = getUserDataFile(filename);
	if (found.length() > 0 && fileReadable(found.c_str()))
		return found;
	
	found = getSystemDataFile(filename);
	if (found.length() > 0 && fileReadable(found.c_str())) 
		return found;
		
	if (fileReadable(filename)) //fall back to current working directory
		return string(filename);

	return string();
}

string FileSystemAbstractor::getConfigFile(const char *filename) const {
	string found = getUserConfigFile(filename);
	if (found.length() > 0 && fileReadable(found.c_str()))
		return found;
	
	found = getSystemConfigFile(filename);
	if (found.length() > 0 && fileReadable(found.c_str())) 
		return found;
		
	return string();
}

string FileSystemAbstractor::getUserDataFile(const char *filename) const {
#ifdef __APPLE__
	char pwbuff[1024];
	struct passwd pw;
	struct passwd *tempptr;
	if (getpwuid_r(getuid(), &pw, pwbuff, 1024, &tempptr) == 0) {
	   return pathJoin(pathJoin(string(pw.pw_dir),
								 "Library/Makerbot/Miracle-Grue"),
					   filename);
	}
	else {
		return string();
	}
#else
	/*other platforms should be fine putting user data files in the same
	  dir as user config files*/	
	return getUserConfigFile(filename);
#endif
}

string FileSystemAbstractor::getSystemDataFile(const char *filename) const {
#ifdef __linux__
	return pathJoin(string("/usr/share/makerbot"), filename);
#else
	return getSystemConfigFile(filename);
#endif
}

string FileSystemAbstractor::getSystemConfigFile(const char *filename) const {
#ifdef WIN32
    char app_path[1024];

	GetModuleFileNameA(0, app_path, sizeof(app_path) - 1);

	// Extract directory
	std::string app_dir = ExtractDirectory(app_path);
	return pathJoin(app_dir, filename);

#elif defined __APPLE__
	CFBundleRef mainBundle;

	mainBundle = CFBundleGetMainBundle();
	if(!mainBundle)
		return string();

	CFURLRef resourceURL;
	resourceURL =
		CFBundleCopyResourceURL(mainBundle,
					CFStringCreateWithCString(NULL,
											  filename,
											  kCFStringEncodingASCII),

								NULL,
								NULL);

	if (!resourceURL) 
		return string();

	char fileurl[1024];
	if(!CFURLGetFileSystemRepresentation(resourceURL,
										 true,
										 (UInt8*)
										 fileurl,
										 1024)) {
		return string();
	}

	return string(fileurl);
#else //linux

	return pathJoin(string("/etc/xdg/makerbot"), filename);
#endif
}

string FileSystemAbstractor::getUserConfigFile(const char *filename) const {
#ifdef WIN32
	char app_data[1024];
	if(SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0,
								 app_data))) { 
		return pathJoin(pathJoin(string(app_data), "Makerbot"),
						filename);
	}
#else //linux or apple
	char pwbuff[1024];
	struct passwd pw;
	struct passwd *tempptr;
	if (getpwuid_r(getuid(), &pw, pwbuff, 1024, &tempptr) == 0) {

#ifdef __APPLE__
		return pathJoin(pathJoin(string(pw.pw_dir),
								 "Library/Preferences/Makerbot/Miracle-Grue"),
						filename);
#else //linux
		return pathJoin(pathJoin(string(pw.pw_dir), ".config/makerbot"),
						filename);
#endif //linux
	}

#endif //win32

	return string();
}

ProgressLog::ProgressLog(unsigned int count)
    :ProgressBar(count,"")
{
	reset(count);
	std::cout << ":";

}


void ProgressLog::onTick(const char* taskName, unsigned int count, unsigned int ticks)
{
	if (ticks == 0) {
		this->deltaTicks = 0;
		this->deltaProgress = 0;
		this->delta = count / 10;
		std::cout << taskName;
		Log::info() << " [" << deltaProgress * 10 << "%] ";
	}

	if (deltaTicks >= this->delta)
	{
		deltaProgress++;
		std::cout << " [" << deltaProgress * 10 << "%] ";
		std::cout.flush();
		Log::info() << " [" << deltaProgress * 10 << "%] ";
		this->deltaTicks = 0;

	}
	if ( ticks >= count -1  ) {

		string now = myPc.clock.now();
        Log::info() << now;
        std::cout << now << endl;
	}
	deltaTicks++;
}






