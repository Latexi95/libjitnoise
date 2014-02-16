#include "billow.h"
#include "../codegenerator.h"
namespace jitnoise {

class BillowNoiseGenerator : public CodeGenerator {
	public:
		BillowNoiseGenerator() : mFunction(0) {}
		llvm::Value *generate(Builder *builder, const Module *module, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *generationData) {
			const Billow *billow = static_cast<const Billow*>(module);
			if (!mFunction) generateFunction(builder, billow);
			return builder->createModuleFunctionCall(mFunction, x, y, z, generationData);
		}

	private:
		void generateFunction(Builder *tempBuilder, const Billow *billow) {
			llvm::Value *x;
			llvm::Value *y;
			llvm::Value *z;
			llvm::Value *generationData;
			llvm::BasicBlock *basicBlock;
			Builder *builder = tempBuilder->createModuleFunction("billow", &x, &y, &z, &generationData, &basicBlock);
			mFunction = builder->function();

			llvm::AllocaInst * value = builder->getIRBuilder()->CreateAlloca(builder->getFPType());
			llvm::AllocaInst * xVar = builder->getIRBuilder()->CreateAlloca(builder->getFPType());
			llvm::AllocaInst * yVar = builder->getIRBuilder()->CreateAlloca(builder->getFPType());
			llvm::AllocaInst * zVar = builder->getIRBuilder()->CreateAlloca(builder->getFPType());
			llvm::AllocaInst * curPersistence = builder->getIRBuilder()->CreateAlloca(builder->getSingleFPType());
			llvm::AllocaInst *curOctave = builder->getIRBuilder()->CreateAlloca(builder->getSingleIntType());
			builder->getIRBuilder()->CreateStore(builder->getFPValue(0), value);
			builder->getIRBuilder()->CreateStore(builder->fmul(x, builder->getFPValue(billow->frequency())), xVar);
			builder->getIRBuilder()->CreateStore(builder->fmul(y, builder->getFPValue(billow->frequency())), yVar);
			builder->getIRBuilder()->CreateStore(builder->fmul(z, builder->getFPValue(billow->frequency())), zVar);
			builder->getIRBuilder()->CreateStore(llvm::ConstantFP::get(builder->getSingleFPType(), 1.0), curPersistence);
			builder->getIRBuilder()->CreateStore(builder->getIRBuilder()->getInt32(0), curOctave);

			llvm::BasicBlock *condBB = builder->createBasicBlock("billowForCond");
			llvm::BasicBlock *loopBodyBB = builder->createBasicBlock("billowForBody");
			llvm::BasicBlock *loopEndBB = builder->createBasicBlock("billowForEnd");

			builder->getIRBuilder()->CreateBr(condBB);
			builder->getIRBuilder()->SetInsertPoint(condBB);
			llvm::Value *octave = builder->getIRBuilder()->CreateLoad(curOctave);
			llvm::Value *cond = builder->getIRBuilder()->CreateICmpULT(octave, builder->getIRBuilder()->getInt32(billow->octaveCount()));
			builder->getIRBuilder()->CreateCondBr(cond, loopBodyBB, loopEndBB);

			builder->getIRBuilder()->SetInsertPoint(loopBodyBB);
			llvm::Value *nx = builder->makeInt32Range(builder->getIRBuilder(), builder->getIRBuilder()->CreateLoad(xVar));
			llvm::Value *ny = builder->makeInt32Range(builder->getIRBuilder(), builder->getIRBuilder()->CreateLoad(yVar));
			llvm::Value *nz = builder->makeInt32Range(builder->getIRBuilder(), builder->getIRBuilder()->CreateLoad(zVar));

			builder->debugPrintDouble(builder->getIRBuilder(), 7, builder->getIRBuilder()->CreateSIToFP(octave, builder->getSingleFPType()));


			llvm::Value *seed;
			if (builder->context().mVectorize) {
				llvm::Value *vectorizedOctave = builder->vectorizeSingleValue(builder->getIRBuilder(), octave);
				seed = builder->getIRBuilder()->CreateAnd(builder->getIRBuilder()->CreateAdd(builder->getIntValue(billow->seed()), vectorizedOctave), builder->getIntValue(0xffffffff));
			}
			else {
				seed = builder->getIRBuilder()->CreateAnd(builder->getIRBuilder()->CreateAdd(builder->getIntValue(billow->seed()), octave), builder->getIntValue(0xffffffff));
			}
			llvm::Value *signal = builder->gradientCoherentNoise3D(builder->getIRBuilder(), nx, ny, nz, seed);
			signal = builder->fsub(builder->fmul(builder->fabs(signal), builder->getFPValue(2.0)), builder->getFPValue(1.0));


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

			builder->getIRBuilder()->CreateStore(builder->fmul(x, builder->getFPValue(billow->lacunarity())), xVar);
			builder->getIRBuilder()->CreateStore(builder->fmul(y, builder->getFPValue(billow->lacunarity())), yVar);
			builder->getIRBuilder()->CreateStore(builder->fmul(z, builder->getFPValue(billow->lacunarity())), zVar);

			builder->getIRBuilder()->CreateStore(builder->fmul(persistence, llvm::ConstantFP::get(builder->getSingleFPType(), billow->persistence())), curPersistence);


			octave = builder->getIRBuilder()->CreateLoad	(curOctave);
			builder->getIRBuilder()->CreateStore(builder->getIRBuilder()->CreateAdd(octave, builder->getIRBuilder()->getInt32(1)), curOctave);
			builder->getIRBuilder()->CreateBr(condBB);

			builder->getIRBuilder()->SetInsertPoint(loopEndBB);


			llvm::Value *ret = builder->fadd(builder->getIRBuilder()->CreateLoad(value), builder->getFPValue(0.5));
			builder->getIRBuilder()->CreateRet(ret);
			delete builder;
		}

		llvm::Function *mFunction;
};


Billow::Billow() :
	Module(0),
	mFrequency(BILLOW_DEFAULT_FREQUENCY),
	mLacunarity(BILLOW_DEFAULT_LACUNARITY),
	mOctaveCount(BILLOW_DEFAULT_OCTAVE_COUNT),
	mPersistence(BILLOW_DEFAULT_PERSISTENCE),
	mSeed(BILLOW_DEFAULT_SEED),
	mCodeGenerator(new BillowNoiseGenerator()){
}

Billow::~Billow() {
	delete mCodeGenerator;
}

CodeGenerator *Billow::codeGenerator() const {
	return mCodeGenerator;
}

}
