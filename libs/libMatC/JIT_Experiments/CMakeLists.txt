project(jittest CXX)
cmake_minimum_required(VERSION 2.8)


OPTION(USE_LLVM "Build with LLVM support?" ON)

set( LLVM_LIB_LIST 
    clangFrontend
    clangDriver
    clangCodeGen
    clangSema
    clangChecker
    clangIndex
    clangAnalysis
    clangRewrite
    clangAST
    clangParse
    clangLex
    clangBasic
LLVMInstrumentation
LLVMipo
LLVMX86CodeGen
LLVMX86Info
LLVMSelectionDAG
LLVMAsmPrinter
LLVMJIT
LLVMExecutionEngine
LLVMCodeGen
LLVMScalarOpts
LLVMInstCombine
LLVMTransformUtils
LLVMipa
LLVMAnalysis
LLVMTarget
LLVMMC
LLVMBitReader
LLVMBitWriter
LLVMCore
LLVMSupport
LLVMX86AsmParser
LLVMAsmParser
LLVMSystem
)

IF( USE_LLVM )

  set(LLVM_FOUND FALSE)
  if (MSVC)
    set(LLVM_ROOT "C:/Program Files/LLVM" CACHE PATH "Directory where LLVM is installed")
    macro(FIND_LLVM_MSVC_LIB __libname__ )
      find_library(LIB_${__libname__} ${__libname__} DOC "Location of ${__libname__}.lib" 
      				  PATHS ${LOCAL_PATH} "${LLVM_ROOT}/lib")
      mark_as_advanced(LIB_${__libname__})
    endmacro(FIND_LLVM_MSVC_LIB)
    find_path(LLVM_INCLUDE_DIR NAMES llvm-c/Core.h DOC "Location of llvm-c" 
    			       PATHS ${LOCAL_PATH} ${LLVM_ROOT}/include)
    if (LLVM_INCLUDE_DIR)
		foreach( libname IN ITEMS ${LLVM_LIB_LIST} )
			FIND_LLVM_MSVC_LIB( ${libname} )
			list(APPEND LLVM_LIBL ${LIB_${libname}})
		endforeach( )
	set(LLVM_VERSION "2.7" CACHE STRING "LLVM Version found")
	set(LLVM_LIBS ${LLVM_LIBL} CACHE STRING "LLVM Link Libraries" )

    endif (LLVM_INCLUDE_DIR)
  elseif (MINGW)
    set(LLVM_ROOT "C:/Program Files/LLVM-mingw" CACHE PATH "Directory where LLVM is installed")
    macro(FIND_LLVM_MSVC_LIB __libname__ )
      find_library(LIB_${__libname__} ${__libname__} DOC "Location of ${__libname__}.lib"
                                  PATHS ${LOCAL_PATH} "${LLVM_ROOT}/lib")
      mark_as_advanced(LIB_${__libname__})
    endmacro(FIND_LLVM_MSVC_LIB)
    find_path(LLVM_INCLUDE_DIR NAMES llvm-c/Core.h DOC "Location of llvm-c"
                               PATHS ${LOCAL_PATH} ${LLVM_ROOT}/include)
    if (LLVM_INCLUDE_DIR)
                foreach( libname IN ITEMS ${LLVM_LIB_LIST} )
                        FIND_LLVM_MSVC_LIB( ${libname} )
                        list(APPEND LLVM_LIBL ${LIB_${libname}})
                endforeach( )
        set(LLVM_VERSION "2.7" CACHE STRING "LLVM Version found")
        set(LLVM_LIBS ${LLVM_LIBL} CACHE STRING "LLVM Link Libraries" )

    endif (LLVM_INCLUDE_DIR)
  else (MINGW)
    find_program(LLVM_CONFIG llvm-config)
    execute_process(COMMAND ${LLVM_CONFIG} --libfiles jit nativecodegen ipo instrumentation scalaropts 
    			    OUTPUT_VARIABLE LLVM_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND ${LLVM_CONFIG} --includedir OUTPUT_VARIABLE LLVM_INCLUDE_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND ${LLVM_CONFIG} --version OUTPUT_VARIABLE LLVM_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
    separate_arguments(LLVM_LIBS) 
  endif (MSVC)
#  if (NOT LLVM_VERSION STREQUAL "2.5")
#    message(FATAL_ERROR "LLVM version 2.5 required.")
#    set(USE_LLVM FALSE)
#  else (NOT LLVM_VERSION STREQUAL "2.5") 
    set(LLVM_FOUND TRUE)
    include_directories( ${LLVM_INCLUDE_DIR} )
    add_definitions( -DHAVE_LLVM )
    SET(OPTIONAL_LIBS ${OPTIONAL_LIBS} ${LLVM_LIBS})
#  endif (NOT LLVM_VERSION STREQUAL "2.5")
ENDIF (USE_LLVM)



add_executable( jittest 
jittest.cpp
)

TARGET_LINK_LIBRARIES( jittest
${LLVM_LIBS}
imagehlp
psapi
)

#LLVMArchive
#LLVMAsmParser
#LLVMBitReader
#LLVMBitWriter
#LLVMInterpreter
#LLVMLinker
#LLVMMCParser
#LLVMX86AsmPrinter
#LLVMX86Disassembler
