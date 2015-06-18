#include "jitnoise/module/scalepoint.h"
#include "codegenerator.h"
using namespace noise::module;
using namespace noise;

class ScalePointCodeGenerator : public CodeGenerator
{
public:
    ScalePointCodeGenerator(ScalePoint *a) : m_Module(a) {}
    llvm::Value *generate(Builder &b, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *codeGenContext) {
        x = b.CreateFMul(x, b.getFPValue(m_Module->GetXScale()));
        y = b.CreateFMul(y, b.getFPValue(m_Module->GetYScale()));
        z = b.CreateFMul(z, b.getFPValue(m_Module->GetZScale()));
        return m_Module->GetSourceModule(0).GetCodeGenerator()->generate(b, x, y, z, codeGenContext);
    }
private:
    ScalePoint *m_Module;
};


ScalePoint::ScalePoint ():
  Module (GetSourceModuleCount (), new ScalePointCodeGenerator(this)),
  m_xScale (DEFAULT_SCALE_POINT_X),
  m_yScale (DEFAULT_SCALE_POINT_Y),
  m_zScale (DEFAULT_SCALE_POINT_Z)
{
}
