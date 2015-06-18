#ifndef NOISE_MATHCONSTS_H
#define NOISE_MATHCONSTS_H

// For whatever reason, I can't find the basic math consts in the MSVC version
// of math.h.

namespace noise
{

/// @addtogroup libnoise
/// @{

/// Pi.
const double PI = 3.1415926535897932385;

/// Square root of 2.
const double SQRT_2 = 1.4142135623730950488;

/// Square root of 3.
const double SQRT_3 = 1.7320508075688772935;

/// Converts an angle from degrees to radians.
const double DEG_TO_RAD = PI / 180.0;

/// Converts an angle from radians to degrees.
const double RAD_TO_DEG = 1.0 / DEG_TO_RAD;

/// @}

}

#endif


