/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/
#ifndef PATHEROPERATION_H_
#define PATHEROPERATION_H_

#include "RegionData.h"  // input data type
#include "PathData.h"	 // output data type
#include "Operation.h"


class PatherOperation: public Operation
{

public:
	PatherOperation()
	{
		std::cout << __FUNCTION__ << std::endl;
		std::cout << "(Miracle Grue)" << std::endl;
		std::cout << "PatherOperation() @"  << this<< std::endl;
	};

	virtual ~PatherOperation()
	{
		std::cout << "~PatherOperation() @"  << this<< std::endl;
	}


	void processEnvelope(const DataEnvelope& envelope);



};

#endif /* PatherOperation_H_ */

