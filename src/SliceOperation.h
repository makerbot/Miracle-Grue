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

    SliceOperation(callback_t callback):Operation(callback) {}

    ~SliceOperation() { printf("%s\n", __FUNCTION__ );}
    virtual void main();
    virtual void queue(DataEnvelope &pAtom);
    virtual void cleanup();
    virtual std::string interrogate();

    void setYielder(callback_t  callback);


    virtual uint32_t acceptsEnvelopeType();
    virtual uint32_t  yieldsEnvelopeType();

};


#endif
