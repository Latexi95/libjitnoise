#ifndef NOISE_MODULE_CLAMP_H
#define NOISE_MODULE_CLAMP_H

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

/// Default lower bound of the clamping range for the noise::module::Clamp
/// noise module.
const double DEFAULT_CLAMP_LOWER_BOUND = -1.0;

/// Default upper bound of the clamping range for the noise::module::Clamp
/// noise module.
const double DEFAULT_CLAMP_UPPER_BOUND = 1.0;

/// Noise module that clamps the output value from a source module to a
/// range of values.
///
/// @image html moduleclamp.png
///
/// The range of values in which to clamp the output value is called the
/// <i>clamping range</i>.
///
/// If the output value from the source module is less than the lower
/// bound of the clamping range, this noise module clamps that value to
/// the lower bound.  If the output value from the source module is
/// greater than the upper bound of the clamping range, this noise module
/// clamps that value to the upper bound.
///
/// To specify the upper and lower bounds of the clamping range, call the
/// SetBounds() method.
///
/// This noise module requires one source module.
class Clamp: public Module
{

public:

    /// Constructor.
    ///
    /// The default lower bound of the clamping range is set to
    /// noise::module::DEFAULT_CLAMP_LOWER_BOUND.
    ///
    /// The default upper bound of the clamping range is set to
    /// noise::module::DEFAULT_CLAMP_UPPER_BOUND.
    Clamp ();

    /// Returns the lower bound of the clamping range.
    ///
    /// @returns The lower bound.
    ///
    /// If the output value from the source module is less than the lower
    /// bound of the clamping range, this noise module clamps that value
    /// to the lower bound.
    double GetLowerBound () const
    {
        return m_lowerBound;
    }

    virtual int GetSourceModuleCount () const
    {
        return 1;
    }

    /// Returns the upper bound of the clamping range.
    ///
    /// @returns The upper bound.
    ///
    /// If the output value from the source module is greater than the
    /// upper bound of the clamping range, this noise module clamps that
    /// value to the upper bound.
    double GetUpperBound () const
    {
        return m_upperBound;
    }

    /// Sets the lower and upper bounds of the clamping range.
    ///
    /// @param lowerBound The lower bound.
    /// @param upperBound The upper bound.
    ///
    /// @pre The lower bound must be less than or equal to the
    /// upper bound.
    ///
    /// @throw noise::ExceptionInvalidParam An invalid parameter was
    /// specified; see the preconditions for more information.
    ///
    /// If the output value from the source module is less than the lower
    /// bound of the clamping range, this noise module clamps that value
    /// to the lower bound.  If the output value from the source module
    /// is greater than the upper bound of the clamping range, this noise
    /// module clamps that value to the upper bound.
    void SetBounds (double lowerBound, double upperBound);

protected:

    /// Lower bound of the clamping range.
    double m_lowerBound;

    /// Upper bound of the clamping range.
    double m_upperBound;

};

/// @}

/// @}

/// @}

}

}

#endif
