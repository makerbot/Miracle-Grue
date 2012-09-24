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

class Segmenter {
public:
	Segmenter(Scalar firstSliceZ, Scalar layerH);
	const SliceTable& readSliceTable() const;
	const LayerMeasure& readLayerMeasure() const;
	const std::vector<TriangleType>& readAllTriangles() const;
	const Limits& readLimits() const;
	void tablaturize(const Meshy& mesh);
private:
	void updateSlicesTriangle(size_t newTriangleId);	
	
	SliceTable sliceTable;
	LayerMeasure zTapeMeasure;
	
	std::vector<TriangleType> allTriangles;
	Limits limits;
};

}

#endif	/* SEGMENTER_H */

