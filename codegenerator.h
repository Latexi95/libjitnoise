#ifndef LIBJITNOISE_NOISEGENERATOR_H
#define LIBJITNOISE_NOISEGENERATOR_H
#include "builder.h"
#include "module/module.h"
#include "context.h"
namespace jitnoise {

class CodeGenerator {
	public:
		CodeGenerator();
		virtual ~CodeGenerator();

		virtual llvm::Value *generate(Builder *builder, const Module *module, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *generationData) = 0;
		virtual llvm::Type *generateGenerationData(Builder *builder, const Module *module, int id) { return 0; }
};

}

#endif // LIBJITNOISE_NOISEGENERATOR_H
