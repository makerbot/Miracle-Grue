#include "BGLPoint.h"

namespace BGL {

ostream& operator <<(ostream &os,const Point &pt)
{
    os.precision(3);
    os.setf(ios::fixed);
    os << "(" << pt.x << ", " << pt.y << ")";
    return os;
}

}


