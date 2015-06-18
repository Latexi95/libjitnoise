#ifndef CONTEXT_H_
#define CONTEXT_H_

namespace noise {

/// @addtogroup jitnoise
/// @{

/// Enumerates the noise quality.
enum FloatingPointMode
{
    ///Use float
    FLOATING_POINT_MODE_SINGLE = 0,
    ///Use double
    FLOATING_POINT_MODE_DOUBLE = 1
};

enum NoiseQuality
{

    /// Generates coherent noise quickly.  When a coherent-noise function with
    /// this quality setting is used to generate a bump-map image, there are
    /// noticeable "creasing" artifacts in the resulting image.  This is
    /// because the derivative of that function is discontinuous at integer
    /// boundaries.
    QUALITY_FAST = 0,

    /// Generates standard-quality coherent noise.  When a coherent-noise
    /// function with this quality setting is used to generate a bump-map
    /// image, there are some minor "creasing" artifacts in the resulting
    /// image.  This is because the second derivative of that function is
    /// discontinuous at integer boundaries.
    QUALITY_STD = 1,

    /// Generates the best-quality coherent noise.  When a coherent-noise
    /// function with this quality setting is used to generate a bump-map
    /// image, there are no "creasing" artifacts in the resulting image.  This
    /// is because the first and second derivatives of that function are
    /// continuous at integer boundaries.
    QUALITY_BEST = 2

};
struct Context
{
    Context() : floatingPointMode(FLOATING_POINT_MODE_SINGLE), noiseQuality(QUALITY_STD), vectorize(true) {}

    FloatingPointMode floatingPointMode;
    NoiseQuality noiseQuality;

    bool vectorize;
};
}

#endif // CONTEXT_H_

