#ifndef MULTIPLY_H
#define MULTIPLY_H
#include "module.h"
namespace jitnoise {
	class Multiply : public Module {
		public:
			Multiply();
			~Multiply();
			CodeGenerator *codeGenerator() const { return mCodeGenerator; }
		private:
			CodeGenerator *mCodeGenerator;
	};
}
#endif // MULTIPLY_H
