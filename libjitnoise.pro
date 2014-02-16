#-------------------------------------------------
#
# Project created by QtCreator 2014-02-09T14:25:06
#
#-------------------------------------------------

QT       -= core gui

TARGET = libjitnoise
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    module/module.cpp \
    codegenerator.cpp \
    noiseprogram.cpp \
    builder.cpp \
    module/perlin.cpp \
    context.cpp \
    executioncontext.cpp \
    jitexecutioncontext.cpp \
    module/abs.cpp \
    module/curve.cpp \
    module/add.cpp \
    module/const.cpp \
    module/clamp.cpp \
    module/min.cpp \
    module/max.cpp \
    module/scalebias.cpp \
    module/cache.cpp \
    module/turbulence.cpp \
    module/multiply.cpp \
    module/power.cpp \
    module/ridgedmulti.cpp \
    module/voronoi.cpp \
    module/exponent.cpp \
    module/terrace.cpp \
    module/select.cpp \
    module/billow.cpp \
    module/blend.cpp

HEADERS += \
    module/module.h \
    codegenerator.h \
    noiseprogram.h \
    builder.h \
    module/perlin.h \
    context.h \
    executioncontext.h \
    jitexecutioncontext.h \
    module/abs.h \
    module/curve.h \
    module/add.h \
    module/const.h \
    module/clamp.h \
    module/min.h \
    module/max.h \
    module/scalebias.h \
    module/cache.h \
    module/turbulence.h \
    module/multiply.h \
    module/power.h \
    module/ridgedmulti.h \
    module/voronoi.h \
    mathconsts.h \
    module/exponent.h \
    module/terrace.h \
    module/select.h \
    module/billow.h \
    module/blend.h \
    module/modules.h \
    llvm.h

CONFIG(debug, debug|release) {
	#LIBS += -LD:/Programming/Libs/llvm-3.2.msvc/lib/Debug
	LIBS += -LD:/Programming/Libs/llvm-3.4.msvc10/install_debug/lib
} else {
	#LIBS += -LD:/Programming/Libs/llvm-3.2.msvc/lib/Release
	LIBS += -LD:/Programming/Libs/llvm-3.4.msvc10/install_release/lib
}
#LIBS += -lLLVMVectorize -lLLVMipo -lLLVMLinker -lLLVMBitReader -lLLVMBitWriter -lLLVMInstrumentation -lLLVMX86CodeGen -lLLVMRuntimeDyld -lLLVMExecutionEngine -lLLVMArchive -lLLVMAsmParser -lLLVMAsmPrinter -lLLVMSelectionDAG -lLLVMX86Desc -lLLVMMCParser -lLLVMCodeGen -lLLVMX86AsmPrinter -lLLVMX86Info -lLLVMScalarOpts -lLLVMX86Utils -lLLVMInstCombine -lLLVMTransformUtils -lLLVMipa -lLLVMJIT -lLLVMMCJIT -lLLVMAnalysis -lLLVMTarget -lLLVMCore -lLLVMMC -lLLVMObject -lLLVMSupport
LIBS += -lLLVMVectorize -lLLVMipo -lLLVMLinker -lLLVMBitReader -lLLVMBitWriter -lLLVMIRReader -lLLVMInstrumentation -lLLVMX86CodeGen -lLLVMRuntimeDyld -lLLVMExecutionEngine -lLLVMArchive -lLLVMAsmParser -lLLVMAsmPrinter -lLLVMSelectionDAG -lLLVMX86Desc -lLLVMMCParser -lLLVMCodeGen -lLLVMX86AsmPrinter -lLLVMX86Info -lLLVMScalarOpts -lLLVMX86Utils -lLLVMInstCombine -lLLVMTransformUtils -lLLVMipa -lLLVMJIT -lLLVMMCJIT -lLLVMAnalysis -lLLVMTarget -lLLVMCore -lLLVMMC -lLLVMObject -lLLVMSupport
INCLUDEPATH += D:/Programming/Libs/llvm-3.4.msvc10/install_release/include



unix {
    target.path = /usr/lib
    INSTALLS += target
}
