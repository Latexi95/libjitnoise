
#include "jitnoise/module/rotatepoint.h"
#include "codegenerator.h"
#include <cmath>
#include "jitnoise/mathconsts.h"
using namespace noise::module;
using namespace noise;

class RotatePointCodeGenerator : public CodeGenerator
{
public:
    RotatePointCodeGenerator(RotatePoint *a) : m_Module(a) {}
    llvm::Value *generate(Builder &b, llvm::Value *x, llvm::Value *y, llvm::Value *z, llvm::Value *codeGenContext) {
        double xCos, yCos, zCos, xSin, ySin, zSin;
        xCos = cos (m_Module->GetXAngle() * DEG_TO_RAD);
        yCos = cos (m_Module->GetYAngle() * DEG_TO_RAD);
        zCos = cos (m_Module->GetZAngle() * DEG_TO_RAD);
        xSin = sin (m_Module->GetXAngle() * DEG_TO_RAD);
        ySin = sin (m_Module->GetYAngle() * DEG_TO_RAD);
        zSin = sin (m_Module->GetZAngle() * DEG_TO_RAD);

        double m_x1Matrix = ySin * xSin * zSin + yCos * zCos;
        double m_y1Matrix = xCos * zSin;
        double m_z1Matrix = ySin * zCos - yCos * xSin * zSin;
        double m_x2Matrix = ySin * xSin * zCos - yCos * zSin;
        double m_y2Matrix = xCos * zCos;
        double m_z2Matrix = -yCos * xSin * zCos - ySin * zSin;
        double m_x3Matrix = -ySin * xCos;
        double m_y3Matrix = xSin;
        double m_z3Matrix = yCos * xCos;


        llvm::Value *nx = b.CreateFAdd(
                    b.CreateFMul(b.getFPValue(m_x1Matrix), x),
                    b.CreateFAdd(
                        b.CreateFMul(b.getFPValue(m_y1Matrix), y),
                        b.CreateFMul(b.getFPValue(m_z1Matrix), z)
                        )
                    );
        llvm::Value *ny = b.CreateFAdd(
                    b.CreateFMul(b.getFPValue(m_x2Matrix), x),
                    b.CreateFAdd(
                        b.CreateFMul(b.getFPValue(m_y2Matrix), y),
                        b.CreateFMul(b.getFPValue(m_z2Matrix), z)
                        )
                    );
        llvm::Value *nz = b.CreateFAdd(
                    b.CreateFMul(b.getFPValue(m_x3Matrix), x),
                    b.CreateFAdd(
                        b.CreateFMul(b.getFPValue(m_y3Matrix), y),
                        b.CreateFMul(b.getFPValue(m_z3Matrix), z)
                        )
                    );


        return m_Module->GetSourceModule(0).GetCodeGenerator()->generate(b, nx, ny, nz, codeGenContext);
    }
private:
    RotatePoint *m_Module;
};


RotatePoint::RotatePoint ():
  Module (GetSourceModuleCount (), new RotatePointCodeGenerator(this))
{
  SetAngles (DEFAULT_ROTATE_X, DEFAULT_ROTATE_Y, DEFAULT_ROTATE_Z);
}


void RotatePoint::SetAngles (double xAngle, double yAngle,
  double zAngle)
{
  m_xAngle = xAngle;
  m_yAngle = yAngle;
  m_zAngle = zAngle;
}
