#ifndef ADD_H
#define ADD_H
#include "module.h"
namespace jitnoise {
	class Add : public Module {
		public:
			Add();
			~Add();
			CodeGenerator *codeGenerator() const { return mCodeGenerator; }
		private:
			CodeGenerator *mCodeGenerator;
	};
}
#endif // ADD_H
