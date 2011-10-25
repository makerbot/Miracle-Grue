/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/
#include "Operation.h"
#include "MeshData.h"		// input data type
#include "RegionData.h"		// output data

#include <stdio.h>

#ifndef MG_SKEIN_OPERATION_H
#define MG_SKEIN_OPERATION_H

class SliceOperation :public Operation{
public:

    SliceOperation();
    ~SliceOperation();

	void processEnvelope(const DataEnvelope& envelope);
    //string interrogate();

    std::vector<AtomType> collectsEnvelopeType() = 0;
    std::vector<AtomType> emitsEnvelopeType() = 0;
};


#endif
