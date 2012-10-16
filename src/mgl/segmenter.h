/* 
 * File:   segmenter.h
 * Author: Dev
 *
 * Created on June 19, 2012, 2:05 PM
 */

#ifndef SEGMENTER_H
#define	SEGMENTER_H

#include "segment.h"
#include "obj_limits.h"
#include "abstractable.h"
#include "mgl.h"
#include "meshy.h"

namespace mgl{

class GrueConfig;

class Segmenter {
public:
    Segmenter(const GrueConfig& config);
	const SliceTable& readSliceTable() const;
	const LayerMeasure& readLayerMeasure() const;
	const std::vector<libthing::Triangle3>& readAllTriangles() const;
	const Limits& readLimits() const;
	void tablaturize(const Meshy& mesh);
private:
	void updateSlicesTriangle(size_t newTriangleId);	
	
	SliceTable sliceTable;
	LayerMeasure zTapeMeasure;
	
	std::vector<libthing::Triangle3> allTriangles;
	Limits limits;
};

}

#endif	/* SEGMENTER_H */

