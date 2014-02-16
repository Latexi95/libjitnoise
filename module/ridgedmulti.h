#ifndef RIDGEDMULTI_H
#define RIDGEDMULTI_H
#include "module.h"
namespace jitnoise {

const double RIDGED_DEFAULT_FREQUENCY = 1.0;
const double RIDGED_DEFAULT_LACUNARITY = 2.0;
const int RIDGED_DEFAULT_OCTAVE_COUNT = 6;
const double RIDGED_DEFAULT_SPECTRAL_EXPONENT = 1.0;
const double RIDGED_DEFAULT_GAIN = 2.0;
const double RIDGED_DEFAULT_OFFSET = 1.0;
const int RIDGED_DEFAULT_SEED = 0;
const int RIDGED_MAX_OCTAVE = 30;

class RidgedMultiNoiseGenerator;
class RidgedMulti : public Module {
		friend class RidgedMultiNoiseGenerator;
	public:
		RidgedMulti();
		~RidgedMulti();

		CodeGenerator *codeGenerator() const;

		double frequency() const { return mFrequency; }
		void setFrequency(double f) { mFrequency = f; }
		double lacunarity() const { return mLacunarity; }
		void setLacunarity(double l) { mLacunarity = l; }
		int octaveCount() const { return mOctaveCount; }
		void setOctaveCount(int c) { mOctaveCount = c; }
		int seed() const { return mSeed; }
		void setSeed(int seed) { mSeed = seed; }
		double spectralExponent() const { return mSpectralExponentParameter; }
		void setSpectralExponent(double e) { mSpectralExponentParameter = e; }
		double offset() const { return mOffset; }
		void setOffset(double o) { mOffset = o; }
		double gain() const { return mGain; }
		void setGain(double gain) { mGain = gain; }
	private:
		void calculateSpectralWeights() const;
		double mFrequency;
		double mLacunarity;
		double mSpectralExponentParameter;
		double mOffset;
		double mGain;
		int mOctaveCount;
		int mSeed;
		mutable double mSpectralWeights[RIDGED_MAX_OCTAVE];
		CodeGenerator *mCodeGenerator;
};

}
#endif // RIDGEDMULTI_H
