#ifndef CURVE_H
#define CURVE_H
#include "module.h"
#include <vector>
namespace jitnoise {
	class Curve : public Module {
		public:
			struct ControlPoint {
				ControlPoint() : inputValue(0), outputValue(0) { }
				ControlPoint(double input, double output) : inputValue(input), outputValue(output) { }
				double inputValue;
				double outputValue;
			};

			Curve();
			~Curve();
			CodeGenerator *codeGenerator() const { return mCodeGenerator; }
			void addControlPoint(double input, double output);
			void addControlPoint(const ControlPoint & controlPoint);
			void clearControlPoints();
			const std::vector<ControlPoint> &controlPoints() const { return mControlPoints; }
		private:
			CodeGenerator *mCodeGenerator;
			std::vector<ControlPoint> mControlPoints;
	};
}
#endif // CURVE_H
