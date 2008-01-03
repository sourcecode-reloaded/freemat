
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} )

QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/Interpreter.hpp ${CMAKE_CURRENT_SOURCE_DIR}/Intrepreter.moc.cpp)
 
ADD_LIBRARY( FreeMatlib 
Array.cpp 
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
#JITVM.cpp 
FunctionDef.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/Intrepreter.moc.cpp
)

