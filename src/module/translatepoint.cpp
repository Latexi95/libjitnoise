#include "jitnoise/module/translatepoint.h"
#include "codegenerator.h"
using namespace noise::module;
using namespace noise;

class TranslatePointCodeGenerator : public CodeGenerator
{
public:
    TranslatePointCodeGenerator(TranslatePoint *a) : m_Module(a) {}
    llvm::Value *generate(Builder &b, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *codeGenContext) {
        x = b.CreateFAdd(x, b.getFPValue(m_Module->GetXTranslation()));
        y = b.CreateFAdd(y, b.getFPValue(m_Module->GetYTranslation()));
        z = b.CreateFAdd(z, b.getFPValue(m_Module->GetZTranslation()));
        return m_Module->GetSourceModule(0).GetCodeGenerator()->generate(b, x, y, z, codeGenContext);
    }
private:
    TranslatePoint *m_Module;
};


TranslatePoint::TranslatePoint ():
    Module (GetSourceModuleCount (), new TranslatePointCodeGenerator(this)),
    m_xTranslation (DEFAULT_TRANSLATE_POINT_X),
    m_yTranslation (DEFAULT_TRANSLATE_POINT_Y),
    m_zTranslation (DEFAULT_TRANSLATE_POINT_Z)
{
}

