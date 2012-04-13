/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/

#include "stage.h"

namespace mgl
{

Stage::Stage(const std::string &newname) : name(newname){
	//placeholder, not sure what goes here yet
}

void Sink::addNewData(DataBlock *input) {
	in.push_back(input);
}

DataBlock* Source::getFinishedData() {
	DataBlock *output = out.front();
	out.pop_front();

	return output;
}

DataBlock* Sink::consume() {
	DataBlock *block = in.front();
	in.pop_front();
	working.push_back(block);
	return block;
}

void Source::produce(DataBlock *made) {
	out.push_back(made);
}

void Stage::work() {
	doWork();
}

void Sink::work() {
	Stage::work();

	for (DataList::const_iterator i = working.begin();
		 i != working.end();
		 i++) {
		DataBlock *dead = *i;
		delete dead;
	}

	working.clear();
}

}
