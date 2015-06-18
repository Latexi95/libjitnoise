#include "jitnoise/module/clamp.h"
#include "codegenerator.h"
#include "builder.h"
using namespace noise::module;
using namespace noise;


class ClampCodeGenerator : public CodeGenerator
{
public:
    ClampCodeGenerator(Clamp *a) : m_Module(a) {}
    llvm::Value *generate(Builder &b, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *codeGenContext) {

        llvm::Value *value = m_Module->GetSourceModule(0).GetCodeGenerator()->generate(b, x, y, z, codeGenContext);

        return b.CreateFClamp(value, m_Module->GetLowerBound(), m_Module->GetUpperBound());
    }
private:
    Clamp *m_Module;
};

Clamp::Clamp ():
    Module (GetSourceModuleCount (), new ClampCodeGenerator(this)),
    m_lowerBound (DEFAULT_CLAMP_LOWER_BOUND),
    m_upperBound (DEFAULT_CLAMP_UPPER_BOUND)
{
}

void Clamp::SetBounds (double lowerBound, double upperBound)
{
    assert (lowerBound < upperBound);

    m_lowerBound = lowerBound;
    m_upperBound = upperBound;
}
