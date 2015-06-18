#include "jitnoise/module/exponent.h"
#include "codegenerator.h"
#include "codegencontext.h"
using namespace noise::module;
using namespace noise;

class ExponentCodeGenerator : public CodeGenerator
{
public:
    ExponentCodeGenerator(Exponent *a) : m_Module(a) {}
    llvm::Value *generate(Builder &b, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *codeGenContext) {

        llvm::Value *value = m_Module->GetSourceModule(0).GetCodeGenerator()->generate(b, x, y, z, codeGenContext);

        llvm::Value *fabsResult = b.CreateFAbs(b.CreateFDiv(b.CreateFAdd(value, b.getFPValue(1.0)), b.getFPValue(2.0)));
        llvm::Value *powResult = b.CreateFPow(fabsResult, b.getFPValue(m_Module->GetExponent()));
        return b.CreateFSub(b.CreateFMul(powResult, b.getFPValue(2.0)), b.getFPValue(1.0));
    }
private:
    Exponent *m_Module;
};

Exponent::Exponent ():
  Module (GetSourceModuleCount (), new ExponentCodeGenerator(this)),
  m_exponent (DEFAULT_EXPONENT)
{
}
