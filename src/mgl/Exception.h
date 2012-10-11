
#ifndef MGL_EXCEPTION_H_
#define MGL_EXCEPTION_H_ (1)

#include <string>

namespace mgl {

/**
 * base class for all MGL Exceptions
 */
class Exception : public std::exception {
public:
	std::string error;

	Exception(const char *msg) throw() : error(msg) {
		//	std::cerr << std::endl << msg << std::endl;
		// fprintf(stderr, "%s", msg);
	}
	Exception(const std::string& msg) throw() : error(msg) {}
	const char* what() const throw() {
		return error.c_str();
	}
	~Exception() throw() {}
};

class GeometryException : public mgl::Exception {
public:
    template <typename T>
    GeometryException(const T& arg) : Exception(arg) {}
};

} /*end namespace mgl; */

#endif //MGL_EXCEPTION_H_
