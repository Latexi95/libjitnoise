#include "jitnoise/module/ridgedmulti.h"
#include "codegenerator.h"
#include "builder.h"
using namespace noise::module;
using namespace noise;


class RidgedMultiCodeGenerator : public CodeGenerator
{
public:
    RidgedMultiCodeGenerator(RidgedMulti *a) : m_Module(a), m_Function(0) {}
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

            llvm::Value *weight = b.getFPValue(1.0);

            // These parameters should be user-defined; they may be exposed in a
            // future version of jitnoise.
            double offset = 1.0;
            double gain = 2.0;

            for (int curOctave = 0; curOctave < m_Module->GetOctaveCount(); curOctave++) {
                llvm::Value *nx = b.CreateMakeInt32Range(x);
                llvm::Value *ny = b.CreateMakeInt32Range(y);
                llvm::Value *nz = b.CreateMakeInt32Range(z);

                int seed = m_Module->GetSeed() + curOctave;
                llvm::Value *signal = b.CreateGradientCoherentNoise3D(nx, ny, nz, b.getIValue(seed), m_Module->GetNoiseQuality());
                signal = b.CreateFAbs(signal);
                signal = b.CreateFSub(b.getFPValue(offset), signal);

                signal = b.CreateFMul(signal, signal);

                signal = b.CreateFMul(signal, weight);

                weight = b.CreateFMul(signal, b.getFPValue(gain));
                weight = b.CreateFClamp(weight, 0.0, 1.0);

                value = b.CreateFAdd(value, b.CreateFMul(signal, b.getFPValue(m_Module->GetSpectralWeights()[curOctave])));


                x = b.CreateFMul(x, b.getFPValue(m_Module->GetLacunarity()));
                y = b.CreateFMul(y, b.getFPValue(m_Module->GetLacunarity()));
                z = b.CreateFMul(z, b.getFPValue(m_Module->GetLacunarity()));
            }

            b.CreateRet(value);


            b.restoreIP(ip);
        }

        return b.CreateCallModuleFunction(m_Function, x, y, z, codeGenContext);
    }
private:
    RidgedMulti *m_Module;
    llvm::Function *m_Function;
};

RidgedMulti::RidgedMulti ():
  Module (GetSourceModuleCount (), new RidgedMultiCodeGenerator(this)),
  m_frequency    (DEFAULT_RIDGED_FREQUENCY   ),
  m_lacunarity   (DEFAULT_RIDGED_LACUNARITY  ),
  m_noiseQuality (DEFAULT_RIDGED_QUALITY     ),
  m_octaveCount  (DEFAULT_RIDGED_OCTAVE_COUNT),
  m_seed         (DEFAULT_RIDGED_SEED)
{
  CalcSpectralWeights ();
}

// Calculates the spectral weights for each octave.
void RidgedMulti::CalcSpectralWeights ()
{
  // This exponent parameter should be user-defined; it may be exposed in a
  // future version of jitnoise.
  double h = 1.0;

  double frequency = 1.0;
  for (int i = 0; i < RIDGED_MAX_OCTAVE; i++) {
    // Compute weight for each frequency.
    m_pSpectralWeights[i] = pow (frequency, -h);
    frequency *= m_lacunarity;
  }
}
