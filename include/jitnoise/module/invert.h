

#ifndef NOISE_MODULE_INVERT_H
#define NOISE_MODULE_INVERT_H

#include "modulebase.h"

namespace noise
{

namespace module
{

/// @addtogroup jitnoise
/// @{

/// @addtogroup modules
/// @{

/// @addtogroup modifiermodules
/// @{

/// Noise module that inverts the output value from a source module.
///
/// @image html moduleinvert.png
///
/// This noise module requires one source module.
class Invert: public Module
{

public:

    /// Constructor.
    Invert ();

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
