/* 
 * File:   loop_path_point_type.h
 * Author: Dev
 *
 * Created on July 17, 2012, 1:33 PM
 */

#ifndef LOOP_PATH_POINT_TYPE_H
#define	LOOP_PATH_POINT_TYPE_H

#include "libthing/Vector2.h"

namespace mgl {

typedef libthing::Vector2 PointType;

class PointTypeSuspended : public PointType {
public:
	
	static const bool SUSPENDED_DEFAULT = false;
	
	bool suspended;
	PointTypeSuspended(Scalar nx = 0.0, 
			Scalar ny = 0.0, 
			bool susp = false) 
			: PointType(), suspended(SUSPENDED_DEFAULT) {}
	PointTypeSuspended(const PointTypeSuspended& other) 
			: PointType(other), 
			suspended(other.suspended) {}
	PointTypeSuspended(const PointType& other) 
			: PointType(other), suspended(SUSPENDED_DEFAULT) {}
	PointTypeSuspended& operator=(const PointTypeSuspended& other) {
		x = other.x;
		y = other.y;
		suspended = other.suspended;
		return *this;
	}
};


}



#endif	/* LOOP_PATH_POINT_TYPE_H */

