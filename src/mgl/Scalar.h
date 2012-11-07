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

#define SCALAR_MAX std::numeric_limits<double>::max()
#define SCALAR_MIN -SCALAR_MAX

namespace libthing {
/** (t)olerance (equals)
 * @returns true if two Scalar values are approximately the same using tolerance
 */
	bool tequals(Scalar const& a, Scalar const& b, Scalar const& tol);

/** (t)olerance (lower)
 * @returns true if a is less than b using tolerance
 */
	bool tlower(Scalar const& a, Scalar const& b, Scalar const& tol);
}

/// typdef to indicate index behavior/use of unsigned int
typedef unsigned int index_t;


#endif /*SCALAR_H_ */
