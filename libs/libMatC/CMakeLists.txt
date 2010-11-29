
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} JIT_Experiments)

ADD_LIBRARY( MatC 
	JIT.cpp 
	JITFunc.cpp
	JIT_Experiments/JITCompiler.cpp
)

TARGET_LINK_LIBRARIES( MatC
    clangCodeGen
    libclang
)

set( CLANG_LIB_SOURCES 
    ${CMAKE_SOURCE_DIR}/libs/libFreeMat/Array.cpp
    ${CMAKE_SOURCE_DIR}/libs/libFreeMat/IndexArray.cpp
    ${CMAKE_SOURCE_DIR}/libs/libFreeMat/Exception.cpp
)



configure_file( sources.txt.in sources.txt )
configure_file( header_paths.txt.in header_paths.txt )

# add_executable( jitlib
#     jitlib.cpp
#     JIT_Experiments/JITCompiler.cpp
# )
# 
# TARGET_LINK_LIBRARIES( jitlib
#     clangCodeGen
#     libclang
#     ${QT_QTCORE_LIBRARY}
#     ${OPTIONAL_LIBS}
# )

#TODO: Fix the flags!
add_custom_command( TARGET MatC PRE_BUILD
    COMMAND clang++ ARGS -O3 -emit-llvm -c ${CLANG_LIB_SOURCES}  -I/usr/include/QtCore -I/usr/include/QtGui -I/home/eugening/freemat/FreeMat4/libs/libFreeMat -I/home/eugening/freemat/FreeMat4/libs/libXP -I/home/eugening/freemat/FreeMat4/libs/libMex -I/home/eugening/freemat/FreeMat4/libs/libMatC
    COMMAND llvm-ld *.o -stats -time-passes -link-as-library -o libF.bc
    DEPENDS ${CLANG_LIB_SOURCES}
)
