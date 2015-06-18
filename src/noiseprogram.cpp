#include "jitnoise/noiseprogram.h"
#include <cassert>

typedef void (*UnpackedFloatFunction) (const float*, const float*, const float*, float*, unsigned);
typedef void (*PackedFloatFunction) (const float*, float*, unsigned);
typedef void (*UnpackedDoubleFunction) (const double*, const double*, const double*, double*, unsigned);
typedef void (*PackedDoubleFunction) (const double*, double*, unsigned);
using namespace noise;
NoiseProgram::NoiseProgram(const Context &context, void *unpackedFunction, void *packedFunction) :
    m_Context(context),
    m_UnpackedFunction(unpackedFunction),
    m_PackedFunction(packedFunction)
{

}

void NoiseProgram::execute(const float *x, const float *y, const float *z, float *result, unsigned points)
{
    assert(m_Context.floatingPointMode == FLOATING_POINT_MODE_SINGLE);
    if (m_Context.vectorize) {
        assert(points % 4 == 0);
    }
    UnpackedFloatFunction func = (UnpackedFloatFunction)m_UnpackedFunction;
    func(x, y, z, result, points);
}

void NoiseProgram::execute(const float *packedPoints, float *result, unsigned points)
{
    assert(m_Context.floatingPointMode == FLOATING_POINT_MODE_SINGLE);
    if (m_Context.vectorize) {
        assert(points % 4 == 0);
    }

    PackedFloatFunction func = (PackedFloatFunction)m_PackedFunction;
    func(packedPoints, result, points);
}

void NoiseProgram::execute(const double *x, const double *y, const double *z, double *result, unsigned points)
{
    assert(m_Context.floatingPointMode == FLOATING_POINT_MODE_DOUBLE);
    if (m_Context.vectorize) {
        assert(points % 4 == 0);
    }
    UnpackedDoubleFunction func = (UnpackedDoubleFunction)m_UnpackedFunction;
    func(x, y, z, result, points);
}

void NoiseProgram::execute(const double *packedPoints, double *result, unsigned points)
{
    assert(m_Context.floatingPointMode == FLOATING_POINT_MODE_DOUBLE);
    if (m_Context.vectorize) {
        assert(points % 4 == 0);
    }
    PackedDoubleFunction func = (PackedDoubleFunction)m_PackedFunction;
    func(packedPoints, result, points);
}

