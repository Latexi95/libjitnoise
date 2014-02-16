#ifndef EXECUTIONCONTEXT_H
#define EXECUTIONCONTEXT_H
#include "context.h"
#include <vector>
namespace llvm {
	class Value;
	class Type;
}
namespace jitnoise {
class Module;
class NoiseProgram;
class Builder;
class ExecutionContext {
	public:
		ExecutionContext();
		virtual ~ExecutionContext();
		virtual bool initialize(Context context) = 0;
		virtual NoiseProgram *compile(Module *baseModule) = 0;

	protected:
		llvm::Value *constructGenerationType(Builder *builder, Module *baseModule);
	private:

		void generationTypePart(Builder *builder, Module *module, int &id, std::vector<llvm::Type*> &parts);

};

}

#endif // EXECUTIONCONTEXT_H
