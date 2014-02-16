#include "add.h"
#include "../codegenerator.h"
namespace jitnoise {
	class AddCodeGenerator : public CodeGenerator {
		llvm::Value *generate(Builder *builder, const Module *module, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *generationData) {
			const Add *add = static_cast<const Add*>(module);
			assert(add->getSourceModule(0) && add->getSourceModule(1));

			llvm::Value *a = add->getSourceModule(0)->generate(builder, x, y, z, generationData);
			llvm::Value *b = add->getSourceModule(1)->generate(builder, x, y, z, generationData);
			return builder->fadd(a, b);
		}
	};

	Add::Add() :
	Module(2),
	mCodeGenerator(new AddCodeGenerator){
	}

	Add::~Add() {
		delete mCodeGenerator;
	}

}
