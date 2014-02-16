#ifndef TERRACE_H
#define TERRACE_H
#include "module.h"
#include <vector>
namespace jitnoise {
	class Terrace : public Module {
		public:

			Terrace();
			~Terrace();
			CodeGenerator *codeGenerator() const { return mCodeGenerator; }
			void addControlPoint(double val);
			void clearControlPoints();
			const std::vector<double> &controlPoints() const { return mControlPoints; }
			void invertTerraces(bool invert) { mInvertTerraces = invert; }
			bool isTerracedInverted() const { return mInvertTerraces; }

			void makeControlPoints(int controlPointCount);
		private:
			CodeGenerator *mCodeGenerator;
			std::vector<double> mControlPoints;
			bool mInvertTerraces;
	};
}
#endif // TERRACE_H
