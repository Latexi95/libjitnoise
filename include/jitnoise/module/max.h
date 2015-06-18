
#ifndef NOISE_MODULE_MAX_H
#define NOISE_MODULE_MAX_H

#include "modulebase.h"

namespace noise
{

namespace module
{

/// @addtogroup jitnoise
/// @{

/// @addtogroup modules
/// @{

/// @addtogroup combinermodules
/// @{

/// Noise module that outputs the larger of the two output values from two
/// source modules.
///
/// @image html modulemax.png
///
/// This noise module requires two source modules.
class Max: public Module
{

public:

    /// Constructor.
    Max ();

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
