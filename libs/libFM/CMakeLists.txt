PROJECT(FMT CXX C)
#ENABLE_LANGUAGE(Fortran)


FIND_PACKAGE(PythonInterp REQUIRED)
FIND_PACKAGE(Boost COMPONENTS system timer chrono filesystem)
include_directories(${Boost_INCLUDE_DIRS})


if (NOT APPLE)
set(CMAKE_CXX_FLAGS "-std=c++11")
#FIND_PACKAGE(BLAS REQUIRED)
find_library(BLAS_LIBRARIES NAMES openblas)
FIND_PACKAGE(LAPACK REQUIRED)
set(ACCELERATE_LIB ${LAPACK_LIBRARIES} ${BLAS_LIBRARIES})
add_definitions(-DHAVE_CBLASH)
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

#FIND_PACKAGE(readline)
#FIND_PACKAGE(history)

add_executable( FMT
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
Frame.cpp
VM.cpp
ArrayFormatInfo.cpp
GarbageCollector.cpp
HandleClass.cpp
Debug.cpp
Globals.cpp
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
