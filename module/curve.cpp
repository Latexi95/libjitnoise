#include "curve.h"
#include "../builder.h"
#include "../codegenerator.h"
namespace jitnoise {

class CurveGenerator : public CodeGenerator {
	public:
		CurveGenerator() : mFunction(0) { }

		llvm::Value *generate(Builder *builder, const Module *module, llvm::Value *px, llvm::Value *py, llvm::Value *pz, llvm::Value *generationData) {
			const Curve *curveModule = static_cast<const Curve*>(module);
			if (!mFunction) generateFunction(builder, curveModule);
			return builder->createModuleFunctionCall(mFunction, px, py, pz, generationData);
		}

	private:
		void generateFunction(Builder *builder, const Curve *curveModule) {
			const std::vector<Curve::ControlPoint> &cps = curveModule->controlPoints();
			assert(cps.size() >= 4);
			llvm::Value *x;
			llvm::Value *y;
			llvm::Value *z;
			llvm::BasicBlock *bb;
			llvm::Value *generationData;
			Builder *fb = builder->createModuleFunction("curve", &x, &y, &z, &generationData, &bb);
			mFunction = fb->function();
			llvm::ArrayType *controlValueArrayType = llvm::ArrayType::get(fb->getSingleFPType(), cps.size());
			std::vector<llvm::Constant*> inputValues;
			std::vector<llvm::Constant*> outputValues;
			for (std::vector<Curve::ControlPoint>::const_iterator i = cps.begin(); i != cps.end(); i++) {
				inputValues.push_back(llvm::ConstantFP::get(fb->getSingleFPType(), i->inputValue));
				outputValues.push_back(llvm::ConstantFP::get(fb->getSingleFPType(), i->outputValue));
			}
			llvm::Constant *inputValueArrayValue = llvm::ConstantArray::get(controlValueArrayType, inputValues);
			llvm::Constant *outputValueArrayValue = llvm::ConstantArray::get(controlValueArrayType, outputValues);
			llvm::GlobalVariable *inputValueArray = new llvm::GlobalVariable(*fb->module(), controlValueArrayType, true, llvm::GlobalVariable::PrivateLinkage, inputValueArrayValue);
			llvm::GlobalVariable *outputValueArray = new llvm::GlobalVariable(*fb->module(), controlValueArrayType, true, llvm::GlobalVariable::PrivateLinkage, outputValueArrayValue);


			llvm::Value *sourceModuleValue = curveModule->getSourceModule(0)->generate(fb, x, y, z, generationData);

			if (!fb->context().mVectorize) {
				llvm::Value *indexPosVar = fb->getIRBuilder()->CreateAlloca(fb->getIRBuilder()->getInt32Ty());
				fb->store(fb->getIRBuilder()->getInt32(0), indexPosVar);
				llvm::BasicBlock *condBB = fb->createBasicBlock("curveCond");
				fb->getIRBuilder()->CreateBr(condBB);

				fb->getIRBuilder()->SetInsertPoint(condBB);
				llvm::Value *indexPos = fb->load(indexPosVar);
				llvm::Value *cond = fb->getIRBuilder()->CreateICmpULT(indexPos, fb->getSingleIntValue(cps.size()));
				llvm::BasicBlock *blockBB = fb->createBasicBlock("curveBlock1");
				llvm::BasicBlock *block2BB = fb->createBasicBlock("curveBlock2");
				llvm::BasicBlock *endBB = fb->createBasicBlock("curveEnd");
				fb->getIRBuilder()->CreateCondBr(cond, blockBB, endBB);

				fb->getIRBuilder()->SetInsertPoint(blockBB);
				std::vector<llvm::Value*> gepParam;
				gepParam.push_back(fb->getSingleIntValue(0));
				gepParam.push_back(indexPos);
				llvm::Value *controlPointInputValue = fb->load(fb->getIRBuilder()->CreateGEP(inputValueArray, gepParam));
				llvm::Value *cond2 = fb->getIRBuilder()->CreateFCmpOLT(sourceModuleValue, controlPointInputValue);
				fb->getIRBuilder()->CreateCondBr(cond2, endBB, block2BB);


				fb->getIRBuilder()->SetInsertPoint(block2BB);
				fb->store(fb->getIRBuilder()->CreateAdd(indexPos, fb->getSingleIntValue(1)), indexPosVar);
				fb->getIRBuilder()->CreateBr(condBB);

				fb->getIRBuilder()->SetInsertPoint(endBB);

				indexPos = fb->load(indexPosVar);
				llvm::Value *index0 = fb->clampInt(fb->getIRBuilder()->CreateSub(indexPos, fb->getSingleIntValue(2)), fb->getSingleIntValue(0), fb->getSingleIntValue(cps.size() - 1));
				llvm::Value *index1 = fb->clampInt(fb->getIRBuilder()->CreateSub(indexPos, fb->getSingleIntValue(1)), fb->getSingleIntValue(0), fb->getSingleIntValue(cps.size() - 1));
				llvm::Value *index2 = fb->clampInt(indexPos, fb->getSingleIntValue(0), fb->getSingleIntValue(cps.size() - 1));
				llvm::Value *index3 = fb->clampInt(fb->getIRBuilder()->CreateAdd(indexPos, fb->getSingleIntValue(1)), fb->getSingleIntValue(0), fb->getSingleIntValue(cps.size() - 1));

				cond = fb->getIRBuilder()->CreateICmpEQ(index1, index2);
				llvm::BasicBlock *index1index2 = fb->createBasicBlock("index1_eq_index2");
				llvm::BasicBlock *continueBB = fb->createBasicBlock("continue");
				fb->getIRBuilder()->CreateCondBr(cond, index1index2, continueBB);

				fb->getIRBuilder()->SetInsertPoint(index1index2);
				gepParam[1] = index1;
				llvm::Value *output1 = fb->load(fb->getIRBuilder()->CreateGEP(outputValueArray, gepParam));
				fb->getIRBuilder()->CreateRet(output1);

				fb->getIRBuilder()->SetInsertPoint(continueBB);
				gepParam[1] = index1;
				llvm::Value *input0 = fb->load(fb->getIRBuilder()->CreateGEP(inputValueArray, gepParam));
				gepParam[1] = index2;
				llvm::Value *input1 = fb->load(fb->getIRBuilder()->CreateGEP(inputValueArray, gepParam));
				llvm::Value *alpha = fb->fdiv(fb->fsub(sourceModuleValue, input0), fb->fsub(input1, input0));

				gepParam[1] = index0;
				llvm::Value *output0 = fb->load(fb->getIRBuilder()->CreateGEP(outputValueArray, gepParam));
				gepParam[1] = index1;
				output1 = fb->load(fb->getIRBuilder()->CreateGEP(outputValueArray, gepParam));
				gepParam[1] = index2;
				llvm::Value *output2 = fb->load(fb->getIRBuilder()->CreateGEP(outputValueArray, gepParam));
				gepParam[1] = index3;
				llvm::Value *output3 = fb->load(fb->getIRBuilder()->CreateGEP(outputValueArray, gepParam));
				llvm::Value *ret = fb->interpolateCubic(fb->getIRBuilder(),
									 output0,
									 output1,
									 output2,
									 output3,
									 alpha
									 );
				fb->getIRBuilder()->CreateRet(ret);
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
					llvm::Value *controlPointInputValue = fb->load(fb->getIRBuilder()->CreateGEP(inputValueArray, gepParam));
					llvm::Value *cond2 = fb->getIRBuilder()->CreateFCmpOLT(sourceModuleElementValue, controlPointInputValue);
					fb->getIRBuilder()->CreateCondBr(cond2, endBB, block2BB);


					fb->getIRBuilder()->SetInsertPoint(block2BB);
					fb->store(fb->getIRBuilder()->CreateAdd(indexPos, fb->getSingleIntValue(1)), indexPosVar);
					fb->getIRBuilder()->CreateBr(condBB);

					fb->getIRBuilder()->SetInsertPoint(endBB);

					indexPos = fb->load(indexPosVar);
					llvm::Value *index0 = fb->clampInt(fb->getIRBuilder()->CreateSub(indexPos, fb->getSingleIntValue(2)), fb->getSingleIntValue(0), fb->getSingleIntValue(cps.size() - 1));
					llvm::Value *index1 = fb->clampInt(fb->getIRBuilder()->CreateSub(indexPos, fb->getSingleIntValue(1)), fb->getSingleIntValue(0), fb->getSingleIntValue(cps.size() - 1));
					llvm::Value *index2 = fb->clampInt(indexPos, fb->getSingleIntValue(0), fb->getSingleIntValue(cps.size() - 1));
					llvm::Value *index3 = fb->clampInt(fb->getIRBuilder()->CreateAdd(indexPos, fb->getSingleIntValue(1)), fb->getSingleIntValue(0), fb->getSingleIntValue(cps.size() - 1));

					cond = fb->getIRBuilder()->CreateICmpEQ(index1, index2);
					llvm::BasicBlock *index1index2BB = basicBlocks[++basicBlockIndex];
					index1index2BB->setName("index1index2BB");
					llvm::BasicBlock *continueBB = basicBlocks[++basicBlockIndex];
					continueBB->setName("continueBB");
					fb->getIRBuilder()->CreateCondBr(cond, index1index2BB, continueBB);

					fb->getIRBuilder()->SetInsertPoint(index1index2BB);
					gepParam[1] = index1;
					llvm::Value *output1 = fb->load(fb->getIRBuilder()->CreateGEP(outputValueArray, gepParam));
					ret = fb->load(retVar);
					fb->store(fb->getIRBuilder()->CreateInsertElement(ret, output1, fb->getSingleIntValue(i)), retVar);
					fb->getIRBuilder()->CreateBr(basicBlocks[basicBlockIndex + 1]);


					fb->getIRBuilder()->SetInsertPoint(continueBB);
					gepParam[1] = index1;
					llvm::Value *input0 = fb->load(fb->getIRBuilder()->CreateGEP(inputValueArray, gepParam));
					gepParam[1] = index2;
					llvm::Value *input1 = fb->load(fb->getIRBuilder()->CreateGEP(inputValueArray, gepParam));
					llvm::Value *alpha = fb->fdiv(fb->fsub(sourceModuleElementValue, input0), fb->fsub(input1, input0));

					gepParam[1] = index0;
					llvm::Value *output0 = fb->load(fb->getIRBuilder()->CreateGEP(outputValueArray, gepParam));
					gepParam[1] = index1;
					output1 = fb->load(fb->getIRBuilder()->CreateGEP(outputValueArray, gepParam));
					gepParam[1] = index2;
					llvm::Value *output2 = fb->load(fb->getIRBuilder()->CreateGEP(outputValueArray, gepParam));
					gepParam[1] = index3;
					llvm::Value *output3 = fb->load(fb->getIRBuilder()->CreateGEP(outputValueArray, gepParam));
					llvm::Value *retVal = fb->interpolateCubic(fb->getIRBuilder(),
										 output0,
										 output1,
										 output2,
										 output3,
										 alpha
										 );
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

Curve::Curve() : Module(1), mCodeGenerator(new CurveGenerator()){
}

Curve::~Curve() {
	delete mCodeGenerator;
}

void Curve::addControlPoint(double input, double output) {
	addControlPoint(Curve::ControlPoint(input, output));
}

void Curve::addControlPoint(const Curve::ControlPoint &controlPoint) {
	for (std::vector<Curve::ControlPoint>::iterator i = mControlPoints.begin(); i != mControlPoints.end(); i++) {
		assert(i->inputValue != controlPoint.inputValue);
		if (i->inputValue > controlPoint.inputValue) {
			mControlPoints.insert(i, controlPoint);
			return;
		}
	}
	mControlPoints.push_back(controlPoint);
}

void Curve::clearControlPoints() {
	mControlPoints.clear();
}


}
