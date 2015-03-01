PROJECT(FMT CXX C)
#ENABLE_LANGUAGE(Fortran)

set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")

FIND_PACKAGE(PythonInterp REQUIRED)
FIND_PACKAGE(Boost COMPONENTS system timer chrono filesystem)
include_directories(${Boost_INCLUDE_DIRS})

#FIND_PACKAGE(BLAS REQUIRED)

find_library(ACCELERATE_LIB NAMES Accelerate) 
find_path(ACCELERATE_PATH NAMES accelerate.h)
add_definitions(-DHAVE_ACCELERATE)
add_definitions(-DHAVE_CBLAS)

include_directories(${ACCELERATE_PATH})

#find_library(VEC_LIB NAMES Veclib) 

#FIND_PACKAGE(readline)
#FIND_PACKAGE(history)

add_executable( FMT
Tree.cpp
BLAS.cpp
NCat.cpp
DenseSolver.cpp
Type.cpp
HashMap.cpp
ThreadContext.cpp
DoubleType.cpp
CellType.cpp
EndRemover.cpp
NestedFunctionMover.cpp
FunctionType.cpp
FunctionHandleType.cpp
AnonymousType.cpp
CodeType.cpp
StringType.cpp
Transpose.cpp  
Hermitian.cpp  
LAPACK.cpp
ListType.cpp
MatrixLeftDivide.cpp  
MatrixRightDivide.cpp  
SingleType.cpp
FloatType.cpp
Token.cpp
Scanner.cpp
Parser.cpp
Register.cpp
Compiler.cpp
Symbol.cpp
main.cpp
BaseTypes.hpp
BinOp.hpp
Code.hpp
Compiler.hpp
Complex.hpp
BoolType.hpp
BoolType.cpp
DoubleType.hpp
Exception.hpp
FixedPool.hpp
FMLib.hpp
IntegerType.hpp
IntegerType.cpp
Object.hpp
#IEEEFP.cpp
Operators.hpp
OpCodes.h
ClassType.hpp
ClassType.cpp
Parser.hpp
ArrayType.hpp
ArrayType.cpp
ObjectArrayType.cpp
BoundFunctionType.cpp
PODType.hpp
PODType.cpp
Register.hpp
Scanner.hpp
SingleType.hpp
StringType.hpp
StructType.hpp
StructType.cpp
Symbol.hpp
Token.hpp
Tree.hpp
Tuple.hpp
Type.hpp
Assembler.cpp
Assembler.hpp
Frame.cpp
Frame.hpp
NumericType.cpp
NumericType.hpp
VM.cpp
VM.hpp
MatrixMultiply.cpp
ArrayFormatInfo.cpp
ArrayFormatInfo.hpp
ModuleType.cpp
BuiltInType.cpp
GarbageCollector.cpp
HandleClass.cpp
Debug.cpp
)
target_link_libraries(FMT ${Boost_LIBRARIES} readline ${ACCELERATE_LIB})

add_custom_command(
OUTPUT ${CMAKE_SOURCE_DIR}/OpCodes.cpp ${CMAKE_SOURCE_DIR}/OpCodes.h
DEPENDS OpCodes.csv
COMMAND ${PYTHON_EXECUTABLE} 
ARGS ${CMAKE_SOURCE_DIR}/opcode.py ${CMAKE_SOURCE_DIR}/OpCodes.csv ${CMAKE_SOURCE_DIR}
COMMENT "Generating OpCode tables"
VERBATIM
)
