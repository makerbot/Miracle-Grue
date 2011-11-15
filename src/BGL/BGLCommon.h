//
//  BGLCommon.h
//  Part of the Belfry Geometry Library
//
//  Created by GM on 10/13/10.
//  Copyright 2010 Belfry Software. All rights reserved.
//

#ifndef BGL_COMMON_H
#define BGL_COMMON_H

#include "config.h"

namespace BGL {


extern float EPSILON;
extern float CLOSEENOUGH;

typedef enum {
    USED = 0,
    OUTSIDE = 1,
    INSIDE = 2,
    SHARED = 4,
    UNSHARED = 8
} Relation;


typedef enum {
    VALID = 0,
    INVALID = 1,
    CONSECUTIVELY_INVALID = 2
} Validity;

}
#endif

