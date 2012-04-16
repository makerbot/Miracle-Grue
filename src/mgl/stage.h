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
	DataBlock() {};

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

	virtual ~SimpleDataBlock() {};
private:
	T val;
};

template <class T>
class PtrDataBlock : public SimpleDataBlock<T*> {
public:
	PtrDataBlock(T* newval): SimpleDataBlock<T*>(newval) {};
	T* getVal() { return SimpleDataBlock<T*>::getVal(); }
	virtual ~PtrDataBlock() { delete getVal(); }
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
	virtual void doWork() = 0;
	std::string name;
};

typedef std::deque<DataBlock*> DataQueue;

typedef std::vector<DataBlock*> DataList;

// implementation class to avoid diamond inheritance
class SinkImpl {
public:
	void addNewData(DataBlock *input);
	bool hasNewData() { return !in.empty(); };
	virtual void workImpl();

protected:
	DataBlock* consume();

private:
	DataQueue in;
	DataList working;
};



class Sink : public Stage, public SinkImpl {
public:
	Sink(const std::string &newname) : Stage(newname) {};
	virtual bool isSink() { return true; }
	virtual void work() { Stage::work(); SinkImpl::workImpl(); }

};

class SourceImpl {
public:
	DataBlock* getFinishedData();
	bool hasFinishedData() { return !out.empty(); };

protected:
	void produce(DataBlock *made);

private:
	DataQueue out;
};

class Source : public Stage, public SourceImpl {
public:
	Source(const std::string &newname) : Stage(newname) {};
	virtual bool isSource() { return true; }

};

class Transform : public Stage, public SourceImpl, public SinkImpl {
public:
	Transform(const std::string &newname) : Stage(newname) {};
	virtual void work() { Stage::work(); SinkImpl::workImpl(); }
};

}

#endif
