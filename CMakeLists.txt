######## CMake debugging macro
MACRO(WRITE_OUT_ALL_VARIABLES)
SET(DstFile ${CMAKE_CURRENT_BINARY_DIR}/AllVariables.txt)

#MESSAGE("DEBUG -- Writing out all variables to file ${DstFile}")
    
FILE(WRITE ${DstFile} "")
GET_CMAKE_PROPERTY(VARS VARIABLES)
FOREACH(var ${VARS})
FILE(APPEND ${DstFile} "${var} \"${${var}}\"\n")
ENDFOREACH(var ${VARS})
ENDMACRO(WRITE_OUT_ALL_VARIABLES)
### End Debugging tools

CMAKE_MINIMUM_REQUIRED(VERSION 2.6 FATAL_ERROR)

PROJECT(FreeMat CXX C )

SET( VERSION "4.0")
SET( RESOURCEDIR "" )
SET( CMAKE_VERBOSE_MAKEFILE TRUE )
SET( LOCAL_PATH ${PROJECT_SOURCE_DIR}/dependencies/bin ${PROJECT_BINARY_DIR} /mingw/lib INTERNAL )

IF( MSVC )
	ADD_DEFINITIONS( /Dsnprintf=_snprintf /DNOMINMAX /DVERSION="${VERSION}" /DRESOURCEDIR="${RESOURCEDIR}")
#	ADD_DEFINITIONS( /D_HAS_ITERATOR_DEBUGGING=0 /D_SECURE_SCL=0 )
ELSE( MSVC )
	ADD_DEFINITIONS( -DVERSION="${VERSION}" -DRESOURCEDIR="${RESOURCEDIR}")
	SET(CMAKE_SHARED_MODULE_LINK_FLAGS "${CMAKE_SHARED_MODULE_LINK_FLAGS} -export-dynamic")
	SET(CMAKE_SHARED_MODULE_LINK_CXX_FLAGS "${CMAKE_SHARED_MODULE_LINK_CXX_FLAGS} -export-dynamic")
ENDIF( MSVC )

FIND_PACKAGE(OpenGL REQUIRED) 

SET(QT_USE_QTNETWORK TRUE)
SET(QT_USE_QTXML TRUE)
SET(QT_USE_QTOPENGL TRUE)
SET(QT_USE_QTSVG TRUE)

FIND_PACKAGE(Qt4 REQUIRED)
#FIND_PACKAGE(Qt4)
ADD_DEFINITIONS( ${QT_DEFINITIONS} ) 

MACRO (QT4_GENERATE_UI infile outfile )
     GET_FILENAME_COMPONENT(infile ${infile} ABSOLUTE)

     ADD_CUSTOM_COMMAND(OUTPUT ${outfile}
        COMMAND ${QT_UIC_EXECUTABLE}
        ARGS -o ${outfile} ${infile}
        DEPENDS ${infile})

     MACRO_ADD_FILE_DEPENDENCIES(${infile} ${outfile})
ENDMACRO (QT4_GENERATE_UI)

SET(OPTIONAL_LIBS "")

######################################################################
# BLAS Package
######################################################################

FIND_LIBRARY(BLAS_SYS_LIBRARY NAMES blas cblas DOC "Location of systems BLAS library" PATHS ${LOCAL_PATH}   /usr/lib/atlas )

IF (BLAS_SYS_LIBRARY)
  SET (SYSTEM_BLAS_AVAILABLE TRUE)
ELSE (BLAS_SYS_LIBRARY)
  SET (SYSTEM_BLAS_AVAILABLE FALSE)
ENDIF (BLAS_SYS_LIBRARY)

OPTION(FORCE_BUNDLED_BLAS "Force use of bundled BLAS" OFF)

IF (FORCE_BUNDLED_BLAS OR NOT SYSTEM_BLAS_AVAILABLE)
  ADD_SUBDIRECTORY(libs/libMath)
  SET (BLAS_LIBRARY blas_ref )
ELSE (FORCE_BUNDLED_BLAS OR NOT SYSTEM_BLAS_AVAILABLE)
  SET (BLAS_LIBRARY ${BLAS_SYS_LIBRARY})
ENDIF (FORCE_BUNDLED_BLAS OR NOT SYSTEM_BLAS_AVAILABLE)

######################################################################
# LAPACK Package
######################################################################

FIND_LIBRARY(LAPACK_SYS_LIBRARY NAMES lapack clapack DOC "Location of systems LAPACK library" PATHS ${LOCAL_PATH}  )

IF (LAPACK_SYS_LIBRARY)
  SET (SYSTEM_LAPACK_AVAILABLE TRUE)
ELSE (LAPACK_SYS_LIBRARY)
  SET (SYSTEM_LAPACK_AVAILABLE FALSE)
ENDIF (LAPACK_SYS_LIBRARY)

OPTION(FORCE_BUNDLED_LAPACK "Force use of bundled LAPACK" OFF)

IF (FORCE_BUNDLED_LAPACK OR NOT SYSTEM_LAPACK_AVAILABLE)
  ADD_SUBDIRECTORY(libs/libMath/libLAPACK_C)
  SET (LAPACK_LIBRARY lapack_c )
ELSE (FORCE_BUNDLED_LAPACK OR NOT SYSTEM_LAPACK_AVAILABLE)
  SET (LAPACK_LIBRARY ${LAPACK_SYS_LIBRARY})
ENDIF (FORCE_BUNDLED_LAPACK OR NOT SYSTEM_LAPACK_AVAILABLE)

######################################################################
# ARPACK Package
######################################################################

FIND_LIBRARY(ARPACK_SYS_LIBRARY NAMES arpack arpack DOC "Location of systems ARPACK library" PATHS ${LOCAL_PATH}  )

IF (ARPACK_SYS_LIBRARY)
  SET (SYSTEM_ARPACK_AVAILABLE TRUE)
ELSE (ARPACK_SYS_LIBRARY)
  SET (SYSTEM_ARPACK_AVAILABLE FALSE)
ENDIF (ARPACK_SYS_LIBRARY)

OPTION(FORCE_BUNDLED_ARPACK "Force use of bundled ARPACK" OFF)

IF (FORCE_BUNDLED_ARPACK OR NOT SYSTEM_ARPACK_AVAILABLE)
  ADD_SUBDIRECTORY(libs/libMath/libARPACK_C)
  SET (ARPACK_LIBRARY arpack_c )
ELSE (FORCE_BUNDLED_ARPACK OR NOT SYSTEM_ARPACK_AVAILABLE)
  SET (ARPACK_LIBRARY ${ARPACK_SYS_LIBRARY})
ENDIF (FORCE_BUNDLED_ARPACK OR NOT SYSTEM_ARPACK_AVAILABLE)
ADD_DEFINITIONS( -DHAVE_ARPACK )

#Find AVCALL Library
FIND_LIBRARY(AVCALL_LIBRARY NAMES avcall DOC "Location of the AVCALL library" PATHS ${LOCAL_PATH})
FIND_PATH(AVCALL_INCLUDE_DIR NAMES avcall.h DOC "Location of avcall.h" PATHS ${LOCAL_PATH} /mingw/include)
IF( AVCALL_LIBRARY AND AVCALL_INCLUDE_DIR ) 
	ADD_DEFINITIONS( -DHAVE_AVCALL )
	INCLUDE_DIRECTORIES( ${AVCALL_INCLUDE_DIR} )
        SET(OPTIONAL_LIBS ${OPTIONAL_LIBS} ${AVCALL_LIBRARY} )
ENDIF( AVCALL_LIBRARY AND AVCALL_INCLUDE_DIR )	

#Find FFTW and FFTWF (single precision)
FIND_LIBRARY(FFTW_LIBRARY NAMES fftw3 fftw3-3 libfftw3-3 DOC "Location of the FFTW library" PATHS ${LOCAL_PATH} )
FIND_PATH(FFTW_INCLUDE_DIR NAMES fftw3.h DOC "Location of fftw3.h" PATHS ${LOCAL_PATH} /mingw/include)
IF( FFTW_LIBRARY AND FFTW_INCLUDE_DIR ) 
	ADD_DEFINITIONS( -DHAVE_FFTW )
	INCLUDE_DIRECTORIES( ${FFTW_INCLUDE_DIR} )
        SET(OPTIONAL_LIBS ${OPTIONAL_LIBS} ${FFTW_LIBRARY} )
ENDIF( FFTW_LIBRARY AND FFTW_INCLUDE_DIR )	

FIND_LIBRARY(FFTWF_LIBRARY NAMES fftw3f fftw3f-3 libfftw3f-3 DOC "Location of the FFTW (single precision) library" PATHS ${LOCAL_PATH} )
IF( FFTWF_LIBRARY AND FFTW_INCLUDE_DIR ) 
	ADD_DEFINITIONS( -DHAVE_FFTWF )
	INCLUDE_DIRECTORIES( ${FFTW_INCLUDE_DIR} )
        SET(OPTIONAL_LIBS ${OPTIONAL_LIBS} ${FFTWF_LIBRARY} )
ENDIF( FFTWF_LIBRARY AND FFTW_INCLUDE_DIR )	

FIND_LIBRARY(ZLIB_LIBRARY NAMES z zlib zlib1 DOC "Location of ZLIB library" PATHS ${LOCAL_PATH})
FIND_PATH(ZLIB_INCLUDE_DIR NAMES zlib.h DOC "Location of zlib.h" PATHS ${LOCAL_PATH})
IF( ZLIB_LIBRARY AND ZLIB_INCLUDE_DIR ) 
	INCLUDE_DIRECTORIES( ${ZLIB_INCLUDE_DIR} )
        SET(OPTIONAL_LIBS ${OPTIONAL_LIBS} ${ZLIB_LIBRARY} )
ENDIF( ZLIB_LIBRARY AND ZLIB_INCLUDE_DIR )	

FIND_LIBRARY(UMFPACK_LIBRARY NAMES umfpack DOC "Location of the UMFPACK library" PATHS /mingw/lib)
FIND_PATH(UMFPACK_INCLUDE_DIR NAMES umfpack.h DOC "Location of umfpack.h" PATHS /mingw/include /usr/include/ufsparse /usr/include/suitesparse)
FIND_LIBRARY(AMD_LIBRARY NAMES amd DOC "Location of the AMD library" PATHS /mingw/lib)
FIND_PATH(AMD_INCLUDE_DIR NAMES amd.h DOC "Location of amd.h" PATHS /mingw/include /usr/include/ufsparse /usr/include/suitesparse)
IF( UMFPACK_LIBRARY AND UMFPACK_INCLUDE_DIR AND AMD_LIBRARY AND AMD_INCLUDE_DIR) 
 	ADD_DEFINITIONS( -DHAVE_UMFPACK )
         INCLUDE_DIRECTORIES( ${UMFPACK_INCLUDE_DIR} )
         INCLUDE_DIRECTORIES( ${AMD_INCLUDE_DIR} )
        SET(OPTIONAL_LIBS ${OPTIONAL_LIBS} ${UMFPACK_LIBRARY} ${AMD_LIBRARY})
ENDIF( UMFPACK_LIBRARY AND UMFPACK_INCLUDE_DIR AND AMD_LIBRARY AND AMD_INCLUDE_DIR )	


###
###ADD_SUBDIRECTORY(libs/libMath/libARPACK_C)
###SET ( ARPACK_LIBRARY arpack_c )
###ADD_SUBDIRECTORY(libs/libMath/libBLAS_C)
###SET ( BLAS_LIBRARY blas_c )
###
####FIND_LIBRARY(ARPACK_LIBRARY NAMES arpack DOC "Location of ARPACK library" PATHS ${LOCAL_PATH})
###   IF( ARPACK_LIBRARY ) 
###     ADD_DEFINITIONS( -DHAVE_ARPACK )
###     SET(OPTIONAL_LIBS ${OPTIONAL_LIBS} ${ARPACK_LIBRARY})
###   ENDIF( ARPACK_LIBRARY )	
###

######################################################################
# VOLPACK Support
######################################################################
OPTION(USE_VOLPACK "Build with VOLPACK support?" OFF)
IF( USE_VOLPACK )
FIND_LIBRARY(VOLPACK_LIBRARY NAMES volpack DOC "Location of VOLPACK library" PATHS ${LOCAL_PATH})
FIND_PATH(VOLPACK_INCLUDE_DIR NAMES volpack.h DOC "Location of volpack.h" PATHS ${LOCAL_PATH})
IF( VOLPACK_LIBRARY AND VOLPACK_INCLUDE_DIR ) 
	INCLUDE_DIRECTORIES( ${VOLPACK_INCLUDE_DIR} )
	ADD_DEFINITIONS( -DHAVE_VOLPACK )
        SET(OPTIONAL_LIBS ${OPTIONAL_LIBS} ${VOLPACK_LIBRARY} )
ENDIF( VOLPACK_LIBRARY AND VOLPACK_INCLUDE_DIR )	
ENDIF( USE_VOLPACK)


######################################################################
# LLVM Support
######################################################################
OPTION(USE_LLVM "Build with LLVM support?" ON)
IF( USE_LLVM )
IF( MSVC )
	SET( LLVM_LIB_DEBUG_PATH ${PROJECT_SOURCE_DIR}/dependencies/llvm/build/lib/Debug )	
	SET( LLVM_LIB_RELEASE_PATH ${PROJECT_SOURCE_DIR}/dependencies/llvm/build/lib/Release )	
	
	MACRO(ADD_LLVM_LINK_LIBS executable libs)
		SET_TARGET_PROPERTIES(${executable}
			PROPERTIES
			LINK_FLAGS "/INCLUDE:_X86TargetMachineModule"
		)
		FOREACH(c ${libs})
			TARGET_LINK_LIBRARIES(${executable} debug ${LLVM_LIB_DEBUG_PATH}/${c} optimized ${LLVM_LIB_RELEASE_PATH}/${c})
		ENDFOREACH(c)
	ENDMACRO(ADD_LLVM_LINK_LIBS executable libs)

	SET( LLVM_SEARCH_PATH ${PROJECT_SOURCE_DIR}/dependencies/llvm/include/llvm )
	FIND_PATH( LLVM_INCLUDE_DIR NAMES Module.h PATHS ${LLVM_SEARCH_PATH} DOC "Top level include directory for LLVM" )

	SET(LLVM_LIBS 	
		LLVMSupport.lib
		LLVMSystem.lib
		LLVMVMCore.lib
		LLVMCodeGen.lib
		LLVMSelectionDAG.lib
		LLVMAsmPrinter.lib
		LLVMBitReader.lib
		LLVMBitWriter.lib
		LLVMTransformUtils.lib
		LLVMInstrumentation.lib
		LLVMScalarOpts.lib
		LLVMipo.lib
		LLVMHello.lib
		LLVMLinker.lib
		LLVMAnalysis.lib
		LLVMipa.lib
		LLVMX86CodeGen.lib
		LLVMX86AsmPrinter.lib
		LLVMExecutionEngine.lib
		LLVMInterpreter.lib
		LLVMJIT.lib
		LLVMTarget.lib
		LLVMAsmParser.lib
		LLVMDebugger.lib
		LLVMArchive.lib
	)
	
	IF( LLVM_INCLUDE_DIR  )
		INCLUDE_DIRECTORIES( ${LLVM_INCLUDE_DIR} ${LLVM_INCLUDE_DIR}/../  ${PROJECT_SOURCE_DIR}/dependencies/llvm/build/include )
		SET( LLVM_FOUND TRUE )
		ADD_DEFINITIONS( -DHAVE_LLVM )
    ENDIF( LLVM_INCLUDE_DIR )
ELSE( MSVC )
	FIND_PROGRAM( LLVM_CONFIG llvm-config )
        EXECUTE_PROCESS(COMMAND ${LLVM_CONFIG} --libfiles jit interpreter nativecodegen ipo instrumentation scalaropts OUTPUT_VARIABLE LLVM_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)
	EXECUTE_PROCESS(COMMAND ${LLVM_CONFIG} --libdir OUTPUT_VARIABLE LLVM_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
	EXECUTE_PROCESS(COMMAND ${LLVM_CONFIG} --includedir OUTPUT_VARIABLE LLVM_INCLUDE_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)

  	SEPARATE_ARGUMENTS( LLVM_LIBS )
#	STRING( REGEX MATCHALL "[^ ;][a-zA-Z0-9/]*\\.o([ ;]|$)" X86TARGET_OBJ "${LLVM_LIB1}" )
#	STRING( REGEX MATCHALL "[^ ;][a-zA-Z0-9/]*\\.a([ ;]|$)" LLVM_LIB "${LLVM_LIB1}" )

	SET( LLVM_FOUND TRUE )
	ADD_DEFINITIONS( -DHAVE_LLVM )
	INCLUDE_DIRECTORIES( ${LLVM_INCLUDE_DIR} )

	MACRO(ADD_LLVM_LINK_LIBS executable libs)
		FOREACH(c ${libs})
			TARGET_LINK_LIBRARIES(${executable} ${c})
		ENDFOREACH(c)
	ENDMACRO(ADD_LLVM_LINK_LIBS executable libs)

ENDIF( MSVC )

ELSE( USE_LLVM)

	MACRO(ADD_LLVM_LINK_LIBS executable libs)
		FOREACH(c ${libs})
			TARGET_LINK_LIBRARIES(${executable} ${c})
		ENDFOREACH(c)
	ENDMACRO(ADD_LLVM_LINK_LIBS executable libs)


ENDIF( USE_LLVM )

#FIND_LIBRARY(LAPACK_LIBRARY NAMES lapack clapack DOC "Location of LAPACK library" PATHS ${LOCAL_PATH}  )

##############################################################################
#PCRE library
FIND_LIBRARY(PCRE_LIBRARY NAMES pcre DOC "Location of PCRE library" PATHS ${LOCAL_PATH})
FIND_PATH(PCRE_INCLUDE_DIR NAMES pcre.h DOC "Location of pcre.h" PATHS ${LOCAL_PATH})
IF( PCRE_LIBRARY AND PCRE_INCLUDE_DIR ) 
	INCLUDE_DIRECTORIES( ${PCRE_INCLUDE_DIR} )
	ADD_DEFINITIONS( -DHAVE_PCRE )
        SET(OPTIONAL_LIBS ${OPTIONAL_LIBS} ${PCRE_LIBRARY} )
ENDIF( PCRE_LIBRARY AND PCRE_INCLUDE_DIR )	

##############################################################################
##LEVMAR library
#FIND_LIBRARY(LEVMAR_LIBRARY NAMES levmar DOC "Location of levmar library" PATHS ${LOCAL_PATH})
#FIND_PATH(LEVMAR_INCLUDE_DIR NAMES lm.h DOC "Location of lm.h" PATHS ${LOCAL_PATH})
#IF (LEVMAR_LIBRARY AND LEVMAR_INCLUDE_DIR)
#   INCLUDE_DIRECTORIES(${LEVMAR_INCLUDE_DIR})
#   ADD_DEFINITIONS( -DHAVE_LEVMAR )
#   SET(OPTIONAL_LIBS ${OPTIONAL_LIBS} ${LEVMAR_LIBRARY} )
#ENDIF(LEVMAR_LIBRARY AND LEVMAR_INCLUDE_DIR)

##############################################################################
#Extra libraries
IF(NOT MSVC)
	FIND_LIBRARY(NCURSES_LIBRARY NAMES ncurses DOC "Location of the ncurses library." PATH /mingw/lib)
	#FIND_LIBRARY(SOCKS_LIBRARY NAMES socks wsock32 DOC "Location of the winsock library." PATH /mingw/lib)
	SET(EXTRA_LIBRARIES ${NCURSES_LIBRARY}  )  
        #FIND_LIBRARY(G2C_LIBRARY NAMES g2c DOC "Location of Fortran to C (g2c) library" PATHS /mingw/lib)
ELSE(NOT MSVC)
	FIND_LIBRARY(SOCKS_LIBRARY NAMES wsock32 ws2_32 DOC "Location of the winsock library." PATH /mingw/lib)
        SET(EXTRA_LIBRARIES ${SOCKS_LIBRARY} )  
ENDIF(NOT MSVC)



###########################################################################
# F2C setup
#IF( MSVC )
#	FIND_PROGRAM( F2C_PROG NAMES f2c.exe f2c PATHS ${LOCAL_PATH} DOC "Fortran to C translator" )
#	FIND_LIBRARY( F2C_LIBRARY NAMES vcf2c f2c PATHS ${LOCAL_PATH} DOC "Fortran to C Library" )
#	FIND_PATH( F2C_INCLUDE_DIR f2c.h PATHS ${LOCAL_PATH} DOC "Path to f2c.h" )
#	INCLUDE_DIRECTORIES(${F2C_INCLUDE_DIR}) 
#	SET( EXTRA_LIBRARIES ${EXTRA_LIBRARIES} ${F2C_LIBRARY} )
#ENDIF( MSVC )

###########################################################################
# Macro for f2c conversion
#MACRO( CONVERT_F2C dep C_FILES )
#	IF( MSVC AND F2C_PROG )
#		FOREACH( nm ${ARGN} )
#			GET_FILENAME_COMPONENT(outfile ${nm} NAME_WE)
#			ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${outfile}.c 
#			  COMMAND ${F2C_PROG} 
#			  ARGS -d${CMAKE_CURRENT_BINARY_DIR} -g -c ${nm}
#			  #MAIN_DEPENDENCY ${dep}
#			  DEPENDS ${nm}
#			  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#			  VERBATIM
#			) 
#			MACRO_ADD_FILE_DEPENDENCIES(${nm} ${CMAKE_CURRENT_BINARY_DIR}/${outfile}.c )
#			SET( ${C_FILES} ${${C_FILES}} ${CMAKE_CURRENT_BINARY_DIR}/${outfile}.c )
#		ENDFOREACH( nm ${ARGN} )
#	ENDIF( MSVC AND F2C_PROG )
#ENDMACRO( CONVERT_F2C dep C_FILES )

INCLUDE_DIRECTORIES( libs/libFreeMat libs/libGraphics 
libs/libMex libs/libXP libs/libCore libs/libFN libs/libMatC ${QT_LIBRARIES} src)   


OPTION(USE_DYNLINK "Link BLAS dynamically" OFF)
IF( USE_DYNLINK )
        INCLUDE_DIRECTORIES( libs/libMath )
        ADD_DEFINITIONS( -DDYN_BLAS )
		INSTALL( FILES ${PROJECT_SOURCE_DIR}/blas.ini DESTINATION bin )
		#INSTALL( FILES ${PROJECT_SOURCE_DIR}/dependencies/bin/blas_generic.dll DESTINATION bin )
ELSE( USE_DYNLINK )
 IF( NOT MSVC )
#        LINK_DIRECTORIES(/usr/lib/atlas)
# 	FIND_LIBRARY(BLAS_LIBRARY NAMES blas atlas DOC "Location of BLAS or ATLAS library" PATHS  ${LOCAL_PATH} /usr/lib/atlas )
 ENDIF( NOT MSVC )
ENDIF( USE_DYNLINK )


if( MSVC )
  add_definitions( -D_CRT_SECURE_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS )
  add_definitions( -D_SCL_SECURE_NO_WARNINGS -DCRT_NONSTDC_NO_WARNINGS )
  add_definitions( -D_SCL_SECURE_NO_DEPRECATE )
  #add_definitions( -wd4146 -wd4503 -wd4996 -wd4800 -wd4244 -wd4624 )
  #add_definitions( -wd4355 -wd4715 )
endif( MSVC )

#ADD_SUBDIRECTORY( dependencies )
ADD_SUBDIRECTORY( libs )
ADD_SUBDIRECTORY( src )

#FILE( GLOB_RECURSE TOOLBOX_M_FILES ${PROJECT_SOURCE_DIR}/src/toolbox/*.m )
#ADD_CUSTOM_COMMAND( OUTPUT toolbox/help/helpgen.m
#  MAIN_DEPENDENCY FreeMat
#  DEPENDS TOOLBOX_M_FILES
#  COMMAND ${FreeMat} -f run_helpgen
#  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
#  COMMENT "Build toolboxes"
#  )
  
##ADD_SUBDIRECTORY( images )
##ADD_SUBDIRECTORY( tools )

####################################################################################
#Portable Audio section
IF( MSVC )
	FIND_LIBRARY(PORTAUDIO_LIBRARY NAMES portaudio_x86.lib DOC "Location of portable audio library" PATHS ${LOCAL_PATH} ${LOCAL_PATH}/Release)
	FIND_PATH(PORTAUDIO_INCLUDE_DIR NAMES portaudio.h DOC "Location of portaudio.h" PATHS ${LOCAL_PATH})
	IF( PORTAUDIO_LIBRARY AND PORTAUDIO_INCLUDE_DIR ) 
		INCLUDE_DIRECTORIES( ${PORTAUDIO_INCLUDE_DIR} )
		ADD_DEFINITIONS( -DHAVE_PORTAUDIO19 )
		SET(OPTIONAL_LIBS ${OPTIONAL_LIBS} ${PORTAUDIO_LIBRARY} )
	ENDIF( PORTAUDIO_LIBRARY AND PORTAUDIO_INCLUDE_DIR )	
ENDIF( MSVC )



####################################################################################
#Install section
IF( MSVC )
	FILE(GLOB INST_DLL "dependencies/bin/*.dll" )
	INSTALL(FILES ${INST_DLL} DESTINATION bin )
ENDIF( MSVC )
        
####################################################################################
#Setup section

INCLUDE(InstallRequiredSystemLibraries)

GET_TARGET_PROPERTY(FreeMat_Loc FreeMat LOCATION)
# Do not run helpgen automatically -- there are too many things that
# have to be checked after it is run.
#INSTALL( CODE "EXECUTE_PROCESS( COMMAND ${FreeMat_Loc} -f run_helpgen WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})" )
#INSTALL(DIRECTORY toolbox DESTINATION share/FreeMat PATTERN "*.m" PATTERN ".svn" EXCLUDE PERMISSIONS WORLD_READ )
#INSTALL(DIRECTORY help DESTINATION share/FreeMat PATTERN "*" PERMISSIONS WORLD_READ )
# INSTALL(FILES
# ${QT_QTCORE_LIBRARY} 
# ${QT_QTGUI_LIBRARY} 
# ${QT_QTNETWORK_LIBRARY} 
# ${QT_QTOPENGL_LIBRARY} 
# ${QT_QTXML_LIBRARY} 
# ${QT_QTSVG_LIBRARY} 
# ${QT_QTMAIN_LIBRARY}
# DESTINATION lib PERMISSIONS WORLD_EXECUTE )

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "FreeMat")
SET(CPACK_PACKAGE_VERSION_MAJOR "3")
SET(CPACK_PACKAGE_VERSION_MINOR "99")
SET(CPACK_PACKAGE_VERSION_PATCH "3607")
SET(CPACK_RESOURCE_FILE_LICENSE ${PROJECT_SOURCE_DIR}/COPYING)
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "FreeMat is a free environment for rapid engineering and scientific prototyping and data processing.")


SET(CPACK_PACKAGE_INSTALL_DIRECTORY "FreeMat")
IF(WIN32 AND NOT UNIX)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  #SET(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\FreeMat.exe")
  SET(CPACK_NSIS_DISPLAY_NAME "FreeMat")
  SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\freemat.sf.net")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\freemat.sf.net")
  #SET(CPACK_NSIS_CONTACT "me@my-personal-home-page.com")
  SET(CPACK_NSIS_MODIFY_PATH ON)

	INSTALL(DIRECTORY toolbox DESTINATION . PATTERN "*" PATTERN ".svn" EXCLUDE PERMISSIONS WORLD_READ )
	INSTALL(DIRECTORY help DESTINATION . PATTERN "*" PATTERN ".svn" EXCLUDE PERMISSIONS WORLD_READ )
	INSTALL(FILES
	 ${QT_LIBRARY_DIR}/qtcore4.dll 
	 ${QT_LIBRARY_DIR}/qtgui4.dll
	 ${QT_LIBRARY_DIR}/qtnetwork4.dll
	 ${QT_LIBRARY_DIR}/qtopengl4.dll
	 ${QT_LIBRARY_DIR}/qtxml4.dll
	 ${QT_LIBRARY_DIR}/qtsvg4.dll
	 DESTINATION bin PERMISSIONS WORLD_EXECUTE )
	 

ELSE(WIN32 AND NOT UNIX)
  #SET(CPACK_STRIP_FILES "bin/MyExecutable")
  #SET(CPACK_SOURCE_STRIP_FILES "")
ENDIF(WIN32 AND NOT UNIX)
SET(CPACK_PACKAGE_EXECUTABLES "FreeMat" "FreeMat")
INCLUDE(CPack)

#TODO Handle gfortran vs g77 issue: libgfortran vs libg2c
WRITE_OUT_ALL_VARIABLES()
