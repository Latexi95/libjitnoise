#ifndef NOISE_MODULE_ABS_H
#define NOISE_MODULE_ABS_H

#include "modulebase.h"

namespace noise
{

namespace module {

/// @addtogroup jitnoise
/// @{

/// @addtogroup modules
/// @{

/// @defgroup modifiermodules Modifier Modules
/// @addtogroup modifiermodules
/// @{

/// Noise module that outputs the absolute value of the output value from
/// a source module.
///
/// @image html moduleabs.png
///
/// This noise module requires one source module.
class Abs: public Module
{

public:

    /// Constructor.
    Abs ();

    virtual int GetSourceModuleCount () const
    {
        return 1;
    }

};

/// @}

/// @}

/// @}

}

}

#endif
