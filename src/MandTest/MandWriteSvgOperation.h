/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/
#ifndef MAND_WRITE_SVG_OPERATION_H_
#define MAND_WRITE_SVG_OPERATION_H_

#include "../Operation.h"

#include <iostream>
#include <fstream>

#include <string>
#include <assert.h>

class MandWriteSvgOperation : public Operation
{
	std::ofstream *pStream;
protected:
	bool isValidConfig(Configuration& config) const ;

public:
	MandWriteSvgOperation();
	~MandWriteSvgOperation();

	static Json::Value* getStaticConfigRequirements();


	void processEnvelope(const DataEnvelope& envelope);

//	void start();
	void init(Configuration& config,const std::vector<Operation*> &outputs);

	// closes the file
	void deinit();

	void start();

	void finish();

	// file handle accessor
	std::ostream& stream() const;

};




#endif /* MAND_WRITE_SVG_OPERATION_H_ */

