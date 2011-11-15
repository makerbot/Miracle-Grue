//
//  BGLSVG.h
//  Part of the Belfry Geometry Library
//
//  Created by GM on 10/13/10.
//  Copyright 2010 Belfry Software. All rights reserved.
//

#ifndef BGL_SVG_H
#define BGL_SVG_H

#include <iostream>
#include "config.h"

using namespace std;

namespace BGL {


class SVG {
public:
    // member variables
    double width, height;
    
    // Constructors
    SVG(double w, double h) : width(w), height(h) {}

    ostream &header(ostream &os) const;
    ostream &footer(ostream& os) const;
};



}

#endif

