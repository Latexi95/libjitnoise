#ifndef NOISE_MODULE_POWER_H
#define NOISE_MODULE_POWER_H

#include "modulebase.h"

namespace noise
{

namespace module
{

/// @addtogroup jitnoise
/// @{

/// @addtogroup modules
/// @{

/// @defgroup combinermodules Combiner Modules
/// @addtogroup combinermodules
/// @{

/// Noise module that raises the output value from a first source module
/// to the power of the output value from a second source module.
///
/// @image html modulepower.png
///
/// The first source module must have an index value of 0.
///
/// The second source module must have an index value of 1.
///
/// This noise module requires two source modules.
class Power: public Module
{

public:

    /// Constructor.
    Power ();

    virtual int GetSourceModuleCount () const
    {
        return 2;
    }

};

/// @}

/// @}

/// @}

}

}

#endif
