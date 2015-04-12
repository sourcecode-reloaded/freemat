PROJECT(FMT CXX C)
#ENABLE_LANGUAGE(Fortran)


FIND_PACKAGE(PythonInterp REQUIRED)
set(Boost_USE_STATIC_LIBS ON)
FIND_PACKAGE(Boost COMPONENTS system timer chrono filesystem thread date_time)
include_directories(${Boost_INCLUDE_DIRS})

if (NOT APPLE)
set(CMAKE_CXX_FLAGS "-std=c++11" ${CMAKE_CXX_FLAGS})
#FIND_PACKAGE(BLAS REQUIRED)
find_library(OPENBLAS_LIBRARY NAMES openblas)
find_path(OPENBLAS_INCLUDE_DIR NAMES cblas.h DOC "Location of cblas.h")
set(ACCELERATE_LIB  ${OPENBLAS_LIBRARY})
add_definitions(-DHAVE_CBLASH)
include_directories(${OPENBLAS_INCLUDE_DIR})
else ()
set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")
find_library(ACCELERATE_LIB NAMES Accelerate) 
find_path(ACCELERATE_PATH NAMES accelerate.h)
add_definitions(-DHAVE_ACCELERATE)
add_definitions(-DHAVE_CBLAS)
endif ()

include_directories(${ACCELERATE_PATH})

include_directories(${PROJECT_SOURCE_DIR}/Types)  
include_directories(${PROJECT_SOURCE_DIR}/Math)   
include_directories(${PROJECT_SOURCE_DIR}/Compiler)   
include_directories(${PROJECT_SOURCE_DIR}) 

#find_library(VEC_LIB NAMES Veclib) 

find_library(READLINE_LIBRARY NAMES readline readline-32)
find_path(READLINE_INCLUDE_DIR NAMES readline.h)
include_directories(${READLINE_INCLUDE_DIR})

set(HEADERS
Algorithms.hpp
ArrayFormatInfo.hpp
BinOp.hpp
Complex.hpp
Convert.hpp
Debug.hpp
Exception.hpp
FMLib.hpp
FileSystem.hpp
FixedPool.hpp
Frame.hpp
GarbageCollector.hpp
Globals.hpp
HandleClass.hpp
HashMap.hpp
IEEEFP.hpp
LineNumbers.hpp
Math.hpp
MatrixDivide.hpp
MatrixLeftDivide.hpp
MatrixRightDivide.hpp
MemPtr.hpp
NCat.hpp
Object.hpp
OpAdd.hpp
OpColon.hpp
Operators.hpp
TermIF.hpp
ThreadContext.hpp
Tuple.hpp
TypeUtils.hpp
UnaryOp.hpp
VM.hpp
Compiler/Assembler.hpp
Compiler/Code.hpp
Compiler/EndRemover.hpp
Compiler/NestedFunctionMover.hpp
Compiler/Parser.hpp
Compiler/Register.hpp
Compiler/Scanner.hpp
Compiler/Symbol.hpp
Compiler/Token.hpp
Compiler/Tree.hpp
Math/BLAS.hpp
Math/DenseSolver.hpp
Math/EigenDecompose.hpp
Math/Hermitian.hpp
Math/LAPACK.hpp
Math/MatrixInvert.hpp
Math/MatrixMultiply.hpp
Math/MatrixPower.hpp
Math/PowFunc.hpp
Math/Transpose.hpp
Types/AggregateType.hpp
Types/AllTypes.hpp
Types/AnonymousType.hpp
Types/ArrayType.hpp
Types/BoolType.hpp
Types/BoundFunctionType.hpp
Types/BreakpointType.hpp
Types/CapturedType.hpp
Types/CellType.hpp
Types/ClassType.hpp
Types/CodeType.hpp
Types/DoubleType.hpp
Types/FloatType.hpp
Types/FunctionHandleType.hpp
Types/FunctionType.hpp
Types/HandleType.hpp
Types/IntegerType.hpp
Types/ListType.hpp
Types/ModuleType.hpp
Types/NumericType.hpp
Types/ObjectArrayType.hpp
Types/PODType.hpp
Types/SingleType.hpp
Types/SparseType.hpp
Types/StringType.hpp
Types/StructType.hpp
Types/Type.hpp
)

add_executable( FMT
${HEADERS}
Types/IntegerType.cpp  
Types/BoolType.cpp  
Types/FloatType.cpp  
Types/SingleType.cpp  
Types/ListType.cpp  
Types/StringType.cpp  
Types/AnonymousType.cpp  
Types/FunctionHandleType.cpp  
Types/FunctionType.cpp  
Types/CellType.cpp  
Types/Type.cpp   
Types/DoubleType.cpp  
Types/ClassType.cpp
Types/ArrayType.cpp
Types/ObjectArrayType.cpp
Types/BoundFunctionType.cpp
Types/StructType.cpp
Types/NumericType.cpp
Types/ModuleType.cpp
Types/BreakpointType.cpp
Types/CodeType.cpp
Types/SparseType.cpp
Math/BLAS.cpp
Math/DenseSolver.cpp
Math/Transpose.cpp  
Math/Hermitian.cpp  
Math/LAPACK.cpp  
Math/MatrixMultiply.cpp
Math/EigenDecompose.cpp
Math/MatrixPower.cpp
Math/MatrixInvert.cpp
Compiler/Tree.cpp
Compiler/EndRemover.cpp
Compiler/NestedFunctionMover.cpp
Compiler/Token.cpp
Compiler/Scanner.cpp
Compiler/Parser.cpp
Compiler/Register.cpp
Compiler/Compiler.cpp
Compiler/Symbol.cpp
Compiler/Assembler.cpp
NCat.cpp
HashMap.cpp
ThreadContext.cpp
MatrixLeftDivide.cpp  
MatrixRightDivide.cpp  
main.cpp
#IEEEFP.cpp
FMLib.cpp
Frame.cpp
VM.cpp
ArrayFormatInfo.cpp
GarbageCollector.cpp
HandleClass.cpp
Debug.cpp
Globals.cpp
)

target_link_libraries(FMT  ${READLINE_LIBRARY} ${ACCELERATE_LIB} ${Boost_LIBRARIES})

add_custom_command(
OUTPUT ${CMAKE_SOURCE_DIR}/OpCodes.cpp ${CMAKE_SOURCE_DIR}/OpCodes.h
DEPENDS OpCodes.csv
COMMAND ${PYTHON_EXECUTABLE} 
ARGS ${CMAKE_SOURCE_DIR}/opcode.py ${CMAKE_SOURCE_DIR}/OpCodes.csv ${CMAKE_SOURCE_DIR}
COMMENT "Generating OpCode tables"
VERBATIM
)
