noinst_LIBRARIES = libFreeMat.a

INCLUDES = -I@top_srcdir@/libs/libXP -I@top_srcdir@/libs/libMex -I@top_srcdir@/libs/libMatC

libFreeMat_a_MOC = Interpreter.moc.cpp

BUILT_SOURCES = $(libFreeMat_a_MOC)

libFreeMat_a_HFILES = Array.hpp \
ArrayPrivate.hpp \
Class.hpp \
Context.hpp \
CommonIterator.hpp \
EigenDecompose.hpp \
Exception.hpp \
File.hpp \
FunctionDef.hpp \
GetSet.hpp \
HandleList.hpp \
IEEEFP.hpp \
LAPACK.hpp \
LeastSquaresSolver.hpp \
LinearEqSolver.hpp \
LUDecompose.hpp \
Malloc.hpp \
Math.hpp \
MatrixMultiply.hpp \
MemPtr.hpp \
Module.hpp \
Operators.hpp \
QRDecompose.hpp \
Scope.hpp \
SingularValueDecompose.hpp \
SparseMatrix.hpp \
Stream.hpp \
SymbolTable.hpp \
Types.hpp \
Interpreter.hpp \
Print.hpp \
Token.hpp \
Parser.hpp \
Scanner.hpp \
Tree.hpp \
FastList.hpp \
SparseCCS.hpp

libFreeMat_a_SOURCES = Array.cpp \
Interpreter.cpp \
Context.cpp \
EigenDecompose.cpp \
Exception.cpp \
File.cpp \
IEEEFP.cpp \
LAPACK.cpp \
LeastSquaresSolver.cpp \
LinearEqSolver.cpp \
LUDecompose.cpp \
Malloc.cpp \
Math.cpp \
MatrixMultiply.cpp \
Module.cpp \
QRDecompose.cpp \
Scope.cpp \
SingularValueDecompose.cpp \
SparseMatrix.cpp \
Class.cpp \
Print.cpp \
Token.cpp \
Parser.cpp \
Scanner.cpp \
Tree.cpp \
FunctionDef.cpp \
$(libFreeMat_a_HFILES) 

nodist_libFreeMat_a_SOURCES = $(libFreeMat_a_MOC)

CLEANFILES = $(libFreeMat_a_MOC)

AUTOMAKE_OPTIONS 	= foreign

AM_CPPFLAGS = $(QT_CFLAGS) 

Interpreter.moc.cpp: Interpreter.hpp
	$(QT_MOC) -o $@ $<
