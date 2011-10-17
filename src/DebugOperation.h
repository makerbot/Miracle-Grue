/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/

#include "Operation.h"

#ifndef DEBUGOPERATION_H_
#define DEBUGOPERATION_H_

class DebugOperation : public Operation
{

public:
	DebugOperation() {};

	// -- from Operation Acceptor
	void collect(DataEnvelope& envelope);

	// -- from Operation
    void init(Configuration& config);
    void cleanup();
	DataEnvelope* processEnvelope(const DataEnvelope& envelope);
    string interrogate();
    AtomType collectsEnvelopeType();
    AtomType emitsEnvelopeType();


};

#endif /* DEBUGOPERATION_H_ */
