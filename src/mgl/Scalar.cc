#include "Scalar.h"

namespace mgl {

/** (t)olerance (equals)
 * @returns true if two Scalar values are approximately the same using tolerance
 */
bool tequals(Scalar a, Scalar b, Scalar tol)
{
	return SCALAR_ABS(a-b) < tol;
}

};
