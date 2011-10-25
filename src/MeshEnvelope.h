/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/

#ifndef MG_MESH_ENVELOPE
#define MG_MESH_ENVELOPE

#include "stdint.h"
#include BGL/BGLMesh3d.h

/**
 * Class for containing and passing a 3d Mesh and related metadata
 */ 
class MeshEnvelope : public DataEnvelope
{
protected:
	BGLMesh3d& mesh3d;
	
	MeshEnvelope(BGLMesh3d& mesh3d): DataEnvelope()
	{
		this->mesh3d = mesh3d;
		this->typeID = TYPE_MESH_3D; 
	}

	

};

#endif /*MG_MESH_ENVELOPE*/
