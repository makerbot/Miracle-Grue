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

    SliceOperation(t_callback callback):Operation(callback) {}

    ~SliceOperation() { printf("%s\n", __FUNCTION__ );}
    virtual void main(); //{ printf("__FUNCTION__");}
    virtual void queue(DataEnvelope* pAtom);// = { printf("__FUNCTION__");}
    virtual void cleanup();//{};
    virtual char* interrogate();// = {printf("__FUNCTION__");return "__FUNCTION__"}

};


#endif
