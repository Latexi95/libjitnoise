#ifndef MIN_H
#define MIN_H
#include "module.h"
namespace jitnoise {
	class Min : public Module {
		public:
			Min();
			~Min();
			CodeGenerator *codeGenerator() const { return mCodeGenerator; }
		private:
			CodeGenerator *mCodeGenerator;
	};
}

#endif // MIN_H
