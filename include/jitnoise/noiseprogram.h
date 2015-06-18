#ifndef NOISEPROGRAM_H
#define NOISEPROGRAM_H
#include "context.h"
namespace noise {

class NoiseProgram
{
    friend class ExecutionContext;
    NoiseProgram(const Context &context, void *unpackedFunction, void *packedFunction);
public:
    NoiseProgram(const NoiseProgram &program) = default;
    ~NoiseProgram() = default;
    NoiseProgram &operator=(const NoiseProgram &program) = default;

    void execute(const float *x, const float *y, const float *z, float *result, unsigned points);
    void execute(const float *packedPoints, float *result, unsigned points);

    void execute(const double *x, const double *y, const double *z, double *result, unsigned points);
    void execute(const double *packedPoints, double *result, unsigned points);


private:
    Context m_Context;
    void *m_UnpackedFunction;
    void *m_PackedFunction;
};

}
#endif // NOISEPROGRAM_H
