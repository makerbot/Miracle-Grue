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
    virtual void main();
    virtual void collect(DataEnvelope& dataE);
    virtual void cleanup();
    virtual std::string interrogate();

    virtual uint32_t acceptsEnvelopeType();
    virtual uint32_t  yieldsEnvelopeType();

};


#endif
