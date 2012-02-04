/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/
#include <iostream>
#include "StlEnvelope.h"
#include "../BGL/BGLMesh3d.h"

using namespace std;

StlEnvelope::StlEnvelope(BGL::Mesh3d &mesh):
		ourMesh(mesh), DataEnvelope(/*AtomType*/TYPE_BGL_MESH  )
{
	cout << "StlEnvelope() @"  << this << endl;
}

StlEnvelope::~StlEnvelope()
{
	cout << "~StlEnvelope() @"  << this << endl;
}

const BGL::Mesh3d& StlEnvelope::getMesh() const
{
	return this->ourMesh;
}




