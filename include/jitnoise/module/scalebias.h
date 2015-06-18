#ifndef NOISE_MODULE_SCALEBIAS_H
#define NOISE_MODULE_SCALEBIAS_H

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

/// Default bias for the noise::module::ScaleBias noise module.
const double DEFAULT_BIAS = 0.0;

/// Default scale for the noise::module::ScaleBias noise module.
const double DEFAULT_SCALE = 1.0;

/// Noise module that applies a scaling factor and a bias to the output
/// value from a source module.
///
/// @image html modulescalebias.png
///
/// The GetValue() method retrieves the output value from the source
/// module, multiplies it with a scaling factor, adds a bias to it, then
/// outputs the value.
///
/// This noise module requires one source module.
class ScaleBias: public Module
{

public:

    /// Constructor.
    ///
    /// The default bias is set to noise::module::DEFAULT_BIAS.
    ///
    /// The default scaling factor is set to noise::module::DEFAULT_SCALE.
    ScaleBias ();

    /// Returns the bias to apply to the scaled output value from the
    /// source module.
    ///
    /// @returns The bias to apply.
    ///
    /// The GetValue() method retrieves the output value from the source
    /// module, multiplies it with the scaling factor, adds the bias to
    /// it, then outputs the value.
    double GetBias () const
    {
        return m_bias;
    }

    /// Returns the scaling factor to apply to the output value from the
    /// source module.
    ///
    /// @returns The scaling factor to apply.
    ///
    /// The GetValue() method retrieves the output value from the source
    /// module, multiplies it with the scaling factor, adds the bias to
    /// it, then outputs the value.
    double GetScale () const
    {
        return m_scale;
    }

    virtual int GetSourceModuleCount () const
    {
        return 1;
    }

    /// Sets the bias to apply to the scaled output value from the source
    /// module.
    ///
    /// @param bias The bias to apply.
    ///
    /// The GetValue() method retrieves the output value from the source
    /// module, multiplies it with the scaling factor, adds the bias to
    /// it, then outputs the value.
    void SetBias (double bias)
    {
        m_bias = bias;
    }

    /// Sets the scaling factor to apply to the output value from the
    /// source module.
    ///
    /// @param scale The scaling factor to apply.
    ///
    /// The GetValue() method retrieves the output value from the source
    /// module, multiplies it with the scaling factor, adds the bias to
    /// it, then outputs the value.
    void SetScale (double scale)
    {
        m_scale = scale;
    }

protected:

    /// Bias to apply to the scaled output value from the source module.
    double m_bias;

    /// Scaling factor to apply to the output value from the source
    /// module.
    double m_scale;

};

/// @}

/// @}

/// @}

}

}

#endif
