
#include "jitnoise/module/max.h"
#include "codegenerator.h"
#include "builder.h"
using namespace noise::module;
using namespace noise;


class MaxCodeGenerator : public CodeGenerator
{
public:
    MaxCodeGenerator(Max *a) : m_Module(a) {}
    llvm::Value *generate(Builder &builder, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *codeGenContext) {
        llvm::Value *source0 = m_Module->GetSourceModule(0).GetCodeGenerator()->generate(builder, x, y, z, codeGenContext);
        llvm::Value *source1 = m_Module->GetSourceModule(1).GetCodeGenerator()->generate(builder, x, y, z, codeGenContext);

        return builder.CreateFMax(source0, source1);
    }
private:
    Max *m_Module;
};


using namespace noise::module;

Max::Max ():
  Module (GetSourceModuleCount (), new MaxCodeGenerator(this))
{
}
