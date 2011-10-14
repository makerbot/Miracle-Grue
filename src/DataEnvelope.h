/* LICENSE
 *
 */
#include "stdint.h"
#include <stdio.h>


#ifndef MG_DATA_ENVELOPE
#define MG_DATA_ENVELOPE

// Remove this and use typing of the envelope for the management of
// what type we want to use
typedef enum AtomType {
	TYPE_INVALID = 0,
	TYPE_DUMMY_DATA = 1,
	TYPE_ASCII_GCODE = 2,
	TYPE_ASCII_PATHER = 3,
	//TODO: These should be 32bit id/hash values for dataNamespace strings or something
} AtomType;


/**
 *  This class contains a minimum collection of processable data
 *  (an Atom of Data) as well as metadata about the format of that
 *  atom, what else it contains and uses, and other data context info
 */
class DataEnvelope {

				// the creator is creating.

public:
	bool lastFlag; //this envelope is the last envelope of data
	AtomType typeID; /// id of the contained data
	void* data; /// prt to the data namespace string of datatype
	uint32_t dataSize; ///size of data in bytes
	char* dataNamespaceString; /// namespace string of datatype
	//TODO: add Reference to owner which will be
	// responsible for cleaning up this envelope of data. For now, creator of atom
	// will do the destruction.


	DataEnvelope(): typeID(TYPE_INVALID), data(0x00),
				dataSize(0), dataNamespaceString((char*)""), lastFlag(false)
	{
		printf("%s\n", __FUNCTION__ );
	};

	DataEnvelope(AtomType iD, void* pData, uint32_t dataSz, char* dataNsString)
	: typeID(iD), data(pData), dataSize(dataSz), dataNamespaceString(dataNsString),lastFlag(false) {};
	void setLast(void)
	{
		printf("%s\n", __FUNCTION__ );
		lastFlag = true;
	}
};

#endif /* MG_DATA_ENVELOPE */
