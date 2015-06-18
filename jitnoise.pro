TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

HEADERS += $$files(include/jitnoise/*.h, true)
HEADERS += $$files(src/*.h, true)
SOURCES += $$files(src/*.cpp, true)

LIBS += -L$$(LLVM_LIB)
#LIBS += -lLLVMVectorize -lLLVMipo -lLLVMLinker -lLLVMBitReader -lLLVMBitWriter -lLLVMInstrumentation -lLLVMX86CodeGen -lLLVMRuntimeDyld -lLLVMExecutionEngine -lLLVMArchive -lLLVMAsmParser -lLLVMAsmPrinter -lLLVMSelectionDAG -lLLVMX86Desc -lLLVMMCParser -lLLVMCodeGen -lLLVMX86AsmPrinter -lLLVMX86Info -lLLVMScalarOpts -lLLVMX86Utils -lLLVMInstCombine -lLLVMTransformUtils -lLLVMipa -lLLVMJIT -lLLVMMCJIT -lLLVMAnalysis -lLLVMTarget -lLLVMCore -lLLVMMC -lLLVMObject -lLLVMSupport
LIBS += -lLLVMX86Disassembler -lLLVMX86AsmParser -lLLVMMCJIT -lLLVMIRReader -lLLVMAsmParser -lLLVMTableGen -lLLVMExecutionEngine -lLLVMRuntimeDyld -lLLVMX86CodeGen -lLLVMX86Desc -lLLVMX86Info -lLLVMMCDisassembler -lLLVMX86AsmPrinter -lLLVMX86Utils -lLLVMSelectionDAG -lLLVMAsmPrinter -lLLVMInstrumentation -lLLVMLineEditor -lLLVMDebugInfo -lLLVMLTO -lLLVMCodeGen -lLLVMObjCARCOpts -lLLVMLinker -lLLVMipo -lLLVMVectorize -lLLVMScalarOpts -lLLVMProfileData -lLLVMObject -lLLVMMCParser -lLLVMInstCombine -lLLVMTransformUtils -lLLVMipa -lLLVMAnalysis -lLLVMTarget -lLLVMMC -lLLVMBitWriter -lLLVMBitReader -lLLVMCore -lLLVMSupport
INCLUDEPATH += $$(LLVM_INCLUDE)

LIBS += -lAdvapi32 -lShell32
win32-msvc { #msvc
        QMAKE_CXXFLAGS += /Zc:wchar_t
        LIBS += -ldbghelp
} else { #mingw
        LIBS += -lpsapi -limagehlp # = dbghelp (msvc)
}


INCLUDEPATH += include
INCLUDEPATH += src

DEFINES += __STDC_LIMIT_MACROS __STDC_CONSTANT_MACROS

PRECOMPILED_HEADER = src/llvm.h

