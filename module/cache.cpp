#include "cache.h"
#include "../builder.h"
#include "../codegenerator.h"
namespace jitnoise {

class CacheGenerator : public CodeGenerator {
	public:
		CacheGenerator() : mFunction(0), mCacheDataType(0), mCacheDataIndex(0) {}

		llvm::Value *generate(Builder *builder, const Module *module, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *generationData) {
			const Cache *cacheModule = static_cast<const Cache*>(module);

			if (!mFunction) generateFunction(builder, cacheModule);
			return builder->createModuleFunctionCall(mFunction, x, y, z, generationData);
		}

		void generateFunction(Builder *tempBuilder, const Cache *cache) {
			llvm::Value *x, *y, *z, *generationData;
			llvm::BasicBlock *bb;
			Builder *builder = tempBuilder->createModuleFunction("cacheModule", &x, &y, &z, &generationData, &bb);
			mFunction = builder->function();
			llvm::BasicBlock *trueBB = builder->createBasicBlock("trueBB");
			llvm::BasicBlock *falseBB = builder->createBasicBlock("falseBB");

			std::vector<llvm::Value*> gepParams;
			gepParams.push_back(builder->getSingleIntValue(0));
			gepParams.push_back(builder->getSingleIntValue(mCacheDataIndex));
			llvm::Value *cacheData = builder->getIRBuilder()->CreateGEP(generationData, gepParams);

			gepParams[1] = builder->getSingleIntValue(0);
			llvm::Value *isDataCached = builder->load(builder->getIRBuilder()->CreateGEP(cacheData, gepParams));

			gepParams[1] = builder->getSingleIntValue(1);
			llvm::Value *cachedDataX = builder->load(builder->getIRBuilder()->CreateGEP(cacheData, gepParams));
			gepParams[1] = builder->getSingleIntValue(2);
			llvm::Value *cachedDataY = builder->load(builder->getIRBuilder()->CreateGEP(cacheData, gepParams));
			gepParams[1] = builder->getSingleIntValue(3);
			llvm::Value *cachedDataZ = builder->load(builder->getIRBuilder()->CreateGEP(cacheData, gepParams));

			llvm::Value *cond = builder->getIRBuilder()->CreateAnd(isDataCached,
																   builder->i1x4toi1(builder->getIRBuilder()->CreateAdd(builder->getIRBuilder()->CreateFCmpOEQ(cachedDataX, x),
																   builder->getIRBuilder()->CreateAdd(builder->getIRBuilder()->CreateFCmpOEQ(cachedDataY, y),
																   builder->getIRBuilder()->CreateFCmpOEQ(cachedDataZ, z)))));
			builder->getIRBuilder()->CreateCondBr(cond, trueBB, falseBB);

			builder->getIRBuilder()->SetInsertPoint(trueBB);
			gepParams[1] = builder->getSingleIntValue(4);
			llvm::Value *cachedDataValue = builder->load(builder->getIRBuilder()->CreateGEP(cacheData, gepParams));
			builder->getIRBuilder()->CreateRet(cachedDataValue);

			builder->getIRBuilder()->SetInsertPoint(falseBB);
			llvm::Value *ret = cache->getSourceModule(0)->generate(builder, x, y, z, generationData);

			gepParams[1] = builder->getSingleIntValue(0);
			builder->store(builder->getIRBuilder()->getInt1(true), builder->getIRBuilder()->CreateGEP(cacheData, gepParams));

			gepParams[1] = builder->getSingleIntValue(1);
			builder->store(x, builder->getIRBuilder()->CreateGEP(cacheData, gepParams));
			gepParams[1] = builder->getSingleIntValue(2);
			builder->store(y, builder->getIRBuilder()->CreateGEP(cacheData, gepParams));
			gepParams[1] = builder->getSingleIntValue(3);
			builder->store(z, builder->getIRBuilder()->CreateGEP(cacheData, gepParams));
			gepParams[1] = builder->getSingleIntValue(4);
			builder->store(ret, builder->getIRBuilder()->CreateGEP(cacheData, gepParams));
			builder->getIRBuilder()->CreateRet(ret);

			delete builder;
		}

		llvm::Type *generateGenerationData(Builder *builder, const Module *module, int id) {
			if (!mCacheDataType) {
				mCacheDataIndex = id;
				std::vector<llvm::Type*> parts;
				parts.push_back(llvm::Type::getInt1Ty(builder->module()->getContext()));
				parts.push_back(builder->getFPType());
				parts.push_back(builder->getFPType());
				parts.push_back(builder->getFPType());
				parts.push_back(builder->getFPType());
				mCacheDataType = llvm::StructType::create(parts, "CacheData");

				return mCacheDataType;
			}
			return 0;
		}

	private:
		llvm::Type *mCacheDataType;
		int mCacheDataIndex;
		llvm::Function *mFunction;
};

Cache::Cache() : Module(1), mCodeGenerator(new CacheGenerator()){
}

Cache::~Cache() {
	delete mCodeGenerator;
}

}
