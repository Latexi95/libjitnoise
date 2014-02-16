#include "voronoi.h"
#include "../codegenerator.h"
#include "../mathconsts.h"
namespace jitnoise {

class VoronoiNoiseGenerator : public CodeGenerator {
	public:
		VoronoiNoiseGenerator() : mFunction(0) {}
		llvm::Value *generate(Builder *builder, const Module *module, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *generationData) {
			const Voronoi *voronoi = static_cast<const Voronoi*>(module);
			if (!mFunction) generateFunction(builder, voronoi);
			return builder->createModuleFunctionCall(mFunction, x, y, z, generationData);
		}

	private:
		void generateFunction(Builder *tempBuilder, const Voronoi *voronoi) {
			llvm::Value *x;
			llvm::Value *y;
			llvm::Value *z;
			llvm::Value *generationData;
			llvm::BasicBlock *basicBlock;
			Builder *builder = tempBuilder->createModuleFunction("voronoi", &x, &y, &z, &generationData, &basicBlock);
			mFunction = builder->function();

			llvm::Value *freq = builder->getFPValue(voronoi->frequency());
			x = builder->fmul(x, freq);
			y = builder->fmul(y, freq);
			z = builder->fmul(z, freq);

			llvm::Value *xi = builder->getIRBuilder()->CreateFPToSI(x, builder->getIntType());
			llvm::Value *yi = builder->getIRBuilder()->CreateFPToSI(y, builder->getIntType());
			llvm::Value *zi = builder->getIRBuilder()->CreateFPToSI(z, builder->getIntType());

			llvm::Value *xInt = builder->getIRBuilder()->CreateSelect(builder->getIRBuilder()->CreateFCmpUGT(x, builder->getFPValue(0.0)), xi, builder->getIRBuilder()->CreateSub(xi, builder->getIntValue(1)));
			llvm::Value *yInt = builder->getIRBuilder()->CreateSelect(builder->getIRBuilder()->CreateFCmpUGT(y, builder->getFPValue(0.0)), yi, builder->getIRBuilder()->CreateSub(yi, builder->getIntValue(1)));
			llvm::Value *zInt = builder->getIRBuilder()->CreateSelect(builder->getIRBuilder()->CreateFCmpUGT(z, builder->getFPValue(0.0)), zi, builder->getIRBuilder()->CreateSub(zi, builder->getIntValue(1)));

			llvm::AllocaInst *minDistVar = builder->getIRBuilder()->CreateAlloca(builder->getFPType());
			llvm::AllocaInst *xCandidateVar = builder->getIRBuilder()->CreateAlloca(builder->getFPType());
			llvm::AllocaInst *yCandidateVar = builder->getIRBuilder()->CreateAlloca(builder->getFPType());
			llvm::AllocaInst *zCandidateVar = builder->getIRBuilder()->CreateAlloca(builder->getFPType());
			builder->store(builder->getFPValue(2147483647.0), minDistVar);
			builder->store(builder->getFPValue(0), xCandidateVar);
			builder->store(builder->getFPValue(0), yCandidateVar);
			builder->store(builder->getFPValue(0), zCandidateVar);

			llvm::AllocaInst *xCurVar = builder->getIRBuilder()->CreateAlloca(builder->getSingleIntType());
			llvm::AllocaInst *yCurVar = builder->getIRBuilder()->CreateAlloca(builder->getSingleIntType());
			llvm::AllocaInst *zCurVar = builder->getIRBuilder()->CreateAlloca(builder->getSingleIntType());
			builder->store(builder->getIRBuilder()->CreateSub(xInt, builder->getIntValue(2)), xCurVar);

			llvm::BasicBlock *xLoopCondBB = builder->createBasicBlock("voronoiXLoopCondBB");
			llvm::BasicBlock *xLoopBodyBB = builder->createBasicBlock("voronoiXLoopBodyBB");
			llvm::BasicBlock *yLoopCondBB = builder->createBasicBlock("voronoiYLoopCondBB");
			llvm::BasicBlock *yLoopBodyBB = builder->createBasicBlock("voronoiYLoopBodyBB");
			llvm::BasicBlock *zLoopCondBB = builder->createBasicBlock("voronoiZLoopCondBB");
			llvm::BasicBlock *zLoopBodyBB = builder->createBasicBlock("voronoiZLoopBodyBB");
			llvm::BasicBlock *zLoopBodyEndBB = builder->createBasicBlock("voronoiXLoopBodyEnd");
			llvm::BasicBlock *yLoopBodyEndBB = builder->createBasicBlock("voronoiYLoopBodyEnd");
			llvm::BasicBlock *xLoopBodyEndBB = builder->createBasicBlock("voronoiXLoopBodyEnd");
			llvm::BasicBlock *loopEndBB = builder->createBasicBlock("voronoiLoopEndBB");

			llvm::Value *xLimit = builder->getIRBuilder()->CreateAdd(xInt, builder->getSingleIntValue(2));
			llvm::Value *yLimit = builder->getIRBuilder()->CreateAdd(yInt, builder->getSingleIntValue(2));
			llvm::Value *zLimit = builder->getIRBuilder()->CreateAdd(zInt, builder->getSingleIntValue(2));
			builder->branch(xLoopCondBB);

			builder->setInsertPoint(xLoopCondBB);
			llvm::Value *xCur = builder->load(xCurVar);
			llvm::Value *cond = builder->getIRBuilder()->CreateICmpSLE(xCur, xLimit);
			builder->conditionalBranch(cond, xLoopBodyBB, loopEndBB);

			builder->setInsertPoint(xLoopBodyBB);
			builder->store(builder->getIRBuilder()->CreateSub(yInt, builder->getIntValue(2)), yCurVar);
			builder->branch(yLoopCondBB);

			builder->setInsertPoint(yLoopCondBB);
			llvm::Value *yCur = builder->load(yCurVar);
			cond = builder->getIRBuilder()->CreateICmpSLE(yCur, yLimit);
			builder->conditionalBranch(cond, yLoopBodyBB, xLoopBodyEndBB);

			builder->setInsertPoint(yLoopBodyBB);
			builder->store(builder->getIRBuilder()->CreateSub(zInt, builder->getIntValue(2)), zCurVar);
			builder->branch(zLoopCondBB);

			builder->setInsertPoint(zLoopCondBB);
			llvm::Value *zCur = builder->load(zCurVar);
			cond = builder->getIRBuilder()->CreateICmpSLE(zCur, zLimit);
			builder->conditionalBranch(cond, zLoopBodyBB, yLoopBodyEndBB);

			builder->setInsertPoint(zLoopBodyBB);

			llvm::Value *fxCur = builder->getIRBuilder()->CreateSIToFP(xCur, builder->getFPType());
			llvm::Value *fyCur = builder->getIRBuilder()->CreateSIToFP(yCur, builder->getFPType());
			llvm::Value *fzCur = builder->getIRBuilder()->CreateSIToFP(zCur, builder->getFPType());
			llvm::Value *xPos = builder->fadd(fxCur, builder->valueNoise3D(xCur, yCur, zCur, builder->getIntValue(voronoi->seed())));
			llvm::Value *yPos = builder->fadd(fyCur, builder->valueNoise3D(xCur, yCur, zCur, builder->getIntValue(voronoi->seed() + 1)));
			llvm::Value *zPos = builder->fadd(fzCur, builder->valueNoise3D(xCur, yCur, zCur, builder->getIntValue(voronoi->seed() + 2)));

			llvm::Value *xDist = builder->fsub(xPos, x);
			llvm::Value *yDist = builder->fsub(yPos, y);
			llvm::Value *zDist = builder->fsub(zPos, z);
			llvm::Value *dist = builder->fadd(builder->fmul(xDist, xDist), builder->fadd(builder->fmul(yDist, yDist), builder->fmul(zDist, zDist)));
			if (builder->context().mVectorize) {
				llvm::BasicBlock *basicBlocks[4 * 2];
				llvm::BasicBlock *basicBlockAfter = zLoopBodyEndBB;
				basicBlocks[7] = zLoopBodyEndBB;
				llvm::Value *minDist = builder->load(minDistVar);
				for (int i = 6; i >= 0; i--) {
					basicBlocks[i] = builder->createBasicBlock("newMinDistBB", basicBlockAfter);
					basicBlockAfter = basicBlocks[i];
				}
				for (int i = 0; i < 4; i++) {
					llvm::Value *distPart = builder->getIRBuilder()->CreateExtractElement(dist, builder->getSingleIntValue(i));
					cond = builder->getIRBuilder()->CreateFCmpOLT(distPart, builder->getIRBuilder()->CreateExtractElement(minDist, builder->getSingleIntValue(i)));
					builder->conditionalBranch(cond, basicBlocks[i * 2], basicBlocks[i * 2 + 1]);

					builder->setInsertPoint(basicBlocks[i * 2]);
					minDist = builder->load(minDistVar);
					llvm::Value *xCandidate = builder->load(xCandidateVar);
					llvm::Value *yCandidate = builder->load(yCandidateVar);
					llvm::Value *zCandidate = builder->load(zCandidateVar);

					builder->store(builder->getIRBuilder()->CreateInsertElement(minDist, distPart, builder->getSingleIntValue(i)), minDistVar);
					builder->store(builder->getIRBuilder()->CreateInsertElement(
									   xCandidate, builder->getIRBuilder()->CreateExtractElement(xPos, builder->getSingleIntValue(i)), builder->getSingleIntValue(i)), xCandidate);
					builder->store(builder->getIRBuilder()->CreateInsertElement(
									   yCandidate, builder->getIRBuilder()->CreateExtractElement(yPos, builder->getSingleIntValue(i)), builder->getSingleIntValue(i)), yCandidate);
					builder->store(builder->getIRBuilder()->CreateInsertElement(
									   zCandidate, builder->getIRBuilder()->CreateExtractElement(zPos, builder->getSingleIntValue(i)), builder->getSingleIntValue(i)), zCandidate);
					builder->branch(basicBlocks[i * 2 + 1]);
					builder->setInsertPoint(basicBlocks[i * 2 + 1]);
				}
			}
			else {
				llvm::BasicBlock *newMinDistBB = builder->createBasicBlock("voronoiNewMinDistBB", zLoopBodyBB);
				cond = builder->getIRBuilder()->CreateFCmpOLT(dist, builder->load(minDistVar));
				builder->conditionalBranch(cond, newMinDistBB, zLoopBodyEndBB);

				builder->setInsertPoint(newMinDistBB);
				builder->store(dist, minDistVar);
				builder->store(xPos, xCandidateVar);
				builder->store(yPos, yCandidateVar);
				builder->store(zPos, zCandidateVar);
				builder->branch(zLoopBodyEndBB);
			}

			builder->setInsertPoint(zLoopBodyEndBB);
			builder->store(builder->getIRBuilder()->CreateAdd(zCur, builder->getSingleIntValue(1)), zCurVar);
			builder->branch(zLoopCondBB);

			builder->setInsertPoint(yLoopBodyEndBB);
			builder->store(builder->getIRBuilder()->CreateAdd(yCur, builder->getSingleIntValue(1)), yCurVar);
			builder->branch(yLoopCondBB);

			builder->setInsertPoint(xLoopBodyEndBB);
			builder->store(builder->getIRBuilder()->CreateAdd(xCur, builder->getSingleIntValue(1)), xCurVar);
			builder->branch(xLoopCondBB);

			builder->setInsertPoint(loopEndBB);

			llvm::Value *xCandidate = builder->load(xCandidateVar);
			llvm::Value *yCandidate = builder->load(yCandidateVar);
			llvm::Value *zCandidate = builder->load(zCandidateVar);
			llvm::Value *baseVal;
			if (voronoi->isDistanceEnabled()) {
				llvm::Value *xDist = builder->fsub(xCandidate, x);
				llvm::Value *yDist = builder->fsub(yCandidate, y);
				llvm::Value *zDist = builder->fsub(zCandidate, z);
				llvm::Value *distSq = builder->fadd(builder->fmul(xDist, xDist), builder->fadd(builder->fmul(yDist, yDist), builder->fmul(zDist, zDist)));
				baseVal = builder->fsub(builder->fmul(builder->fsqrt(distSq), builder->getFPValue(SQRT_3)), builder->getFPValue(1.0));
			}
			else {
				baseVal = builder->getFPValue(0.0);
			}

			//TODO: floor?
			llvm::Value *xCandidateFloored = builder->getIRBuilder()->CreateFPToSI(xCandidate, builder->getSingleIntType());
			llvm::Value *yCandidateFloored = builder->getIRBuilder()->CreateFPToSI(yCandidate, builder->getSingleIntType());
			llvm::Value *zCandidateFloored = builder->getIRBuilder()->CreateFPToSI(zCandidate, builder->getSingleIntType());
			llvm::Value *ret = builder->fadd(baseVal, builder->fmul(builder->getFPValue(voronoi->displacement()),
																	builder->valueNoise3D(xCandidateFloored, yCandidateFloored, zCandidateFloored, builder->getIntValue(0))));

			builder->getIRBuilder()->CreateRet(ret);
			delete builder;
		}

		llvm::Function *mFunction;
};


Voronoi::Voronoi() :
	Module(0),
	mFrequency(VORONOI_DEFAULT_FREQUENCY),
	mDisplacement(VORONOI_DEFAULT_DISPLACEMENT),
	mEnableDistance(false),
	mSeed(VORONOI_DEFAULT_SEED),
	mCodeGenerator(new VoronoiNoiseGenerator()){
}

Voronoi::~Voronoi() {
	delete mCodeGenerator;
}

CodeGenerator *Voronoi::codeGenerator() const {
	return mCodeGenerator;
}

}
