#ifndef PERLIN_H
#define PERLIN_H
#include "module.h"
namespace jitnoise {

const double PERLIN_DEFAULT_FREQUENCY = 1.0;
const double PERLIN_DEFAULT_LACUNARITY = 2.0;
const int PERLIN_DEFAULT_OCTAVE_COUNT = 6;
const double PERLIN_DEFAULT_PERSISTENCE = 0.5;
const int PERLIN_DEFAULT_SEED = 0;
const int PERLIN_MAX_OCTAVE = 30;

class Perlin : public Module {
	public:
		Perlin();
		~Perlin();

		CodeGenerator *codeGenerator() const;

		double frequency() const { return mFrequency; }
		void setFrequency(double f) { mFrequency = f; }
		double lacunarity() const { return mLacunarity; }
		void setLacunarity(double l) { mLacunarity = l; }
		int octaveCount() const { return mOctaveCount; }
		void setOctaveCount(int c) { mOctaveCount = c; }
		double persistence() const { return mPersistence; }
		void setPersistence(double p) { mPersistence = p; }
		int seed() const { return mSeed; }
		void setSeed(int seed) { mSeed = seed; }
	private:
		double mFrequency;
		double mLacunarity;
		int mOctaveCount;
		double mPersistence;
		int mSeed;
		CodeGenerator *mCodeGenerator;
};

}
#endif // PERLIN_H
