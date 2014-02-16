#include "min.h"
#include "../codegenerator.h"
namespace jitnoise {
	class MinCodeGenerator : public CodeGenerator {
		llvm::Value *generate(Builder *builder, const Module *module, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *generationData) {
			const Min *min = static_cast<const Min*>(module);
			assert(min->getSourceModule(0) && min->getSourceModule(1));

			llvm::Value *a = min->getSourceModule(0)->generate(builder, x, y, z, generationData);
			llvm::Value *b = min->getSourceModule(1)->generate(builder, x, y, z, generationData);
			return builder->minFP(a, b);
		}
	};

	Min::Min() :
	Module(2),
	mCodeGenerator(new MinCodeGenerator){
	}

	Min::~Min() {
		delete mCodeGenerator;
	}

}
