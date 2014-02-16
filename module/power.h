#ifndef POWER_H
#define POWER_H
#include "module.h"
namespace jitnoise {
	class Power : public Module {
		public:
			Power();
			~Power();
			CodeGenerator *codeGenerator() const { return mCodeGenerator; }
		private:
			CodeGenerator *mCodeGenerator;
	};
}
#endif // POWER_H
