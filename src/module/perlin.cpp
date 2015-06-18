#include "jitnoise/module/perlin.h"
#include "codegenerator.h"
#include "builder.h"
using namespace noise::module;
using namespace noise;


class PerlinCodeGenerator : public CodeGenerator
{
public:
    PerlinCodeGenerator(Perlin *a) : m_Module(a), m_Function(0) {}
    llvm::Value *generate(Builder &b, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *codeGenContext) {
        if (!m_Function) {
            llvm::Value *x, *y, *z, *codeGenContext;
            m_Function = b.CreateModuleFunction(x, y, z, codeGenContext);

            Builder::InsertPoint ip = b.saveAndClearIP();
            llvm::BasicBlock *entryBB = llvm::BasicBlock::Create(b.getContext(), "entry", m_Function);
            b.SetInsertPoint(entryBB);

            llvm::Value *value = b.getFPValue(0.0);

            x = b.CreateFMul(x, b.getFPValue(m_Module->GetFrequency()));
            y = b.CreateFMul(y, b.getFPValue(m_Module->GetFrequency()));
            z = b.CreateFMul(z, b.getFPValue(m_Module->GetFrequency()));

            double curPersistence = 1.0;
            for (int curOctave = 0; curOctave < m_Module->GetOctaveCount(); curOctave++) {
                llvm::Value *nx = b.CreateMakeInt32Range(x);
                llvm::Value *ny = b.CreateMakeInt32Range(y);
                llvm::Value *nz = b.CreateMakeInt32Range(z);

                int seed = m_Module->GetSeed() + curOctave;
                llvm::Value *signal = b.CreateGradientCoherentNoise3D(nx, ny, nz, b.getIValue(seed), m_Module->GetNoiseQuality());
                value = b.CreateFAdd(value, b.CreateFMul(signal, b.getFPValue(curPersistence)));


                x = b.CreateFMul(x, b.getFPValue(m_Module->GetLacunarity()));
                y = b.CreateFMul(y, b.getFPValue(m_Module->GetLacunarity()));
                z = b.CreateFMul(z, b.getFPValue(m_Module->GetLacunarity()));

                curPersistence *= m_Module->GetPersistence();
            }

            b.CreateRet(value);


            b.restoreIP(ip);
        }

        return b.CreateCallModuleFunction(m_Function, x, y, z, codeGenContext);
    }
private:
    Perlin *m_Module;
    llvm::Function *m_Function;
};


using namespace noise::module;

Perlin::Perlin ():
  Module (GetSourceModuleCount (), new PerlinCodeGenerator(this)),
  m_frequency    (DEFAULT_PERLIN_FREQUENCY   ),
  m_lacunarity   (DEFAULT_PERLIN_LACUNARITY  ),
  m_noiseQuality (DEFAULT_PERLIN_QUALITY     ),
  m_octaveCount  (DEFAULT_PERLIN_OCTAVE_COUNT),
  m_persistence  (DEFAULT_PERLIN_PERSISTENCE ),
  m_seed         (DEFAULT_PERLIN_SEED)
{
}
