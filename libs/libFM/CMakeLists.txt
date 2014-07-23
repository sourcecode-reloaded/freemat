PROJECT(FMT CXX C)
#ENABLE_LANGUAGE(Fortran)

FIND_PACKAGE(PythonInterp REQUIRED)
FIND_PACKAGE(Boost COMPONENTS system timer chrono)
include_directories(${Boost_INCLUDE_DIRS})

#FIND_PACKAGE(readline)
#FIND_PACKAGE(history)

add_executable( FMT
Tree.cpp
DoubleType.cpp
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
Operators.hpp
OpCodes.h
Parser.hpp
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
Type.cpp
Assembler.cpp
Assembler.hpp
Frame.cpp
Frame.hpp
NumericType.cpp
NumericType.hpp
VM.cpp
VM.hpp
ArrayFormatInfo.cpp
ArrayFormatInfo.hpp
)
target_link_libraries(FMT ${Boost_LIBRARIES} readline)

add_custom_command(
OUTPUT ${CMAKE_SOURCE_DIR}/OpCodes.cpp ${CMAKE_SOURCE_DIR}/OpCodes.h
DEPENDS OpCodes.csv
COMMAND ${PYTHON_EXECUTABLE} 
ARGS ${CMAKE_SOURCE_DIR}/opcode.py ${CMAKE_SOURCE_DIR}/OpCodes.csv ${CMAKE_SOURCE_DIR}
COMMENT "Generating OpCode tables"
VERBATIM
)
