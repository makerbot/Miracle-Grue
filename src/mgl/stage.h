/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/

#ifndef STAGE_H_
#define STAGE_H_

#include <string>
#include <deque>
#include <vector>

namespace mgl {

/**
 * Base class for the unit of data passed between stages.
 */
class DataBlock {
public:
	DataBlock();

	virtual void dump(const std::string &path) {};
	virtual ~DataBlock() {};
};

/**
 * Simple DataBlock that contains one templated value.  Does not implement dump.
 */
template <class T>
class SimpleDataBlock : public DataBlock {
public:
	SimpleDataBlock(T newval): val(newval) {};
	T getVal() {return val;};

private:
	T val;
};


/**
 * One stage of a data pipeline
 */
class Stage {
public:
	Stage(const std::string &newname);

	virtual void work();

	bool dataReady();

	virtual bool isSource() { return false; };
	virtual bool isSink() { return false; };
	std::string& getName() { return name; };

	virtual ~Stage() {};

protected:
	virtual void doWork();
	std::string name;
};

typedef std::deque<DataBlock*> DataQueue;

typedef std::vector<DataBlock*> DataList;

class Sink : public Stage {
public:
	Sink(const std::string &newname) : Stage(newname) {};
	virtual bool isSink() { return true; }
	void addNewData(DataBlock *input);
	bool hasNewData() { return !in.empty(); };
	virtual void work();

protected:
	DataBlock* consume();

private:
	DataQueue in;
	DataList working;
};


class Source : public Stage {
public:
	Source(const std::string &newname) : Stage(newname) {};
	DataBlock* getFinishedData();
	bool hasFinishedData() { return !out.empty(); };

	virtual bool isSource() { return true; }

protected:
	void produce(DataBlock *made);

private:
	DataQueue out;
};

class Transform : public Sink, public Source {
	//nothing here, just make it so we don't have to multiply inherit everywhere
};

}

#endif
