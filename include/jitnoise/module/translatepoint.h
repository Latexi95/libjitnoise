
#ifndef NOISE_MODULE_TRANSLATEPOINT_H
#define NOISE_MODULE_TRANSLATEPOINT_H

#include "modulebase.h"

namespace noise
{

namespace module
{

/// @addtogroup jitnoise
/// @{

/// @addtogroup modules
/// @{

/// @addtogroup transformermodules
/// @{

/// Default translation factor applied to the @a x coordinate for the
/// noise::module::TranslatePoint noise module.
const double DEFAULT_TRANSLATE_POINT_X = 0.0;

/// Default translation factor applied to the @a y coordinate for the
/// noise::module::TranslatePoint noise module.
const double DEFAULT_TRANSLATE_POINT_Y = 0.0;

/// Default translation factor applied to the @a z coordinate for the
/// noise::module::TranslatePoint noise module.
const double DEFAULT_TRANSLATE_POINT_Z = 0.0;

/// Noise module that moves the coordinates of the input value before
/// returning the output value from a source module.
///
/// @image html moduletranslatepoint.png
///
/// The GetValue() method moves the ( @a x, @a y, @a z ) coordinates of
/// the input value by a translation amount before returning the output
/// value from the source module.  To set the translation amount, call
/// the SetTranslation() method.  To set the translation amount to
/// apply to the individual @a x, @a y, or @a z coordinates, call the
/// SetXTranslation(), SetYTranslation() or SetZTranslation() methods,
/// respectively.
///
/// This noise module requires one source module.
class TranslatePoint: public Module
{

public:

    /// Constructor.
    ///
    /// The default translation amount to apply to the @a x coordinate is
    /// set to noise::module::DEFAULT_TRANSLATE_POINT_X.
    ///
    /// The default translation amount to apply to the @a y coordinate is
    /// set to noise::module::DEFAULT_TRANSLATE_POINT_Y.
    ///
    /// The default translation amount to apply to the @a z coordinate is
    /// set to noise::module::DEFAULT_TRANSLATE_POINT_Z.
    TranslatePoint ();

    virtual int GetSourceModuleCount () const
    {
        return 1;
    }

    /// Returns the translation amount to apply to the @a x coordinate of
    /// the input value.
    ///
    /// @returns The translation amount to apply to the @a x coordinate.
    double GetXTranslation () const
    {
        return m_xTranslation;
    }

    /// Returns the translation amount to apply to the @a y coordinate of
    /// the input value.
    ///
    /// @returns The translation amount to apply to the @a y coordinate.
    double GetYTranslation () const
    {
        return m_yTranslation;
    }

    /// Returns the translation amount to apply to the @a z coordinate of
    /// the input value.
    ///
    /// @returns The translation amount to apply to the @a z coordinate.
    double GetZTranslation () const
    {
        return m_zTranslation;
    }

    /// Sets the translation amount to apply to the input value.
    ///
    /// @param translation The translation amount to apply.
    ///
    /// The GetValue() method moves the ( @a x, @a y, @a z ) coordinates
    /// of the input value by a translation amount before returning the
    /// output value from the source module
    void SetTranslation (double translation)
    {
        m_xTranslation = translation;
        m_yTranslation = translation;
        m_zTranslation = translation;
    }

    /// Sets the translation amounts to apply to the ( @a x, @a y, @a z )
    /// coordinates of the input value.
    ///
    /// @param xTranslation The translation amount to apply to the @a x
    /// coordinate.
    /// @param yTranslation The translation amount to apply to the @a y
    /// coordinate.
    /// @param zTranslation The translation amount to apply to the @a z
    /// coordinate.
    ///
    /// The GetValue() method moves the ( @a x, @a y, @a z ) coordinates
    /// of the input value by a translation amount before returning the
    /// output value from the source module
    void SetTranslation (double xTranslation, double yTranslation,
                         double zTranslation)
    {
        m_xTranslation = xTranslation;
        m_yTranslation = yTranslation;
        m_zTranslation = zTranslation;
    }

    /// Sets the translation amount to apply to the @a x coordinate of the
    /// input value.
    ///
    /// @param xTranslation The translation amount to apply to the @a x
    /// coordinate.
    ///
    /// The GetValue() method moves the ( @a x, @a y, @a z ) coordinates
    /// of the input value by a translation amount before returning the
    /// output value from the source module
    void SetXTranslation (double xTranslation)
    {
        m_xTranslation = xTranslation;
    }

    /// Sets the translation amount to apply to the @a y coordinate of the
    /// input value.
    ///
    /// @param yTranslation The translation amount to apply to the @a y
    /// coordinate.
    ///
    /// The GetValue() method moves the ( @a x, @a y, @a z ) coordinates
    /// of the input value by a translation amount before returning the
    /// output value from the source module
    void SetYTranslation (double yTranslation)
    {
        m_yTranslation = yTranslation;
    }

    /// Sets the translation amount to apply to the @a z coordinate of the
    /// input value.
    ///
    /// @param zTranslation The translation amount to apply to the @a z
    /// coordinate.
    ///
    /// The GetValue() method moves the ( @a x, @a y, @a z ) coordinates
    /// of the input value by a translation amount before returning the
    /// output value from the source module
    void SetZTranslation (double zTranslation)
    {
        m_zTranslation = zTranslation;
    }

protected:

    /// Translation amount applied to the @a x coordinate of the input
    /// value.
    double m_xTranslation;

    /// Translation amount applied to the @a y coordinate of the input
    /// value.
    double m_yTranslation;

    /// Translation amount applied to the @a z coordinate of the input
    /// value.
    double m_zTranslation;

};

/// @}

/// @}

/// @}

}

}

#endif
