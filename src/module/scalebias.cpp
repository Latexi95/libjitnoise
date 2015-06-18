#include "jitnoise/module/scalebias.h"
#include "codegenerator.h"
using namespace noise::module;
using namespace noise;

class ScaleBiasCodeGenerator : public CodeGenerator
{
public:
    ScaleBiasCodeGenerator(ScaleBias *a) : m_Module(a) {}
    llvm::Value *generate(Builder &b, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *codeGenContext) {
        llvm::Value *sourceVal = m_Module->GetSourceModule(0).GetCodeGenerator()->generate(b, x, y, z, codeGenContext);

        return b.CreateFAdd(b.CreateFMul(sourceVal, b.getFPValue(m_Module->GetScale())), b.getFPValue(m_Module->GetBias()));
    }
private:
    ScaleBias *m_Module;
};


ScaleBias::ScaleBias ():
  Module (GetSourceModuleCount (), new ScaleBiasCodeGenerator(this)),
  m_bias  (DEFAULT_BIAS ),
  m_scale (DEFAULT_SCALE)
{
}
