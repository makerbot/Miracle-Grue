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

protected:
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
	bool lastFlag; ///this flags the current envelope as the last in this stream

	///Generic empty data constructor

public:
	DataEnvelope(){}

	virtual ~DataEnvelope(){}

	bool isLastEnvelope() const
	{
		return lastFlag;
	}

	void setLast(void)
	{
		printf("%s\n", __FUNCTION__ );
		lastFlag = true;
	}



};

#endif /* MG_DATA_ENVELOPE */
