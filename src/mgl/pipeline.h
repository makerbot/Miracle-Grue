/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/

#ifndef PIPELINE_H_
#define PIPELINE_H_

#include "stage.h"
#include <vector>
#include "Exception.h"

namespace mgl {

typedef std::vector<Stage> StageList;

/**
 * Simple array based execution pipeline
 */

class Pipeline {
public:
	Pipeline(const std::string &path) : dump_path(path) {};
	Pipeline() : dump_path("") {};

	void addStage(Stage &newstage);
	void run();

	~Pipeline();
private:
	StageList stages;
	std::string dump_path;

};

class PipeAbortException : public Exception {
	PipeAbortException(const char *msg) : Exception(msg) {};
};
class PipeSkipException : public Exception {
	PipeSkipException(const char *msg) : Exception(msg) {};
};
class PipeSetupException : public Exception {
	PipeSetupException(const char *msg) : Exception(msg) {};
};

}

#endif
