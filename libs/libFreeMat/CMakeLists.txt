
FIND_PACKAGE(Qt4)

QT4_WRAP_CPP( Intrepreter.moc.cpp Interpreter.hpp )
ADD_DEFINITIONS( ${QT_DEFINITIONS} ) 
INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} )

 
ADD_LIBRARY( FreeMat Array.cpp 
Interpreter.cpp 
Context.cpp 
Data.cpp 
Dimensions.cpp 
EigenDecompose.cpp 
Exception.cpp 
File.cpp 
IEEEFP.cpp 
LAPACK.cpp 
LeastSquaresSolver.cpp 
LinearEqSolver.cpp 
LUDecompose.cpp 
Malloc.cpp 
Math.cpp 
MatrixMultiply.cpp 
Module.cpp 
NumericArray.cpp 
QRDecompose.cpp 
Scope.cpp 
Serialize.cpp 
SingularValueDecompose.cpp 
Sparse.cpp 
Class.cpp 
Print.cpp 
Token.cpp 
Parser.cpp 
Scanner.cpp 
Tree.cpp 
JITVM.cpp 
FunctionDef.cpp 
Interpreter.moc.cpp )

