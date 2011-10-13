/* TODO: License/Metadata Insert
// some concepts by XXX of Mandoline
 *
 */
#include "DataEnvelope.h"
#ifndef MG_OPERATION_H
#define MG_OPERATION_H

typedef DataEnvelope* (*t_callback)(DataEnvelope*);

class Operation {

public:
    bool isCancelled;
	char* description;
	t_callback yeildToCallback;

    Operation(t_callback callback) : isCancelled(false), description(0x00),
    		yeildToCallback(callback){}

    virtual ~Operation() {};
    virtual void main() = 0;
    virtual void queue(DataEnvelope* data) = 0;
    virtual void cleanup() = 0;
    virtual char* interrogate() = 0;
};

#endif

