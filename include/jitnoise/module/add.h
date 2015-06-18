#ifndef NOISE_MODULE_ADD_H
#define NOISE_MODULE_ADD_H

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
    
    /// Noise module that outputs the sum of the two output values from two
    /// source modules.
    ///
    /// @image html moduleadd.png
    ///
    /// This noise module requires two source modules.
    class Add: public Module
    {

      public:

        /// Constructor.
        Add ();

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
