#ifndef NOISE_MODULE_CHECKERBOARD_H
#define NOISE_MODULE_CHECKERBOARD_H

#include "modulebase.h"

namespace noise
{

  namespace module
  {

    /// @addtogroup jitnoise
    /// @{

    /// @addtogroup modules
    /// @{

    /// @addtogroup generatormodules
    /// @{

    /// Noise module that outputs a checkerboard pattern.
    ///
    /// @image html modulecheckerboard.png
    ///
    /// This noise module outputs unit-sized blocks of alternating values.
    /// The values of these blocks alternate between -1.0 and +1.0.
    ///
    /// This noise module is not really useful by itself, but it is often used
    /// for debugging purposes.
    ///
    /// This noise module does not require any source modules.
    class Checkerboard: public Module
    {

      public:

        /// Constructor.
        Checkerboard ();

        virtual int GetSourceModuleCount () const
        {
        return 0;
        }


    };

    /// @}

    /// @}

    /// @}

  }

}

#endif
