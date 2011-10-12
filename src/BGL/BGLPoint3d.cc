#include "BGLPoint.h"

namespace BGL {

    ostream& operator <<(ostream &os,const Point3d &pt)
    {
	os.precision(2);
	os.setf(ios::fixed);
	os << "(" << pt.x << ", " << pt.y << ", " << pt.z << ")";
	return os;
    }


}


