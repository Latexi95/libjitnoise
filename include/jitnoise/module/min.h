#ifndef NOISE_MODULE_MIN_H
#define NOISE_MODULE_MIN_H

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

    /// Noise module that outputs the smaller of the two output values from
    /// two source modules.
    ///
    /// @image html modulemin.png
    ///
    /// This noise module requires two source modules.
    class Min: public Module
    {

      public:

        /// Constructor.
        Min ();

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
