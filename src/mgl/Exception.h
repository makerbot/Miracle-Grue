
#ifndef EXCEPTION_H_
#define EXCEPTION_H_ (1)

#include <string>

namespace mgl
{

/**
 * base class for all MGL Exceptions
 */
class Exception
{

public:
	std::string error;
	Exception(const char *msg) :error(msg)
	{
		//	std::cerr << std::endl << msg << std::endl;
		// fprintf(stderr, "%s", msg);
	}

};

} /*end namespace mgl; */

#endif //EXCEPTION_H_
