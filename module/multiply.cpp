#include "multiply.h"
#include "../codegenerator.h"
namespace jitnoise {
	class MultiplyCodeGenerator : public CodeGenerator {
		llvm::Value *generate(Builder *builder, const Module *module, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *generationData) {
			const Multiply *multiply = static_cast<const Multiply*>(module);
			assert(multiply->getSourceModule(0) && multiply->getSourceModule(1));

			llvm::Value *a = multiply->getSourceModule(0)->generate(builder, x, y, z, generationData);
			llvm::Value *b = multiply->getSourceModule(1)->generate(builder, x, y, z, generationData);
			return builder->fmul(a, b);
		}
	};

	Multiply::Multiply() :
	Module(2),
	mCodeGenerator(new MultiplyCodeGenerator){
	}

	Multiply::~Multiply() {
		delete mCodeGenerator;
	}

}
