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

#include "Operation.h"


class PatherOperation: public Operation
{
    Configuration* pConfig;

public:
	PatherOperation()
		:pConfig(NULL)
	{
		std::cout << __FUNCTION__ << std::endl;
		std::cout << "(Miracle Grue)" << std::endl;
	};
	virtual ~PatherOperation(){cleanup();}

	void init(Configuration& config);
	void start();
	DataEnvelope* processEnvelope(const DataEnvelope& envelope);
	void cleanup();
/*
	std::string interrogate();
	AtomType collectsEnvelopeType();
	AtomType  emitsEnvelopeType();
*/
	const Configuration &configuration()const {return *pConfig;}



};

#endif /* PatherOperation_H_ */

