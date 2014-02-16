#include "terrace.h"
#include "../builder.h"
#include "../codegenerator.h"
namespace jitnoise {

class TerraceGenerator : public CodeGenerator {
	public:
		TerraceGenerator() : mFunction(0) { }

		llvm::Value *generate(Builder *builder, const Module *module, llvm::Value *px, llvm::Value *py, llvm::Value *pz, llvm::Value *generationData) {
			const Terrace *terraceModule = static_cast<const Terrace*>(module);
			if (!mFunction) generateFunction(builder, terraceModule);
			return builder->createModuleFunctionCall(mFunction, px, py, pz, generationData);
		}

	private:
		void generateFunction(Builder *builder, const Terrace *terraceModule) {
			const std::vector<double> &cps = terraceModule->controlPoints();
			assert(cps.size() >= 2);
			llvm::Value *x;
			llvm::Value *y;
			llvm::Value *z;
			llvm::BasicBlock *bb;
			llvm::Value *generationData;
			Builder *fb = builder->createModuleFunction("terrace", &x, &y, &z, &generationData, &bb);
			mFunction = fb->function();
			llvm::ArrayType *controlValueArrayType = llvm::ArrayType::get(fb->getSingleFPType(), cps.size());
			std::vector<llvm::Constant*> controlValues;
			for (std::vector<double>::const_iterator i = cps.begin(); i != cps.end(); i++) {
				controlValues.push_back(llvm::ConstantFP::get(fb->getSingleFPType(), *i));
			}
			llvm::Constant *controlValueArrayValue = llvm::ConstantArray::get(controlValueArrayType, controlValues);
			llvm::GlobalVariable *controlValueArray = new llvm::GlobalVariable(*fb->module(), controlValueArrayType, true, llvm::GlobalVariable::PrivateLinkage, controlValueArrayValue);


			llvm::Value *sourceModuleValue = terraceModule->getSourceModule(0)->generate(fb, x, y, z, generationData);

			if (!fb->context().mVectorize) {
				llvm::Value *indexPosVar = fb->getIRBuilder()->CreateAlloca(fb->getIRBuilder()->getInt32Ty());
				fb->store(fb->getIRBuilder()->getInt32(0), indexPosVar);
				llvm::BasicBlock *condBB = fb->createBasicBlock("TerraceCond");
				fb->getIRBuilder()->CreateBr(condBB);

				fb->getIRBuilder()->SetInsertPoint(condBB);
				llvm::Value *indexPos = fb->load(indexPosVar);
				llvm::Value *cond = fb->getIRBuilder()->CreateICmpULT(indexPos, fb->getSingleIntValue(cps.size()));
				llvm::BasicBlock *blockBB = fb->createBasicBlock("TerraceBlock1");
				llvm::BasicBlock *block2BB = fb->createBasicBlock("TerraceBlock2");
				llvm::BasicBlock *endBB = fb->createBasicBlock("TerraceEnd");
				fb->getIRBuilder()->CreateCondBr(cond, blockBB, endBB);

				fb->getIRBuilder()->SetInsertPoint(blockBB);
				std::vector<llvm::Value*> gepParam;
				gepParam.push_back(fb->getSingleIntValue(0));
				gepParam.push_back(indexPos);
				llvm::Value *controlPointInputValue = fb->load(fb->getIRBuilder()->CreateGEP(controlValueArray, gepParam));
				llvm::Value *cond2 = fb->getIRBuilder()->CreateFCmpOLT(sourceModuleValue, controlPointInputValue);
				fb->getIRBuilder()->CreateCondBr(cond2, endBB, block2BB);


				fb->getIRBuilder()->SetInsertPoint(block2BB);
				fb->store(fb->getIRBuilder()->CreateAdd(indexPos, fb->getSingleIntValue(1)), indexPosVar);
				fb->getIRBuilder()->CreateBr(condBB);

				fb->getIRBuilder()->SetInsertPoint(endBB);

				indexPos = fb->load(indexPosVar);
				llvm::Value *index0 = fb->clampInt(fb->getIRBuilder()->CreateSub(indexPos, fb->getSingleIntValue(1)), fb->getSingleIntValue(0), fb->getSingleIntValue(cps.size() - 1));
				llvm::Value *index1 = fb->clampInt(indexPos, fb->getSingleIntValue(0), fb->getSingleIntValue(cps.size() - 1));

				cond = fb->getIRBuilder()->CreateICmpEQ(index0, index1);
				llvm::BasicBlock *index0index1 = fb->createBasicBlock("index0_eq_index1");
				llvm::BasicBlock *continueBB = fb->createBasicBlock("continue");
				fb->getIRBuilder()->CreateCondBr(cond, index0index1, continueBB);

				fb->getIRBuilder()->SetInsertPoint(index0index1);
				gepParam[1] = index1;
				llvm::Value *output = fb->load(fb->getIRBuilder()->CreateGEP(controlValueArray, gepParam));
				fb->getIRBuilder()->CreateRet(output);

				fb->getIRBuilder()->SetInsertPoint(continueBB);
				gepParam[1] = index0;
				llvm::Value *input0 = fb->load(fb->getIRBuilder()->CreateGEP(controlValueArray, gepParam));
				gepParam[1] = index1;
				llvm::Value *input1 = fb->load(fb->getIRBuilder()->CreateGEP(controlValueArray, gepParam));
				llvm::Value *alpha = fb->fdiv(fb->fsub(sourceModuleValue, input0), fb->fsub(input1, input0));

				if (terraceModule->isTerracedInverted()) {
					alpha = fb->fsub(fb->getFPValue(1.0), alpha);
					std::swap(input0, input1);
				}

				alpha = fb->fmul(alpha, alpha);
				llvm::Value *ret = fb->interpolateLinear(fb->getIRBuilder(), input0, input1, alpha);
				fb->getIRBuilder()->CreateRet(ret);
				return;
			}
			else {
				llvm::Value *ret = llvm::UndefValue::get(fb->getFPType());
				llvm::Value *retVar = fb->getIRBuilder()->CreateAlloca(fb->getFPType());
				fb->getIRBuilder()->CreateStore(ret, retVar);
				const int basicBlockCount = 4 * 6 + 1;
				llvm::BasicBlock *basicBlocks[basicBlockCount];
				for (int i = 0; i < basicBlockCount; i++) {
					basicBlocks[i] = fb->createBasicBlock("bb1");
				}

				llvm::Value *indexPosVar = fb->getIRBuilder()->CreateAlloca(fb->getIRBuilder()->getInt32Ty());
				int basicBlockIndex = 0;
				for (int i = 0; i < 4; i++) {
					fb->store(fb->getIRBuilder()->getInt32(0), indexPosVar);
					llvm::BasicBlock *condBB = basicBlocks[basicBlockIndex];
					condBB->setName("condBB");
					fb->getIRBuilder()->CreateBr(condBB);

					fb->getIRBuilder()->SetInsertPoint(condBB);
					llvm::Value *indexPos = fb->load(indexPosVar);
					llvm::Value  *sourceModuleElementValue = fb->getIRBuilder()->CreateExtractElement(sourceModuleValue, fb->getSingleIntValue(i));
					llvm::Value *cond = fb->getIRBuilder()->CreateICmpULT(indexPos, fb->getSingleIntValue(cps.size()));
					llvm::BasicBlock *blockBB = basicBlocks[++basicBlockIndex];
					blockBB->setName("blockBB");
					llvm::BasicBlock *block2BB = basicBlocks[++basicBlockIndex];
					block2BB->setName("block2BB");
					llvm::BasicBlock *endBB = basicBlocks[++basicBlockIndex];
					endBB->setName("endBB");
					fb->getIRBuilder()->CreateCondBr(cond, blockBB, endBB);

					fb->getIRBuilder()->SetInsertPoint(blockBB);
					std::vector<llvm::Value*> gepParam;
					gepParam.push_back(fb->getSingleIntValue(0));
					gepParam.push_back(indexPos);
					llvm::Value *controlPointInputValue = fb->load(fb->getIRBuilder()->CreateGEP(controlValueArray, gepParam));
					llvm::Value *cond2 = fb->getIRBuilder()->CreateFCmpOLT(sourceModuleElementValue, controlPointInputValue);
					fb->getIRBuilder()->CreateCondBr(cond2, endBB, block2BB);


					fb->getIRBuilder()->SetInsertPoint(block2BB);
					fb->store(fb->getIRBuilder()->CreateAdd(indexPos, fb->getSingleIntValue(1)), indexPosVar);
					fb->getIRBuilder()->CreateBr(condBB);

					fb->getIRBuilder()->SetInsertPoint(endBB);

					indexPos = fb->load(indexPosVar);
					llvm::Value *index0 = fb->clampInt(fb->getIRBuilder()->CreateSub(indexPos, fb->getSingleIntValue(1)), fb->getSingleIntValue(0), fb->getSingleIntValue(cps.size() - 1));
					llvm::Value *index1 = fb->clampInt(indexPos, fb->getSingleIntValue(0), fb->getSingleIntValue(cps.size() - 1));

					cond = fb->getIRBuilder()->CreateICmpEQ(index0, index1);
					llvm::BasicBlock *index1index2BB = basicBlocks[++basicBlockIndex];
					index1index2BB->setName("index1index2BB");
					llvm::BasicBlock *continueBB = basicBlocks[++basicBlockIndex];
					continueBB->setName("continueBB");
					fb->getIRBuilder()->CreateCondBr(cond, index1index2BB, continueBB);

					fb->getIRBuilder()->SetInsertPoint(index1index2BB);
					gepParam[1] = index1;
					llvm::Value *output = fb->load(fb->getIRBuilder()->CreateGEP(controlValueArray, gepParam));
					ret = fb->load(retVar);
					fb->store(fb->getIRBuilder()->CreateInsertElement(ret, output, fb->getSingleIntValue(i)), retVar);
					fb->getIRBuilder()->CreateBr(basicBlocks[basicBlockIndex + 1]);


					fb->getIRBuilder()->SetInsertPoint(continueBB);
					gepParam[1] = index0;
					llvm::Value *input0 = fb->load(fb->getIRBuilder()->CreateGEP(controlValueArray, gepParam));
					gepParam[1] = index1;
					llvm::Value *input1 = fb->load(fb->getIRBuilder()->CreateGEP(controlValueArray, gepParam));
					llvm::Value *alpha = fb->fdiv(fb->fsub(sourceModuleElementValue, input0), fb->fsub(input1, input0));

					if (terraceModule->isTerracedInverted()) {
						alpha = builder->fsub(fb->getFPValue(1.0), alpha);
						std::swap(input0, input1);
					}
					alpha = fb->fmul(alpha, alpha);

					llvm::Value *retVal = fb->interpolateLinear(fb->getIRBuilder(),input0, input1, alpha);
					ret = fb->getIRBuilder()->CreateLoad(retVar);
					fb->getIRBuilder()->CreateStore(fb->getIRBuilder()->CreateInsertElement(ret, retVal, fb->getSingleIntValue(i)), retVar);
					basicBlockIndex++;
				}
				fb->getIRBuilder()->CreateBr(basicBlocks[basicBlockIndex]);
				llvm::BasicBlock *returnBB = basicBlocks[basicBlockIndex];
				fb->getIRBuilder()->SetInsertPoint(returnBB);
				fb->getIRBuilder()->CreateRet(fb->load(retVar));
			}
			delete fb;
		}

		llvm::Function *mFunction;
};

Terrace::Terrace() : Module(1), mCodeGenerator(new TerraceGenerator()), mInvertTerraces(false) {
}

Terrace::~Terrace() {
	delete mCodeGenerator;
}

void Terrace::addControlPoint(double controlPoint) {
	for (std::vector<double>::iterator i = mControlPoints.begin(); i != mControlPoints.end(); i++) {
		assert(*i != controlPoint);
		if (*i > controlPoint) {
			mControlPoints.insert(i, controlPoint);
			return;
		}
	}
	mControlPoints.push_back(controlPoint);
}

void Terrace::clearControlPoints() {
	mControlPoints.clear();
}

void Terrace::makeControlPoints(int controlPointCount) {
	assert(controlPointCount < 2);
	clearControlPoints();

	double terraceStep = 2.0 / (double)(controlPointCount - 1);
	double curValue = -1.0;
	for (int i = 0; i < controlPointCount; i++) {
		mControlPoints.push_back(curValue);
		curValue += terraceStep;
	}

}


}
