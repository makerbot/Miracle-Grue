/* TODO: License/Metadata Insert
// some concepts by XXX of Mandoline
 *
 */
#include "Operation.h"
#include "DataEnvelope.h"
#include <stdio.h>

#ifndef MG_SKEIN_OPERATION_H
#define MG_SKEIN_OPERATION_H

class SliceOperation :public Operation{
public:

    SliceOperation():Operation() {}

    ~SliceOperation() { printf("%s\n", __FUNCTION__ );}


    void init(Configuration& config);
    void cleanup();
	DataEnvelope* processEnvelope(const DataEnvelope& envelope);
    string interrogate();
    AtomType collectsEnvelopeType();
    AtomType emitsEnvelopeType();
};


#endif
