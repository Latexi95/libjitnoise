#include "select.h"
#include "../codegenerator.h"
namespace jitnoise {
	class SelectCodeGenerator : public CodeGenerator {
		public:
			SelectCodeGenerator() : mFunction(0) { }

			llvm::Value *generate(Builder *builder, const Module *module, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *generationData) {
				const Select *select = static_cast<const Select*>(module);
				assert(select->getSourceModule(0) && select->getSourceModule(1) && select->getSourceModule(2));
				if (!mFunction) generateFunction(builder, select);

				return builder->createModuleFunctionCall(mFunction, x, y, z, generationData);
			}

			void generateFunction(Builder *tempBuilder, const Select *select) {

				llvm::Value *x;
				llvm::Value *y;
				llvm::Value *z;
				llvm::BasicBlock *bb;
				llvm::Value *generationData;
				Builder *builder = tempBuilder->createModuleFunction("select", &x, &y, &z, &generationData, &bb);
				mFunction = builder->function();

				llvm::Value *controlValue = select->getSourceModule(2)->generate(builder, x, y, z, generationData);

				if (select->edgeFalloff() > 0) {
					if (builder->context().mVectorize) {
						llvm::Value *retVar = builder->getIRBuilder()->CreateAlloca(builder->getFPType());
						llvm::Value *a = select->getSourceModule(0)->generate(builder, x, y, z, generationData);
						llvm::Value *b = select->getSourceModule(0)->generate(builder, x, y, z, generationData);
						llvm::BasicBlock *bbs[9 * 4];
						for (int i = 0; i < 9 * 4; i++) {
							bbs[i] = builder->createBasicBlock("bb");
						}
						for (int i = 0; i < 4; i++) {
							llvm::BasicBlock *cond1TrueBB = bbs[i * 9];
							llvm::BasicBlock *cond2BB = bbs[i * 9 + 1];
							llvm::BasicBlock *cond2TrueBB = bbs[i * 9 + 2];
							llvm::BasicBlock *cond3BB = bbs[i * 9 + 3];
							llvm::BasicBlock *cond3TrueBB = bbs[i * 9 + 4];
							llvm::BasicBlock *cond4BB = bbs[i * 9 + 5];
							llvm::BasicBlock *cond4TrueBB = bbs[i * 9 + 6];
							llvm::BasicBlock *elseBB = bbs[i * 9 + 7];
							llvm::Value *control = builder->extractElement(controlValue, i);
							llvm::Value *cond = builder->getIRBuilder()->CreateFCmpOLT(control, builder->getSingleFPValue(select->lowerBound() - select->edgeFalloff()));
							builder->conditionalBranch(cond, cond1TrueBB, cond2BB);

							builder->setInsertPoint(cond1TrueBB);
							llvm::Value *ret = builder->load(retVar);
							builder->store(builder->insertElement(ret, builder->extractElement(a, i), i), retVar);
							builder->branch(bbs[i * 9 + 8]);

							builder->setInsertPoint(cond2BB);
							cond = builder->getIRBuilder()->CreateFCmpOLT(control, builder->getSingleFPValue(select->lowerBound() + select->edgeFalloff()));
							builder->conditionalBranch(cond, cond2TrueBB, cond3BB);

							builder->setInsertPoint(cond2TrueBB);
							double lowerCurve = select->lowerBound() - select->edgeFalloff();
							double upperCurve = select->lowerBound() + select->edgeFalloff();

							llvm::Value *alpha = builder->sCurve3(builder->getIRBuilder(), builder->fdiv(builder->fsub(control, builder->getSingleFPValue(lowerCurve)), builder->getSingleFPValue(upperCurve - lowerCurve)));
							llvm::Value *v = builder->interpolateLinear(
																   builder->getIRBuilder(),
																   builder->extractElement(a, i),
																   builder->extractElement(b, i),
																   alpha);
							ret = builder->load(retVar);
							builder->store(builder->insertElement(ret, v, i), retVar);
							builder->branch(bbs[i * 9 + 8]);

							builder->setInsertPoint(cond3BB);
							cond = builder->getIRBuilder()->CreateFCmpOLT(control, builder->getSingleFPValue(select->upperBound() - select->edgeFalloff()));
							builder->conditionalBranch(cond, cond3TrueBB, cond4BB);

							builder->setInsertPoint(cond3TrueBB);
							ret = builder->load(retVar);
							builder->store(builder->insertElement(ret, builder->extractElement(b, i), i), retVar);
							builder->branch(bbs[i * 9 + 8]);

							builder->setInsertPoint(cond4BB);
							cond = builder->getIRBuilder()->CreateFCmpOLT(control, builder->getSingleFPValue(select->upperBound() + select->edgeFalloff()));
							builder->conditionalBranch(cond, cond4TrueBB, elseBB);

							builder->setInsertPoint(cond4TrueBB);
							lowerCurve = select->upperBound() - select->edgeFalloff();
							upperCurve = select->upperBound() + select->edgeFalloff();

							alpha = builder->sCurve3(builder->getIRBuilder(), builder->fdiv(builder->fsub(control, builder->getSingleFPValue(lowerCurve)), builder->getSingleFPValue(upperCurve - lowerCurve)));
							v = builder->interpolateLinear(
																   builder->getIRBuilder(),
																   builder->extractElement(b, i),
																   builder->extractElement(a, i),
																   alpha);
							ret = builder->load(retVar);
							builder->store(builder->insertElement(ret, v, i), retVar);
							builder->branch(bbs[i * 9 + 8]);

							builder->setInsertPoint(elseBB);
							ret = builder->load(retVar);
							builder->store(builder->insertElement(ret, builder->extractElement(a, i), i), retVar);
							builder->branch(bbs[i * 9 + 8]);

							builder->setInsertPoint(bbs[i * 9 + 8]);
						}
						builder->getIRBuilder()->CreateRet(builder->load(retVar));
					}
					else {
						llvm::Value *cond = builder->getIRBuilder()->CreateFCmpOLT(controlValue, builder->getFPValue(select->lowerBound() - select->edgeFalloff()));
						llvm::BasicBlock *cond1TrueBB = builder->createBasicBlock("cond1TrueBB");
						llvm::BasicBlock *cond2BB = builder->createBasicBlock("cond2BB");
						llvm::BasicBlock *cond2TrueBB = builder->createBasicBlock("cond2TrueBB");
						llvm::BasicBlock *cond3BB = builder->createBasicBlock("cond3BB");
						llvm::BasicBlock *cond3TrueBB = builder->createBasicBlock("cond3TrueBB");
						llvm::BasicBlock *cond4BB = builder->createBasicBlock("cond4BB");
						llvm::BasicBlock *cond4TrueBB = builder->createBasicBlock("cond4TrueBB");
						llvm::BasicBlock *elseBB = builder->createBasicBlock("elseBB");
						builder->conditionalBranch(cond, cond1TrueBB, cond2BB);

						builder->setInsertPoint(cond1TrueBB);
						builder->getIRBuilder()->CreateRet(select->getSourceModule(0)->generate(builder, x, y, z, generationData));

						builder->setInsertPoint(cond2BB);
						cond = builder->getIRBuilder()->CreateFCmpOLT(controlValue, builder->getFPValue(select->lowerBound() + select->edgeFalloff()));
						builder->conditionalBranch(cond, cond2TrueBB, cond3BB);

						builder->setInsertPoint(cond2TrueBB);
						double lowerCurve = select->lowerBound() - select->edgeFalloff();
						double upperCurve = select->lowerBound() + select->edgeFalloff();

						llvm::Value *alpha = builder->sCurve3(builder->getIRBuilder(), builder->fdiv(builder->fsub(controlValue, builder->getSingleFPValue(lowerCurve)), builder->getSingleFPValue(upperCurve - lowerCurve)));
						builder->getIRBuilder()->CreateRet(builder->interpolateLinear(
															   builder->getIRBuilder(),
															   select->getSourceModule(0)->generate(builder, x, y, z, generationData),
															   select->getSourceModule(1)->generate(builder, x, y, z, generationData),
															   alpha));

						builder->setInsertPoint(cond3BB);
						cond = builder->getIRBuilder()->CreateFCmpOLT(controlValue, builder->getFPValue(select->upperBound() - select->edgeFalloff()));
						builder->conditionalBranch(cond, cond3TrueBB, cond4BB);

						builder->setInsertPoint(cond3TrueBB);
						builder->getIRBuilder()->CreateRet(select->getSourceModule(1)->generate(builder, x, y, z, generationData));

						builder->setInsertPoint(cond4BB);
						cond = builder->getIRBuilder()->CreateFCmpOLT(controlValue, builder->getFPValue(select->upperBound() + select->edgeFalloff()));
						builder->conditionalBranch(cond, cond4TrueBB, elseBB);

						builder->setInsertPoint(cond4TrueBB);
						lowerCurve = select->upperBound() - select->edgeFalloff();
						upperCurve = select->upperBound() + select->edgeFalloff();

						alpha = builder->sCurve3(builder->getIRBuilder(), builder->fdiv(builder->fsub(controlValue, builder->getSingleFPValue(lowerCurve)), builder->getSingleFPValue(upperCurve - lowerCurve)));
						builder->getIRBuilder()->CreateRet(builder->interpolateLinear(
															   builder->getIRBuilder(),
															   select->getSourceModule(1)->generate(builder, x, y, z, generationData),
															   select->getSourceModule(0)->generate(builder, x, y, z, generationData),
															   alpha));

						builder->setInsertPoint(elseBB);
						builder->getIRBuilder()->CreateRet(select->getSourceModule(0)->generate(builder, x, y, z, generationData));

					}
				}
				else {
					if (builder->context().mVectorize) {
						llvm::Value *a = select->getSourceModule(0)->generate(builder, x, y, z, generationData);
						llvm::Value *b = select->getSourceModule(1)->generate(builder, x, y, z, generationData);
						llvm::Value *ret = llvm::UndefValue::get(builder->getFPType());
						for (int i = 0; i < 4; i++) {
							llvm::Value *control = builder->getIRBuilder()->CreateExtractElement(controlValue, builder->getSingleIntValue(i));
							llvm::Value *cond = builder->getIRBuilder()->CreateOr(builder->getIRBuilder()->CreateFCmpOLT(control, builder->getFPValue(select->lowerBound())), builder->getIRBuilder()->CreateFCmpOGT(control, builder->getFPValue(select->upperBound())));
							ret = builder->getIRBuilder()->CreateInsertElement(ret, builder->getIRBuilder()->CreateSelect(cond,
																														  builder->getIRBuilder()->CreateExtractElement(a, builder->getSingleIntValue(i)),
																														  builder->getIRBuilder()->CreateExtractElement(b, builder->getSingleIntValue(i))), builder->getSingleIntValue(i));
						}
						builder->getIRBuilder()->CreateRet(ret);

					}
					else {
						llvm::Value *cond = builder->getIRBuilder()->CreateOr(builder->getIRBuilder()->CreateFCmpOLT(controlValue, builder->getFPValue(select->lowerBound())), builder->getIRBuilder()->CreateFCmpOGT(controlValue, builder->getFPValue(select->upperBound())));
						llvm::BasicBlock *condTrueBB = builder->createBasicBlock("condTrueBB");
						llvm::BasicBlock *condFalseBB = builder->createBasicBlock("condFalseBB");
						builder->conditionalBranch(cond, condTrueBB, condFalseBB);

						builder->setInsertPoint(condTrueBB);
						builder->getIRBuilder()->CreateRet(select->getSourceModule(0)->generate(builder, x, y, z, generationData));

						builder->setInsertPoint(condFalseBB);
						builder->getIRBuilder()->CreateRet(select->getSourceModule(1)->generate(builder, x, y, z, generationData));
					}
				}
			}

		private:
			llvm::Function *mFunction;

	};

	Select::Select() :
		Module(3),
		mCodeGenerator(new SelectCodeGenerator),
		mEdgeFalloff(DEFAULT_SELECT_EDGE_FALLOFF),
		mLowerBound(DEFAULT_SELECT_LOWER_BOUND),
		mUpperBound(DEFAULT_SELECT_UPPER_BOUND)
	{
	}

	Select::~Select() {
		delete mCodeGenerator;
	}

}
