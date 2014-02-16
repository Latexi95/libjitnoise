#include "max.h"
#include "../codegenerator.h"
namespace jitnoise {
	class MaxCodeGenerator : public CodeGenerator {
		llvm::Value *generate(Builder *builder, const Module *module, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *generationData) {
			const Max *max = static_cast<const Max*>(module);
			assert(max->getSourceModule(0) && max->getSourceModule(1));

			llvm::Value *a = max->getSourceModule(0)->generate(builder, x, y, z, generationData);
			llvm::Value *b = max->getSourceModule(1)->generate(builder, x, y, z, generationData);
			return builder->maxFP(a, b);
		}
	};

	Max::Max() :
	Module(2),
	mCodeGenerator(new MaxCodeGenerator){
	}

	Max::~Max() {
		delete mCodeGenerator;
	}

}
