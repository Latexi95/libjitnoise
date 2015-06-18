#include "jitnoise/module/turbulence.h"
#include "codegenerator.h"
using namespace noise::module;
using namespace noise;

class TurbulenceCodeGenerator : public CodeGenerator
{
public:
    TurbulenceCodeGenerator(Turbulence *a) : m_Module(a), m_Function(0) {}
    llvm::Value *generate(Builder &b, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *codeGenContext) {
        if (!m_Function) {
            llvm::Value *x, *y, *z, *codeGenContext;
            m_Function = b.CreateModuleFunction(x, y, z, codeGenContext);

            Builder::InsertPoint ip = b.saveAndClearIP();

            llvm::BasicBlock *entryBB = llvm::BasicBlock::Create(b.getContext(), "entry", m_Function);
            b.SetInsertPoint(entryBB);

            llvm::Value *x0, *y0, *z0;
            llvm::Value *x1, *y1, *z1;
            llvm::Value *x2, *y2, *z2;

            x0 = b.CreateAdd(x, b.getFPValue(12414.0 / 65536.0));
            y0 = b.CreateAdd(y, b.getFPValue(65124.0 / 65536.0));
            z0 = b.CreateAdd(z, b.getFPValue(31337.0 / 65536.0));

            x1 = b.CreateAdd(x, b.getFPValue(26519.0 / 65536.0));
            y1 = b.CreateAdd(y, b.getFPValue(18128.0 / 65536.0));
            z1 = b.CreateAdd(z, b.getFPValue(60493.0 / 65536.0));

            x2 = b.CreateAdd(x, b.getFPValue(53820.0 / 65536.0));
            y2 = b.CreateAdd(y, b.getFPValue(11213.0 / 65536.0));
            z2 = b.CreateAdd(z, b.getFPValue(44845.0 / 65536.0));

            llvm::Value *xDistort = b.CreateFAdd(x, b.CreateFMul(m_Module->GetXDistortModule().GetCodeGenerator()->generate(b, x0, y0, z0, codeGenContext), b.getFPValue(m_Module->GetPower())));
            llvm::Value *yDistort = b.CreateFAdd(y, b.CreateFMul(m_Module->GetXDistortModule().GetCodeGenerator()->generate(b, x1, y1, z1, codeGenContext), b.getFPValue(m_Module->GetPower())));
            llvm::Value *zDistort = b.CreateFAdd(z, b.CreateFMul(m_Module->GetXDistortModule().GetCodeGenerator()->generate(b, x2, y2, z2, codeGenContext), b.getFPValue(m_Module->GetPower())));

            b.CreateRet(m_Module->GetSourceModule(0).GetCodeGenerator()->generate(b, xDistort, yDistort, zDistort, codeGenContext));
            b.restoreIP(ip);
        }

        return b.CreateCallModuleFunction(m_Function, x, y, z, codeGenContext);
    }
private:
    Turbulence *m_Module;
    llvm::Function *m_Function;
};


Turbulence::Turbulence ():
    Module (GetSourceModuleCount (), new TurbulenceCodeGenerator(this)),
    m_power (DEFAULT_TURBULENCE_POWER)
{
    SetSeed (DEFAULT_TURBULENCE_SEED);
    SetFrequency (DEFAULT_TURBULENCE_FREQUENCY);
    SetRoughness (DEFAULT_TURBULENCE_ROUGHNESS);
}

double Turbulence::GetFrequency () const
{
    // Since each noise::module::Perlin noise module has the same frequency, it
    // does not matter which module we use to retrieve the frequency.
    return m_xDistortModule.GetFrequency ();
}

int Turbulence::GetSeed () const
{
    return m_xDistortModule.GetSeed ();
}



void Turbulence::SetSeed (int seed)
{
    // Set the seed of each noise::module::Perlin noise modules.  To prevent any
    // sort of weird artifacting, use a slightly different seed for each noise
    // module.
    m_xDistortModule.SetSeed (seed    );
    m_yDistortModule.SetSeed (seed + 1);
    m_zDistortModule.SetSeed (seed + 2);
}
