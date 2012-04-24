#ifndef INSET_TESTS_H_
#define INSET_TESTS_H_

#include "mgl/shrinky.h"

#include "insetTests.h"

using namespace mgl;
using namespace std;
using namespace libthing;



void clip(const std::vector<LineSegment2> &in,
			unsigned int min,
			unsigned int max,
			std::vector<LineSegment2> &out)
{
	int newSize = in.size() - (max -min);
	std::cout << "NEW SIZE " << newSize << std::endl;
	assert(newSize > 0);
	out.reserve(newSize);


	for(unsigned int i =0; i <in.size(); i++)
	{
		const LineSegment2 s = in[i];
		if(i == min-1)
		{
			i = max+1; // advance the cursor
			LineSegment2 s1 = in[i];
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
	mgl::ScadDebugFile::segment3(std::cout,"","segments", out, 0, 0.1);

};

#endif
