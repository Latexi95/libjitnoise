#include "ridgedmulti.h"
#include "../codegenerator.h"
namespace jitnoise {

class RidgedMultiNoiseGenerator : public CodeGenerator {
	public:
		RidgedMultiNoiseGenerator() : mFunction(0) {}
		llvm::Value *generate(Builder *builder, const Module *module, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *generationData) {
			const RidgedMulti *ridged = static_cast<const RidgedMulti*>(module);
			if (!mFunction) generateFunction(builder, ridged);
			return builder->createModuleFunctionCall(mFunction, x, y, z, generationData);
		}

	private:
		void generateFunction(Builder *tempBuilder, const RidgedMulti *ridged) {
			llvm::Value *x;
			llvm::Value *y;
			llvm::Value *z;
			llvm::Value *generationData;
			llvm::BasicBlock *basicBlock;
			Builder *builder = tempBuilder->createModuleFunction("ridgedMulti", &x, &y, &z, &generationData, &basicBlock);
			mFunction = builder->function();

			ridged->calculateSpectralWeights();
			llvm::ArrayType *spectralWeightsArrayType = llvm::ArrayType::get(builder->getSingleFPType(), ridged->octaveCount());
			std::vector<llvm::Constant*> spectralWeightValues;
			for (int i = 0; i < ridged->octaveCount(); i++) {
				spectralWeightValues.push_back(builder->getSingleFPValue(ridged->mSpectralWeights[i]));
			}
			llvm::Constant *spectralWeightsArrayInitializer = llvm::ConstantArray::get(spectralWeightsArrayType, spectralWeightValues);
			llvm::GlobalVariable *spectralWeightsArray = new llvm::GlobalVariable(*builder->module(), spectralWeightsArrayType, true, llvm::GlobalVariable::PrivateLinkage, spectralWeightsArrayInitializer);


			llvm::AllocaInst * value = builder->getIRBuilder()->CreateAlloca(builder->getFPType());
			llvm::AllocaInst * xVar = builder->getIRBuilder()->CreateAlloca(builder->getFPType());
			llvm::AllocaInst * yVar = builder->getIRBuilder()->CreateAlloca(builder->getFPType());
			llvm::AllocaInst * zVar = builder->getIRBuilder()->CreateAlloca(builder->getFPType());
			llvm::AllocaInst * weightVar = builder->getIRBuilder()->CreateAlloca(builder->getFPType());
			llvm::AllocaInst *curOctave = builder->getIRBuilder()->CreateAlloca(builder->getSingleIntType());
			builder->store(builder->getFPValue(0), value);
			builder->store(builder->fmul(x, builder->getFPValue(ridged->frequency())), xVar);
			builder->store(builder->fmul(y, builder->getFPValue(ridged->frequency())), yVar);
			builder->store(builder->fmul(z, builder->getFPValue(ridged->frequency())), zVar);
			builder->store(builder->getFPValue(1.0), weightVar);
			builder->store(builder->getIRBuilder()->getInt32(0), curOctave);

			llvm::BasicBlock *condBB = builder->createBasicBlock("ridgedForCond");
			llvm::BasicBlock *loopBodyBB = builder->createBasicBlock("ridgedForBody");
			llvm::BasicBlock *loopEndBB = builder->createBasicBlock("ridgedForEnd");

			builder->getIRBuilder()->CreateBr(condBB);
			builder->getIRBuilder()->SetInsertPoint(condBB);
			llvm::Value *octave = builder->load(curOctave);
			llvm::Value *cond = builder->getIRBuilder()->CreateICmpULT(octave, builder->getIRBuilder()->getInt32(ridged->octaveCount()));
			builder->getIRBuilder()->CreateCondBr(cond, loopBodyBB, loopEndBB);

			builder->getIRBuilder()->SetInsertPoint(loopBodyBB);
			llvm::Value *nx = builder->makeInt32Range(builder->getIRBuilder(), builder->getIRBuilder()->CreateLoad(xVar));
			llvm::Value *ny = builder->makeInt32Range(builder->getIRBuilder(), builder->getIRBuilder()->CreateLoad(yVar));
			llvm::Value *nz = builder->makeInt32Range(builder->getIRBuilder(), builder->getIRBuilder()->CreateLoad(zVar));


			llvm::Value *seed;
			if (builder->context().mVectorize) {
				llvm::Value *vectorizedOctave = builder->vectorizeSingleValue(builder->getIRBuilder(), octave);
				seed = builder->getIRBuilder()->CreateAnd(builder->getIRBuilder()->CreateAdd(builder->getIntValue(ridged->seed()), vectorizedOctave), builder->getIntValue(0x7fffffff));
			}
			else {
				seed = builder->getIRBuilder()->CreateAnd(builder->getIRBuilder()->CreateAdd(builder->getIntValue(ridged->seed()), octave), builder->getIntValue(0x7fffffff));
			}
			llvm::Value *signal = builder->gradientCoherentNoise3D(builder->getIRBuilder(), nx, ny, nz, seed);

			signal = builder->fabs(signal);
			signal = builder->fsub(builder->getFPValue(ridged->offset()), signal);
			signal = builder->fmul(signal, signal);
			llvm::Value *weight = builder->load(weightVar);
			signal = builder->fmul(signal, weight);

			weight = builder->fmul(signal, builder->getFPValue(ridged->gain()));
			weight = builder->clampFP(weight, builder->getFPValue(0.0), builder->getFPValue(1.0));
			builder->store(weight, weightVar);



			octave = builder->load(curOctave);
			std::vector<llvm::Value*> gepParams;
			gepParams.push_back(builder->getSingleIntValue(0));
			gepParams.push_back(octave);
			llvm::Value *spectralWeight = builder->load(builder->getIRBuilder()->CreateGEP(spectralWeightsArray, gepParams));
			if (builder->context().mVectorize) {
				spectralWeight = builder->vectorizeSingleValue(builder->getIRBuilder(), spectralWeight);
			}

			llvm::Value *val = builder->getIRBuilder()->CreateLoad(value);
			builder->getIRBuilder()->CreateStore(builder->fadd(val, builder->fmul(signal, spectralWeight)), value);


			x = builder->getIRBuilder()->CreateLoad(xVar);
			y = builder->getIRBuilder()->CreateLoad(yVar);
			z = builder->getIRBuilder()->CreateLoad(zVar);

			builder->getIRBuilder()->CreateStore(builder->fmul(x, builder->getFPValue(ridged->lacunarity())), xVar);
			builder->getIRBuilder()->CreateStore(builder->fmul(y, builder->getFPValue(ridged->lacunarity())), yVar);
			builder->getIRBuilder()->CreateStore(builder->fmul(z, builder->getFPValue(ridged->lacunarity())), zVar);

			builder->getIRBuilder()->CreateStore(builder->getIRBuilder()->CreateAdd(octave, builder->getIRBuilder()->getInt32(1)), curOctave);
			builder->getIRBuilder()->CreateBr(condBB);


			builder->getIRBuilder()->SetInsertPoint(loopEndBB);
			llvm::Value *ret = builder->getIRBuilder()->CreateLoad(value);
			builder->getIRBuilder()->CreateRet(ret);
			delete builder;
		}

		llvm::Function *mFunction;
};


RidgedMulti::RidgedMulti() :
	Module(0),
	mFrequency(RIDGED_DEFAULT_FREQUENCY),
	mLacunarity(RIDGED_DEFAULT_LACUNARITY),
	mSpectralExponentParameter(RIDGED_DEFAULT_SPECTRAL_EXPONENT),
	mOffset(RIDGED_DEFAULT_OFFSET),
	mGain(RIDGED_DEFAULT_GAIN),
	mOctaveCount(RIDGED_DEFAULT_OCTAVE_COUNT),
	mSeed(RIDGED_DEFAULT_SEED),
	mCodeGenerator(new RidgedMultiNoiseGenerator()){
}

RidgedMulti::~RidgedMulti() {
	delete mCodeGenerator;
}

CodeGenerator *RidgedMulti::codeGenerator() const {
	return mCodeGenerator;
}

void RidgedMulti::calculateSpectralWeights() const {
	double frequency = 1.0;
	for (int i = 0; i < RIDGED_MAX_OCTAVE; i++) {
		// Compute weight for each frequency.
		mSpectralWeights[i] = pow (frequency, -mSpectralExponentParameter);
		frequency *= mLacunarity;
	}
}

}
