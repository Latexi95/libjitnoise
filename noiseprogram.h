#ifndef LIBJITNOISE_NOISEPROGRAM_H
#define LIBJITNOISE_NOISEPROGRAM_H
#include "module/module.h"

namespace jitnoise {

class NoiseProgram {
	public:
		NoiseProgram(Module *baseModule);
		virtual ~NoiseProgram();
		virtual void run(double *inputVector, double *outputValues, unsigned int count) = 0;
		virtual void run(float *inputVector, float *outputValues, unsigned int count) = 0;
	private:
		Module *mBaseModule;
};

}

#endif // LIBJITNOISE_NOISEPROGRAM_H
