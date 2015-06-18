#include "jitnoise/module/modulebase.h"
#include "codegenerator.h"

using namespace noise::module;
using namespace noise;

Module::Module (int sourceModuleCount, CodeGenerator *codeGenerator):
    m_pSourceModule(0),
    m_CodeGenerator(codeGenerator)
{
    m_pSourceModule = NULL;

    // Create an array of pointers to all source modules required by this
    // noise module.  Set these pointers to NULL.
    if (sourceModuleCount > 0) {
        m_pSourceModule = new const Module*[sourceModuleCount];
        for (int i = 0; i < sourceModuleCount; i++) {
            m_pSourceModule[i] = NULL;
        }
    } else {
        m_pSourceModule = NULL;
    }
}

Module::~Module ()
{
    delete[] m_pSourceModule;
    delete m_CodeGenerator;
}
