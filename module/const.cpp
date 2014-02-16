#include "const.h"
#include "../codegenerator.h"
namespace jitnoise {
	class ConstCodeGenerator : public CodeGenerator {
		llvm::Value *generate(Builder *builder, const Module *module, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *generationData) {
			const Const *c = static_cast<const Const*>(module);
			return builder->getFPValue(c->value());
		}
	};

	Const::Const() :
	Module(0),
	mCodeGenerator(new ConstCodeGenerator),
	mConstValue(CONST_DEFAULT_VALUE){
	}

	Const::~Const() {
		delete mCodeGenerator;
	}

}
