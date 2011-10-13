/* LICENSE
 *
 */
#include "stdint.h"

#ifndef MG_DATA_ENVELOPE
#define MG_DATA_ENVELOPE
typedef enum AtomType {
	TYPE_INVALID = 0,
	TYPE_DUMMY_DATA = 1,
	//TODO: These should be 32bit id/hash values for dataNamespace strings or something
} AtomType;


/**
 *  This class contains a minimum collection of processable data
 *  (an Atom of Data) as well as metadata about the format of that
 *  atom, what else it contains and uses, and other data context info
 */
class DataEnvelope {

public:
	AtomType typeID; /// id of the contained data
	void* data; /// prt to the data namespace string of datatype
	uint32_t dataSize; ///size of data in bytes
	char* dataNamespaceString; /// namespace string of datatype

	DataEnvelope(): typeID(TYPE_INVALID), data(0x00),
				dataSize(0), dataNamespaceString((char*)"") {};
	DataEnvelope(AtomType iD, void* pData, uint32_t dataSz, char* dataNsString)
	: typeID(iD), data(pData), dataSize(dataSz), dataNamespaceString(dataNsString) {};
};

#endif /* MG_DATA_ENVELOPE */
