#include "perlin.h"
#include "../codegenerator.h"
namespace jitnoise {

class PerlinNoiseGenerator : public CodeGenerator {
	public:
		PerlinNoiseGenerator() : mFunction(0) {}
		llvm::Value *generate(Builder *builder, const Module *module, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *generationData) {
			const Perlin *perlin = static_cast<const Perlin*>(module);
			if (!mFunction) generateFunction(builder, perlin);
			return builder->createModuleFunctionCall(mFunction, x, y, z, generationData);
			/*
			double value = 0.0;
			double signal = 0.0;
			double curPersistence = 1.0;
			double nx, ny, nz;
			int seed;

			x *= m_frequency;
			y *= m_frequency;
			z *= m_frequency;

			for (int curOctave = 0; curOctave < m_octaveCount; curOctave++) {

			// Make sure that these floating-point values have the same range as a 32-
			// bit integer so that we can pass them to the coherent-noise functions.
			nx = MakeInt32Range (x);
			ny = MakeInt32Range (y);
			nz = MakeInt32Range (z);

			// Get the coherent-noise value from the input value and add it to the
			// final result.
			seed = (m_seed + curOctave) & 0xffffffff;
			signal = GradientCoherentNoise3D (nx, ny, nz, seed, m_noiseQuality);
			value += signal * curPersistence;

			// Prepare the next octave.
			x *= m_lacunarity;
			y *= m_lacunarity;
			z *= m_lacunarity;
			curPersistence *= m_persistence;
			}
			*/
		}

	private:
		void generateFunction(Builder *tempBuilder, const Perlin *perlin) {
			llvm::Value *x;
			llvm::Value *y;
			llvm::Value *z;
			llvm::Value *generationData;
			llvm::BasicBlock *basicBlock;
			Builder *builder = tempBuilder->createModuleFunction("perlin", &x, &y, &z, &generationData, &basicBlock);
			mFunction = builder->function();
			builder->debugPrintDouble(builder->getIRBuilder(), 1, x);
			builder->debugPrintDouble(builder->getIRBuilder(), 2, y);
			builder->debugPrintDouble(builder->getIRBuilder(), 3, z);

			llvm::AllocaInst * value = builder->getIRBuilder()->CreateAlloca(builder->getFPType());
			llvm::AllocaInst * xVar = builder->getIRBuilder()->CreateAlloca(builder->getFPType());
			llvm::AllocaInst * yVar = builder->getIRBuilder()->CreateAlloca(builder->getFPType());
			llvm::AllocaInst * zVar = builder->getIRBuilder()->CreateAlloca(builder->getFPType());
			llvm::AllocaInst * curPersistence = builder->getIRBuilder()->CreateAlloca(builder->getSingleFPType());
			llvm::AllocaInst *curOctave = builder->getIRBuilder()->CreateAlloca(builder->getSingleIntType());
			builder->getIRBuilder()->CreateStore(builder->getFPValue(0), value);
			builder->getIRBuilder()->CreateStore(builder->fmul(x, builder->getFPValue(perlin->frequency())), xVar);
			builder->getIRBuilder()->CreateStore(builder->fmul(y, builder->getFPValue(perlin->frequency())), yVar);
			builder->getIRBuilder()->CreateStore(builder->fmul(z, builder->getFPValue(perlin->frequency())), zVar);
			builder->getIRBuilder()->CreateStore(llvm::ConstantFP::get(builder->getSingleFPType(), 1.0), curPersistence);
			builder->getIRBuilder()->CreateStore(builder->getIRBuilder()->getInt32(0), curOctave);

			llvm::BasicBlock *condBB = builder->createBasicBlock("perlinForCond");
			llvm::BasicBlock *loopBodyBB = builder->createBasicBlock("perlinForBody");
			llvm::BasicBlock *loopEndBB = builder->createBasicBlock("perlinForEnd");

			builder->getIRBuilder()->CreateBr(condBB);
			builder->getIRBuilder()->SetInsertPoint(condBB);
			llvm::Value *octave = builder->getIRBuilder()->CreateLoad(curOctave);
			llvm::Value *cond = builder->getIRBuilder()->CreateICmpULT(octave, builder->getIRBuilder()->getInt32(perlin->octaveCount()));
			builder->getIRBuilder()->CreateCondBr(cond, loopBodyBB, loopEndBB);

			builder->getIRBuilder()->SetInsertPoint(loopBodyBB);
			llvm::Value *nx = builder->makeInt32Range(builder->getIRBuilder(), builder->getIRBuilder()->CreateLoad(xVar));
			llvm::Value *ny = builder->makeInt32Range(builder->getIRBuilder(), builder->getIRBuilder()->CreateLoad(yVar));
			llvm::Value *nz = builder->makeInt32Range(builder->getIRBuilder(), builder->getIRBuilder()->CreateLoad(zVar));
			builder->debugPrintDouble(builder->getIRBuilder(), 4, nx);
			builder->debugPrintDouble(builder->getIRBuilder(), 5, ny);
			builder->debugPrintDouble(builder->getIRBuilder(), 6, nz);

			builder->debugPrintDouble(builder->getIRBuilder(), 7, builder->getIRBuilder()->CreateSIToFP(octave, builder->getSingleFPType()));


			llvm::Value *seed;
			if (builder->context().mVectorize) {
				llvm::Value *vectorizedOctave = builder->vectorizeSingleValue(builder->getIRBuilder(), octave);
				seed = builder->getIRBuilder()->CreateAnd(builder->getIRBuilder()->CreateAdd(builder->getIntValue(perlin->seed()), vectorizedOctave), builder->getIntValue(0xffffffff));
			}
			else {
				seed = builder->getIRBuilder()->CreateAnd(builder->getIRBuilder()->CreateAdd(builder->getIntValue(perlin->seed()), octave), builder->getIntValue(0xffffffff));
			}
			llvm::Value *signal = builder->gradientCoherentNoise3D(builder->getIRBuilder(), nx, ny, nz, seed);
			builder->debugPrintDouble(builder->getIRBuilder(), 11, signal);

			llvm::Value *persistence = builder->getIRBuilder()->CreateLoad(curPersistence);
			llvm::Value *val = builder->getIRBuilder()->CreateLoad(value);
			builder->debugPrintDouble(builder->getIRBuilder(), 8, val);
			if (builder->context().mVectorize) {
				builder->getIRBuilder()->CreateStore(builder->fadd(val, builder->fmul(signal, builder->vectorizeSingleValue(builder->getIRBuilder(), persistence))), value);
			}
			else {
				builder->getIRBuilder()->CreateStore(builder->fadd(val, builder->fmul(signal, persistence)), value);
			}


			x = builder->getIRBuilder()->CreateLoad(xVar);
			y = builder->getIRBuilder()->CreateLoad(yVar);
			z = builder->getIRBuilder()->CreateLoad(zVar);

			builder->getIRBuilder()->CreateStore(builder->fmul(x, builder->getFPValue(perlin->lacunarity())), xVar);
			builder->getIRBuilder()->CreateStore(builder->fmul(y, builder->getFPValue(perlin->lacunarity())), yVar);
			builder->getIRBuilder()->CreateStore(builder->fmul(z, builder->getFPValue(perlin->lacunarity())), zVar);

			builder->getIRBuilder()->CreateStore(builder->fmul(persistence, llvm::ConstantFP::get(builder->getSingleFPType(), perlin->persistence())), curPersistence);


			octave = builder->getIRBuilder()->CreateLoad	(curOctave);
			builder->getIRBuilder()->CreateStore(builder->getIRBuilder()->CreateAdd(octave, builder->getIRBuilder()->getInt32(1)), curOctave);
			builder->getIRBuilder()->CreateBr(condBB);

			builder->getIRBuilder()->SetInsertPoint(loopEndBB);


			llvm::Value *ret = builder->getIRBuilder()->CreateLoad(value);
			builder->debugPrintDouble(builder->getIRBuilder(), 9, ret);

			builder->getIRBuilder()->CreateRet(ret);
			delete builder;
		}

		llvm::Function *mFunction;
};


Perlin::Perlin() :
	Module(0),
	mFrequency(PERLIN_DEFAULT_FREQUENCY),
	mLacunarity(PERLIN_DEFAULT_LACUNARITY),
	mOctaveCount(PERLIN_DEFAULT_OCTAVE_COUNT),
	mPersistence(PERLIN_DEFAULT_PERSISTENCE),
	mSeed(PERLIN_DEFAULT_SEED),
	mCodeGenerator(new PerlinNoiseGenerator()){
}

Perlin::~Perlin() {
	delete mCodeGenerator;
}

CodeGenerator *Perlin::codeGenerator() const {
	return mCodeGenerator;
}

}
