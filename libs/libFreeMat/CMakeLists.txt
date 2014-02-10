
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR}  ${LOCAL_PATH})

#QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/Interpreter.hpp ${CMAKE_CURRENT_BINARY_DIR}/Intrepreter.moc.cpp)
#QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/DebugStream.hpp ${CMAKE_CURRENT_BINARY_DIR}/DebugStream.moc.cpp )

set (FREEMAT_SOURCES
Module.cpp 
Interpreter.cpp 
Array.cpp 
Algorithms.cpp 
Context.cpp 
EigenDecompose.cpp 
Exception.cpp 
IEEEFP.cpp 
IndexArray.cpp 
LAPACK.cpp 
LeastSquaresSolver.cpp 
LinearEqSolver.cpp 
LUDecompose.cpp 
Math.cpp 
MatrixMultiply.cpp 
SparseMatrix.cpp 
Class.cpp 
Print.cpp 
Token.cpp 
Parser.cpp 
OctaveScanner.cpp 
Scanner.cpp 
Tree.cpp 
FuncPtr.cpp 
AnonFunc.cpp 
SparseCCS.cpp 
Stats.cpp
Struct.cpp
Serialize.cpp 
FunctionDef.cpp 
PathSearch.cpp
)

add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Loader.cpp
	DEPENDS ${FREEMAT_SOURCES}
	COMMAND ${PYTHON_EXECUTABLE}
	ARGS ${CMAKE_SOURCE_DIR}/tools/siggen.py ${CMAKE_CURRENT_SOURCE_DIR} FreeMat ${CMAKE_CURRENT_BINARY_DIR} ${FREEMAT_SOURCES}
	COMMENT "Generating function signatures"
	VERBATIM
	)
 
ADD_LIBRARY( FreeMatlib 
${FREEMAT_SOURCES}
${CMAKE_CURRENT_BINARY_DIR}/Loader.cpp
)

ADD_EXECUTABLE( FMT
main.cpp)

TARGET_LINK_LIBRARIES(FMT  Core FN FreeMatlib levmar Mex
			   boost_system boost_filesystem boost_timer boost_chrono boost_regex
			   lapack_c readline blasref 
			   ${EXTRA_LIBRARIES} ${OPTIONAL_LIBS})
