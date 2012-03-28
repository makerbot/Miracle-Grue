
#include "Meshy.h"

using namespace mgl;
using namespace std;

	/// requires firstLayerSlice height, and general layer height
Meshy::Meshy(Scalar firstSliceZ, Scalar layerH)
		:zTapeMeasure(firstSliceZ, layerH)
	{ 	}

const std::vector<Triangle3>& Meshy::readAllTriangles() const
{
	return allTriangles;
}

const Limits& Meshy::readLimits() const
{
	return limits;
}

const LayerMeasure& Meshy::readLayerMeasure() const
{
	return zTapeMeasure;
}

const SliceTable& Meshy::readSliceTable() const
{
	return sliceTable;
}


//
// Adds a triangle to the global array and for each slice of interest
//
void Meshy::addTriangle(Triangle3 &t)
{

	Vector3 a, b, c;
	t.zSort(a,b,c);

	unsigned int minSliceIndex = this->zTapeMeasure.zToLayerAbove(a.z);
	if(minSliceIndex > 0)
		minSliceIndex --;

	unsigned int maxSliceIndex = this->zTapeMeasure.zToLayerAbove(c.z);
	if (maxSliceIndex - minSliceIndex > 1)
		maxSliceIndex --;

//		std::cout << "Min max index = [" <<  minSliceIndex << ", "<< maxSliceIndex << "]"<< std::endl;
//		std::cout << "Max index =" <<  maxSliceIndex << std::endl;
	unsigned int currentSliceCount = sliceTable.size();
	if (maxSliceIndex >= currentSliceCount)
	{
		unsigned int newSize = maxSliceIndex+1;
		sliceTable.resize(newSize); // make room for potentially new slices
//			std::cout << "- new slice count: " << sliceTable.size() << std::endl;
	}

	allTriangles.push_back(t);

	size_t newTriangleId = allTriangles.size() -1;

//		 std::cout << "adding triangle " << newTriangleId << " to layer " << minSliceIndex  << " to " << maxSliceIndex << std::endl;
	for (size_t i= minSliceIndex; i<= maxSliceIndex; i++)
	{
		TriangleIndices &trianglesForSlice = sliceTable[i];
		trianglesForSlice.push_back(newTriangleId);
//			std::cout << "   !adding triangle " << newTriangleId << " to layer " << i  << " (size = " << trianglesForSlice.size() << ")" << std::endl;
	}

	limits.grow(t[0]);
	limits.grow(t[1]);
	limits.grow(t[2]);


}


void Meshy::dump(std::ostream &out)
{
	out << "dumping " << this << std::endl;
	out << "Nb of triangles: " << allTriangles.size() << std::endl;
	size_t sliceCount = sliceTable.size();

	out << "triangles per slice: (" << sliceCount << " slices)" << std::endl;
	for (size_t i= 0; i< sliceCount; i++)
	{
		TriangleIndices &trianglesForSlice = sliceTable[i];
		//trianglesForSlice.push_back(newTriangleId);
		out << "  slice " << i << " size: " << trianglesForSlice.size() << std::endl;
		//cout << "adding triangle " << newTriangleId << " to layer " << i << std::endl;
	}
}



size_t Meshy::triangleCount() {
	return allTriangles.size();
	std::cout << "all triangle count" << allTriangles.size();
}

void Meshy::writeStlFile(const char* fileName) const
{
	StlWriter out;
	out.open(fileName);
	size_t triCount = allTriangles.size();
	for (size_t i= 0; i < triCount; i++)
	{
		const Triangle3 &t = allTriangles[i];
		out.writeTriangle(t);
	}
	out.close();
	// cout << fileName << " written!"<< std::endl;

}

void Meshy::writeStlFileForLayer(unsigned int layerIndex, const char* fileName) const
{

	StlWriter out;
	out.open(fileName);

	const TriangleIndices &trianglesForSlice = sliceTable[layerIndex];
	for(std::vector<index_t>::const_iterator i = trianglesForSlice.begin(); i!= trianglesForSlice.end(); i++)
	{
		index_t index = *i;
		const Triangle3 &t = allTriangles[index];
		out.writeTriangle(t);
	}
	out.close();
	// cout << fileName << " written!"<< std::endl;
}
