#ifndef LIBJITNOISE_MODULE_H
#define LIBJITNOISE_MODULE_H
namespace llvm {
	class Value;
}
namespace jitnoise {
class CodeGenerator;
class Builder;
class Module {
	public:
		Module(int sourceModuleCount);
		virtual ~Module();

		virtual CodeGenerator *codeGenerator() const = 0;
		int sourceModuleCount() const { return mSourceModuleCount; }
		Module *getSourceModule(int index) const;
		void setSourceModule(int index, Module *source);

		llvm::Value *generate(Builder *builder, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *generateData) const;

	protected:
		Module **mSourceModules;
		const int mSourceModuleCount;
};

}

#endif // LIBJITNOISE_MODULE_H
