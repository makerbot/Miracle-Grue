/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/

#include "pipeline.h"
#include "Exception.h"
#include "log.h"

namespace mgl {

void Pipeline::addStage(Stage *newstage) {
	if (stages.empty() && (!newstage->isSource()))
		throw Exception("First stage in a pipeline needs to be a source");

	stages.push_back(newstage);
}
//
void Pipeline::run() {

	bool allgood = true;
	Source *last = NULL;

	for (StageList::iterator i = stages.begin();
		 allgood && i != stages.end(); i++) {
		Stage *stage = *i;
		try {
			if (stage->isSource() && !stage->isSink())
				stage->work(); //Sources don't have an input queue to exhaust
			else {
				Sink *cur = (Sink*)*i;
				if (last) {
					while (last->hasFinishedData()) {
						DataBlock* finished = last->getFinishedData();
						finished->dump(dump_path);

						cur->addNewData(finished);
					}
				}

				while (cur->hasNewData()) {
					cur->work();
				}
			}
		}
		catch (PipeAbortException &abort) {
			allgood = false;
			 Log::error() << "Pipeline aborted at stage: " << stage->getName();
			Log::error() << ": " << abort.error << std::endl;
		}
		catch (PipeSkipException &skip) {
			Log::error() << "Pipeline skipped work unit at stage: " << stage->getName();
			Log::error() << ": " << skip.error << std::endl;
		}
		catch (std::exception &stdexp) {
			Log::error() << "Pipeline threw STL exception at stage: " << stage->getName();
			Log::error() << ": " << stdexp.what() << std::endl;
		}
		catch (Exception &exp) {
			Log::error() << "Pipeline threw mgl Exception at stage: " << stage->getName();
			Log::error() << ": " << exp.error << std::endl;
		}
		catch (...) {
			allgood = false;
			Log::error() << "Unknown exception at stage: " << stage->getName();
		}

		last = (Source*)*i;
	}
}

Pipeline::~Pipeline() {
	for(StageList::const_iterator i = stages.begin(); i!= stages.end(); i++) {
		delete *i;
	}
}

}
