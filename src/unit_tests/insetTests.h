#ifndef INSET_TESTS_H_
#define INSET_TESTS_H_


#include "mgl/shrinky.h"


void clip(const std::vector<mgl::LineSegment2d> &in,
			unsigned int min,
			unsigned int max,
			std::vector<mgl::LineSegment2d> &out)
{
	int newSize = in.size() - (max -min);
	std::cout << "NEW SIZE " << newSize << std::endl;
	assert(newSize > 0);
	out.reserve(newSize);


	for(unsigned int i =0; i <in.size(); i++)
	{
		const mgl::LineSegment2d s = in[i];
		if(i == min-1)
		{
			i = max+1;
			mgl::LineSegment2d s1 = in[i];
			s1.a = s.b;
			out.push_back(s);
			out.push_back(s1);
		}
		else
		{
			out.push_back(s);
		}

	}

	std::cout << "clipped segments" << std::endl;
	mgl::ScadTubeFile::segment3(std::cout,"","segments", out, 0, 0.1);

}

#endif
