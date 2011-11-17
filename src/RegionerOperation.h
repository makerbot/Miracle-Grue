/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/
#ifndef RegionerOperation_H_
#define RegionerOperation_H_

#include "SliceData.h"  // input data type
#include "RegionData.h"	 // output data type
#include "Operation.h"


class RegionerOperation: public Operation
{

public:
	RegionerOperation();

	virtual ~RegionerOperation();
	void processEnvelope(const DataEnvelope& envelope);
	virtual void init(Configuration&, const std::vector<Operation*>&outputs);
	virtual void start(){}
	virtual void finish(){}
	virtual void deinit(){}
	virtual bool isValidConfig(Configuration&) const{return true;}

};


#endif /* RegionerOperation_H_ */

