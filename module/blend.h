#ifndef BLEND_H
#define BLEND_H
#include "module.h"
namespace jitnoise {
	class Blend : public Module {
		public:
			Blend();
			~Blend();
			CodeGenerator *codeGenerator() const { return mCodeGenerator; }
		private:
			CodeGenerator *mCodeGenerator;
	};
}
#endif // BLEND_H
