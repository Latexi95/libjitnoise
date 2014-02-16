#include "executioncontext.h"
#include "builder.h"
#include "module/module.h"
#include "codegenerator.h"
namespace jitnoise {

ExecutionContext::ExecutionContext()
{
}

ExecutionContext::~ExecutionContext() {

}

llvm::Value *ExecutionContext::constructGenerationType(Builder *builder, Module *baseModule) {
	std::vector<llvm::Type*> parts;
	int typeCount = 0;
	generationTypePart(builder, baseModule, typeCount, parts);
	if (parts.empty()) {
		return 0;
	}

	llvm::StructType *generationType = llvm::StructType::create(parts, "GenerationType");
	llvm::AllocaInst *var = builder->getIRBuilder()->CreateAlloca(generationType);
	builder->getIRBuilder()->CreateStore(llvm::Constant::getNullValue(generationType), var);
	return var;

}

void ExecutionContext::generationTypePart(Builder *builder, Module *module, int &id, std::vector<llvm::Type*> &parts) {
	llvm::Type *genTy = module->codeGenerator()->generateGenerationData(builder, module, id);
	if (genTy != 0) {
		parts.push_back(genTy);
		++id;
	}
	int sourceModuleCount = module->sourceModuleCount();
	for (int i = 0; i < sourceModuleCount; i++) {
		generationTypePart(builder, module->getSourceModule(i), id, parts);
	}
}


}
