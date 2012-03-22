#ifndef SCALAR_H_
#define SCALAR_H_ (1)

#include <cmath>

//////////
// Scalar: Our basic numerical type. double for now;
///////////
typedef double Scalar;
#define SCALAR_SQRT(s) std::sqrt(s)
#define SCALAR_ABS(s) std::abs(s)
#define SCALAR_ACOS(s) std::acos(s)
#define SCALAR_SIN(s) std::sin(s)
#define SCALAR_COS(s) std::cos(s)

// See float.h for details on these
#define SCALAR_EPSILON DBL_EPSILON

namespace mgl {
/** (t)olerance (equals)
 * @returns true if two Scalar values are approximately the same using tolerance
 */
	bool tequals(Scalar a, Scalar b, Scalar tol);
}
#endif /*SCALAR_H_ */
