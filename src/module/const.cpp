#include "jitnoise/module/const.h"
#include "codegenerator.h"
#include "builder.h"
using namespace noise::module;
using namespace noise;


class ConstCodeGenerator : public CodeGenerator
{
public:
    ConstCodeGenerator(Const *a) : m_Module(a) {}
    llvm::Value *generate(Builder &b, llvm::Value *, llvm::Value *, llvm::Value *, llvm::Value *) {
        return b.getFPValue(m_Module->GetConstValue());
    }
private:
    Const *m_Module;
};

Const::Const ():
  Module (GetSourceModuleCount (), new ConstCodeGenerator(this)),
  m_constValue (DEFAULT_CONST_VALUE)
{
}
