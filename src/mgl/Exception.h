
#ifndef MGL_EXCEPTION_H_
#define MGL_EXCEPTION_H_ (1)

#include <string>
#include <stdexcept>

namespace mgl {

/**
 * base class for all MGL Exceptions
 */
class Exception : public std::runtime_error {
public:
	Exception(const char *msg) throw() : std::runtime_error(msg) {}
	Exception(const std::string& msg) throw() : std::runtime_error(msg.c_str()) {}
};

class GeometryException : public Exception {
public:
    template <typename T>
    GeometryException(const T& arg) throw() : Exception(arg) {}
};

} /*end namespace mgl; */

#endif //MGL_EXCEPTION_H_
