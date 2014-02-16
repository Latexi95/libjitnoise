#ifndef ABS_H
#define ABS_H
#include "module.h"
namespace jitnoise {
	class Abs : public Module {
		public:
			Abs();
			~Abs();
			CodeGenerator *codeGenerator() const { return mCodeGenerator; }
		private:
			CodeGenerator *mCodeGenerator;
	};
}
#endif // ABS_H
