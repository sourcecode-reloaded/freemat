INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR})

add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Loader.cpp
	DEPENDS MainApp.cpp
	COMMAND ${PYTHON_EXECUTABLE}
	ARGS ${CMAKE_SOURCE_DIR}/tools/siggen.py ${CMAKE_CURRENT_SOURCE_DIR} Main ${CMAKE_CURRENT_BINARY_DIR}
	COMMENT "Generating function signatures"
	VERBATIM
	)

ADD_EXECUTABLE( FreeMat MACOSX_BUNDLE WIN32
main.cpp
# Added per: http://www.cmake.org/pipermail/cmake/2010-July/038172.html
dummy.f
)

#${X86TARGET_OBJ}

#IF( MSVC )
#	SET( LAPACK_LIBRARY ${lapack} )
#ENDIF( MSVC )

IF( USE_DYNLINK )
	TARGET_LINK_LIBRARIES( FreeMat blas )
ENDIF( USE_DYNLINK )



TARGET_LINK_LIBRARIES( FreeMat
Core
FN
FreeMatlib
Mex
MatC
levmar
lapack_c
dynblas
blasref
${EXTRA_LIBRARIES}
${OPTIONAL_LIBS}
)


WRITE_OUT_ALL_VARIABLES()
