#include "power.h"
#include "../codegenerator.h"
namespace jitnoise {
	class PowerCodeGenerator : public CodeGenerator {
		llvm::Value *generate(Builder *builder, const Module *module, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *generationData) {
			const Power *power = static_cast<const Power*>(module);
			assert(power->getSourceModule(0) && power->getSourceModule(1));

			llvm::Value *a = power->getSourceModule(0)->generate(builder, x, y, z, generationData);
			llvm::Value *b = power->getSourceModule(1)->generate(builder, x, y, z, generationData);
			return builder->fpow(a, b);
		}
	};

	Power::Power() :
	Module(2),
	mCodeGenerator(new PowerCodeGenerator){
	}

	Power::~Power() {
		delete mCodeGenerator;
	}

}
