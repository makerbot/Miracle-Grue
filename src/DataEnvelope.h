/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/

#ifndef MG_DATA_ENVELOPE
#define MG_DATA_ENVELOPE

#include "stdint.h"
#include <stdio.h>
#include <vector>
#include <iostream>
#include <map>

typedef double real;

struct Point2D
{
	Point2D(real x, real y)
		:x(x), y(y)
	{}
	real x;
	real y;
};

typedef std::vector<Point2D> Polygon;
typedef std::vector<Polygon> Paths;

// for now, use cout, until we add Boost support
#define BOOST_LOG_TRIVIAL(trace) std::cout
/// boost log values (future use) are trace/debug/info/warning/error/fatal


// This enum represents known data types
//TODO: These should be 32bit id/hash values for dataNamespace strings or something
typedef enum AtomType {
	TYPE_INVALID = 0,
	TYPE_DUMMY_DATA = 1,

	TYPE_STL_ASCII = 4,
	TYPE_STL_BINARY = 5,

	TYPE_SHELL_BINARY = 6,
	TYPE_REGION_BINARY = 7,

	TYPE_PATH_BINARY = 8,
	TYPE_PATH_ASCII= 3,
	
	TYPE_MESH_3D = 9, 

	TYPE_GCODE_ASCII = 2,
} AtomType;


/**
 *  This class is a data Enveloper, to contain processing data,
 *  and metadata for the data available.
 *
 *  Rules of the Envelope:
 *  1) Each envelope must be at a minimum an entire data slice and no less.IE, one
 *  entire slice of a shell, or one entire region file.
 */
class DataEnvelope {

private:
	int useCount; ///in the future, this will use boost weak_ptr and strong_ptr or something
	std::map<void*, int> useMap;
	/*
	void* data; ///data
	uint32_t dataSize; ///size of data in bytes
	char* dataNamespaceString; /// namespace string of datatype
	//TODO: add Reference to owner which will be responsible for cleaning up
	// this envelope of data. For now, creator of atom will do the destruction.
	//TODO: build a 'id to namespace string' converter

public:

	AtomType typeID; /// id of the contained data
	bool type() {return typeID; };

	///Baseline data constructor
	DataEnvelope(AtomType iD, void* pData, uint32_t dataSz, char* dataNsString, bool isLast = false)
	: typeID(iD), data(pData), dataSize(dataSz), dataNamespaceString(dataNsString),lastFlag(isLast) {};


	DataEnvelope(): typeID(TYPE_INVALID), data(0x00),
				dataSize(0), dataNamespaceString((char*)""), lastFlag(false)
	{
		// printf("%s\n", __FUNCTION__ );
	};
*/
protected:
	bool lastFlag; ///this flags the current envelope as the last in this stream
	int streamId; ///ID for the current stream. zero if no stream is defined

	///Generic empty data constructor

public:
	DataEnvelope():useCount(1), streamId(0){}

	virtual ~DataEnvelope(){
		if (useCount > 0 )
			BOOST_LOG_TRIVIAL(trace) <<  __FUNCTION__ << " " << this << " useCount is nonzero on deletion. Fail?" << std::endl;
		else
			BOOST_LOG_TRIVIAL(trace) <<  __FUNCTION__ << " " << this << " useCount is zero on deletion! FTW" << std::endl;

	}

	bool isLastEnvelope() const { return lastFlag; }

	void setLast(void) {
		BOOST_LOG_TRIVIAL(trace) <<  __FUNCTION__ << std::endl;
		lastFlag = true;
	}

	/// increments use countage of this envelope
	void addRef(void* user = NULL) const
	{
		int* cnt = (int*)&useCount;
		std::map<void*, int>* map = const_cast<std::map<void*, int>* >(&useMap);
		(*map)[user]++;
		(*cnt)++;
	}

	/// decrements use countage of this envelope
	void release(void* user = NULL) const {
		BOOST_LOG_TRIVIAL(trace) << __FUNCTION__ << "ENTER " << useCount << std::endl;

		int* cnt = (int*)&useCount;
		(*cnt)--;
		BOOST_LOG_TRIVIAL(trace) <<  __FUNCTION__ << " new useCount " << useCount << std::endl;

		if(useCount > 0){
			BOOST_LOG_TRIVIAL(trace) <<  __FUNCTION__ << " useCount expected zero! got: " << useCount << std::endl;
			std::map<void*, int>* map = const_cast<std::map<void*, int>* >(&useMap);
			(*map)[user]--;
		}
		else if (useCount == 0){
			if( useMap.size() > 0 )
				BOOST_LOG_TRIVIAL(trace) <<  __FUNCTION__ << "Could not unpair released. Minor bug!" << std::endl;

			BOOST_LOG_TRIVIAL(trace) <<  __FUNCTION__ << "Envelope auto-free" << std::endl;
			delete this;
		}
		else if(useCount < 0){
			BOOST_LOG_TRIVIAL(trace) <<  __FUNCTION__ << std::endl;
			BOOST_LOG_TRIVIAL(trace) <<  "Use Count Blown.  Trying to decrement %d" << std::endl;
		}
		BOOST_LOG_TRIVIAL(trace) << __FUNCTION__ << "EXIT" << std::endl;
	}


};

#endif /* MG_DATA_ENVELOPE */
