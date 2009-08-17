# Original version from OpenGTL, modified by SKB to look for LLVM on WIN32
#
# Detect LLVM and set various variable to link against the different component of LLVM
#
# LLVM_LIBS : Libraries to link with
# LLVM_INCLUDE_DIR : directory with LLVM include
#
# LLVM_COMPILE_FLAGS : compile flags needed to build a program using LLVM headers
# LLVM_LDFLAGS : ldflags needed to link
# LLVM_LIBS_CORE : ldflags needed to link against a LLVM core library
# LLVM_LIBS_JIT : ldflags needed to link against a LLVM JIT
# LLVM_LIBS_JIT_OBJECTS : objects you need to add to your source when using LLVM JIT

if (LLVM_INCLUDE_DIR)
  set(LLVM_FOUND TRUE)
else (LLVM_INCLUDE_DIR)
  if (MSVC)
    set(LLVM_ROOT "C:/Program Files/LLVM" CACHE PATH "Directory where LLVM is installed")
    macro(FIND_LLVM_MSVC_LIB __libname__  __varname__ )
      find_library(${__varname__} ${__libname__} DOC "Location of ${__libname__}.lib" 
      				  PATHS ${LOCAL_PATH} "${LLVM_ROOT}/lib")
      mark_as_advanced(${__varname__})
    endmacro(FIND_LLVM_MSVC_LIB)
    find_path(LLVM_INCLUDE_DIR NAMES llvm-c/Core.h DOC "Location of llvm-c" 
    			       PATHS ${LOCAL_PATH} ${LLVM_ROOT}/include)
    set(LLVM_LIBS "")			       
    if (LLVM_INCLUDE_DIR)
        FIND_LLVM_MSVC_LIB( LLVMInstrumentation LIB_LLVMInstrumentation )
    	FIND_LLVM_MSVC_LIB( LLVMipo LIB_LLVMipo )
	FIND_LLVM_MSVC_LIB( LLVMX86CodeGen LIB_LLVMX86CodeGen )
    	FIND_LLVM_MSVC_LIB( LLVMSelectionDAG LIB_LLVMSelectionDAG )
    	FIND_LLVM_MSVC_LIB( LLVMAsmPrinter LIB_LLVMAsmPrinter )
    	FIND_LLVM_MSVC_LIB( LLVMExecutionEngine LIB_LLVMExecutionEngine )
    	FIND_LLVM_MSVC_LIB( LLVMJIT LIB_LLVMJIT )
    	FIND_LLVM_MSVC_LIB( LLVMCodeGen LIB_LLVMCodeGen )
    	FIND_LLVM_MSVC_LIB( LLVMScalarOpts LIB_LLVMScalarOpts )
    	FIND_LLVM_MSVC_LIB( LLVMTransformUtils LIB_LLVMTransformUtils )
    	FIND_LLVM_MSVC_LIB( LLVMipa LIB_LLVMipa )
    	FIND_LLVM_MSVC_LIB( LLVMAnalysis LIB_LLVMAnalysis )
    	FIND_LLVM_MSVC_LIB( LLVMTarget LIB_LLVMTarget )
    	FIND_LLVM_MSVC_LIB( LLVMCore LIB_LLVMCore )
    	FIND_LLVM_MSVC_LIB( LLVMSupport LIB_LLVMSupport )
    	FIND_LLVM_MSVC_LIB( LLVMSystem LIB_LLVMSystem )
    	set(LLVM_LIBS ${LIB_LLVMInstrumentation} ${LIB_LLVMipo} ${LIB_LLVMX86CodeGen}	
		  ${LIB_LLVMSelectionDAG} 
    		  ${LIB_LLVMAsmPrinter} ${LIB_LLVMExecutionEngine} ${LIB_LLVMJIT} ${LIB_LLVMCodeGen} 
		  ${LIB_LLVMScalarOpts} ${LIB_LLVMTransformUtils} ${LIB_LLVMipa} ${LIB_LLVMAnalysis} 
		  ${LIB_LLVMTarget} ${LIB_LLVMCore} ${LIB_LLVMSupport} ${LIB_LLVMSystem} 
		  CACHE STRING "LLVM Link Libraries" )
    endif (LLVM_INCLUDE_DIR)
  else (MSVC)
    find_program(LLVM_CONFIG llvm-config)
    execute_process(COMMAND ${LLVM_CONFIG} --libfiles jit nativecodegen ipo instrumentation scalaropts 
    			    OUTPUT_VARIABLE LLVM_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND ${LLVM_CONFIG} --includedir OUTPUT_VARIABLE LLVM_INCLUDE_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
    separate_arguments(LLVM_LIBS) 
 endif (MSVC)
  if(LLVM_INCLUDE_DIR)
    set(LLVM_FOUND TRUE)
  endif(LLVM_INCLUDE_DIR)
  if(LLVM_FOUND)
    message(STATUS "Found LLVM: ${LLVM_INCLUDE_DIR}")
  else(LLVM_FOUND)
    if(LLVM_FIND_REQUIRED)
      message(FATAL_ERROR "Could NOT find LLVM")
    endif(LLVM_FIND_REQUIRED)
  endif(LLVM_FOUND)
endif (LLVM_INCLUDE_DIR)
