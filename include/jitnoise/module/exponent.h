
#ifndef NOISE_MODULE_EXPONENT_H
#define NOISE_MODULE_EXPONENT_H

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

/// Default exponent for the noise::module::Exponent noise module.
const double DEFAULT_EXPONENT = 1.0;

/// Noise module that maps the output value from a source module onto an
/// exponential curve.
///
/// @image html moduleexponent.png
///
/// Because most noise modules will output values that range from -1.0 to
/// +1.0, this noise module first normalizes this output value (the range
/// becomes 0.0 to 1.0), maps that value onto an exponential curve, then
/// rescales that value back to the original range.
///
/// This noise module requires one source module.
class Exponent: public Module
{

public:

    /// Constructor.
    ///
    /// The default exponent is set to noise::module::DEFAULT_EXPONENT.
    Exponent ();

    /// Returns the exponent value to apply to the output value from the
    /// source module.
    ///
    /// @returns The exponent value.
    ///
    /// Because most noise modules will output values that range from -1.0
    /// to +1.0, this noise module first normalizes this output value (the
    /// range becomes 0.0 to 1.0), maps that value onto an exponential
    /// curve, then rescales that value back to the original range.
    double GetExponent () const
    {
        return m_exponent;
    }

    virtual int GetSourceModuleCount () const
    {
        return 1;
    }

    /// Sets the exponent value to apply to the output value from the
    /// source module.
    ///
    /// @param exponent The exponent value.
    ///
    /// Because most noise modules will output values that range from -1.0
    /// to +1.0, this noise module first normalizes this output value (the
    /// range becomes 0.0 to 1.0), maps that value onto an exponential
    /// curve, then rescales that value back to the original range.
    void SetExponent (double exponent)
    {
        m_exponent = exponent;
    }

protected:

    /// Exponent to apply to the output value from the source module.
    double m_exponent;

};

/// @}

/// @}

/// @}

}

}

#endif
