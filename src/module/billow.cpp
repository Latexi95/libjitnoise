#include "jitnoise/module/billow.h"
#include "codegenerator.h"
#include "builder.h"
using namespace noise::module;
using namespace noise;


class BillowCodeGenerator : public CodeGenerator
{
public:
    BillowCodeGenerator(Billow *a) : m_Module(a), m_Function(0) {}
    llvm::Value *generate(Builder &b, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *codeGenContext) {

        if (!m_Function) {
            llvm::Value *x, *y, *z, *codeGenContext;
            m_Function = b.CreateModuleFunction(x, y, z, codeGenContext);

            Builder::InsertPoint ip = b.saveAndClearIP();

            llvm::BasicBlock *entryBB = llvm::BasicBlock::Create(b.getContext(), "entry", m_Function);

            b.SetInsertPoint(entryBB);

            llvm::Value *valueVar = b.CreateAlloca(b.getFPType());
            b.CreateStore(b.getFPValue(0), valueVar);

            llvm::Value *xVar = b.CreateAlloca(b.getFPType());
            llvm::Value *yVar = b.CreateAlloca(b.getFPType());
            llvm::Value *zVar = b.CreateAlloca(b.getFPType());

            b.CreateStore(b.CreateFMul(x, b.getFPValue(m_Module->GetFrequency())), xVar);
            b.CreateStore(b.CreateFMul(y, b.getFPValue(m_Module->GetFrequency())), yVar);
            b.CreateStore(b.CreateFMul(z, b.getFPValue(m_Module->GetFrequency())), zVar);

            double curPersistence = 1.0;
            for (int curOctave = 0; curOctave < m_Module->GetOctaveCount(); curOctave++) {
                x = b.CreateLoad(xVar);
                y = b.CreateLoad(xVar);
                z = b.CreateLoad(xVar);
                llvm::Value *nx = b.CreateMakeInt32Range(x);
                llvm::Value *ny = b.CreateMakeInt32Range(y);
                llvm::Value *nz = b.CreateMakeInt32Range(z);

                llvm::Value *seed = b.getIValue(m_Module->GetSeed() + curOctave);
                llvm::Value *signal = b.CreateGradientCoherentNoise3D(nx, ny, nz, seed, m_Module->GetNoiseQuality());
                signal = b.CreateFSub(b.CreateFMul(b.getFPValue(2.0), b.CreateFAbs(signal)), b.getFPValue(1.0));

                llvm::Value *value = b.CreateLoad(valueVar);
                b.CreateStore(b.CreateAdd(b.CreateFMul(signal, b.getFPValue(curPersistence)), value), valueVar);

                b.CreateStore(b.CreateFMul(x, b.getFPValue(m_Module->GetLacunarity())), xVar);
                b.CreateStore(b.CreateFMul(y, b.getFPValue(m_Module->GetLacunarity())), yVar);
                b.CreateStore(b.CreateFMul(z, b.getFPValue(m_Module->GetLacunarity())), zVar);
                curPersistence = m_Module->GetPersistence();
            }

            llvm::Value *value = b.CreateLoad(valueVar);

            b.CreateRet(b.CreateAdd(value, b.getFPValue(0.5)));

            b.restoreIP(ip);
        }




        return b.CreateCallModuleFunction(m_Function, x, y, z, codeGenContext);
    }
private:
    Billow *m_Module;
    llvm::Function *m_Function;
};

Billow::Billow ():
  Module (GetSourceModuleCount (), new BillowCodeGenerator(this)),
  m_frequency    (DEFAULT_BILLOW_FREQUENCY   ),
  m_lacunarity   (DEFAULT_BILLOW_LACUNARITY  ),
  m_noiseQuality (DEFAULT_BILLOW_QUALITY     ),
  m_octaveCount  (DEFAULT_BILLOW_OCTAVE_COUNT),
  m_persistence  (DEFAULT_BILLOW_PERSISTENCE ),
  m_seed         (DEFAULT_BILLOW_SEED)
{
}

