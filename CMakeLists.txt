IF(WIN32)
	PROJECT(FreeMat CXX C)
ELSE(WIN32)
	PROJECT(FreeMat CXX Fortran C)
ENDIF(WIN32)


SET( VERSION " svn")
SET( RESOURCEDIR "" )
SET( CMAKE_VERBOSE_MAKEFILE TRUE )

ADD_DEFINITIONS( -D'VERSION=\"${VERSION}\"' -D'RESOURCEDIR=\"${RESOURCEDIR}\"')

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
WRITE_OUT_ALL_VARIABLES()
### End Debugging tools

FIND_PACKAGE(OpenGL REQUIRED) 

SET(QT_USE_QTNETWORK TRUE)
SET(QT_USE_QTXML TRUE)
SET(QT_USE_QTOPENGL TRUE)

#FIND_PACKAGE(Qt4 REQUIRED)
FIND_PACKAGE(Qt4)
ADD_DEFINITIONS( ${QT_DEFINITIONS} ) 

MACRO (QT4_GENERATE_UI infile outfile )
     GET_FILENAME_COMPONENT(infile ${infile} ABSOLUTE)

     ADD_CUSTOM_COMMAND(OUTPUT ${outfile}
        COMMAND ${QT_UIC_EXECUTABLE}
        ARGS -o ${outfile} ${infile}
        DEPENDS ${infile})

     MACRO_ADD_FILE_DEPENDENCIES(${infile} ${outfile})
ENDMACRO (QT4_GENERATE_UI)

###########################################################################################
FIND_LIBRARY(BLAS_LIBRARY NAMES blas atlas DOC "Location of BLAS or ATLAS library" PATHS ${PROJECT_BINARY_DIR} /mingw/lib)
FIND_LIBRARY(LAPACK_LIBRARY NAMES lapack DOC "Location of LAPACK library" PATHS ${PROJECT_BINARY_DIR} /mingw/lib)

#Find AVCALL Library
FIND_LIBRARY(AVCALL_LIBRARY NAMES avcall DOC "Location of the AVCALL library" PATHS /mingw/lib)
FIND_PATH(AVCALL_INCLUDE_DIR NAMES avcall.h DOC "Location of avcall.h" PATHS /mingw/include)
IF( AVCALL_LIBRARY AND AVCALL_INCLUDE_DIR ) 
	ADD_DEFINITIONS( -DHAVE_AVCALL )
	INCLUDE_DIRECTORIES( ${AVCALL_INCLUDE_DIR} )
ENDIF( AVCALL_LIBRARY AND AVCALL_INCLUDE_DIR )	

#Find FFTW and FFTWF (single precision)
FIND_LIBRARY(FFTW_LIBRARY NAMES fftw3 DOC "Location of the FFTW library" PATHS ${PROJECT_BINARY_DIR} /mingw/lib)
FIND_PATH(FFTW_INCLUDE_DIR NAMES fftw3.h DOC "Location of fftw3.h" PATHS /mingw/include)
IF( FFTW_LIBRARY AND FFTW_INCLUDE_DIR ) 
	ADD_DEFINITIONS( -DHAVE_FFTW )
	INCLUDE_DIRECTORIES( ${FFTW_INCLUDE_DIR} )
ENDIF( FFTW_LIBRARY AND FFTW_INCLUDE_DIR )	

FIND_LIBRARY(FFTWF_LIBRARY NAMES fftw3f DOC "Location of the FFTW (single precision) library" PATHS ${PROJECT_BINARY_DIR} /mingw/lib )
IF( FFTWF_LIBRARY AND FFTW_INCLUDE_DIR ) 
	ADD_DEFINITIONS( -DHAVE_FFTWF )
	INCLUDE_DIRECTORIES( ${FFTW_INCLUDE_DIR} )
ENDIF( FFTWF_LIBRARY AND FFTW_INCLUDE_DIR )	

#Find UMFPACK
#FIND_LIBRARY(UMFPACK_LIBRARY NAMES umfpack DOC "Location of the UMFPACK library" PATHS /mingw/lib)
#FIND_PATH(UMFPACK_INCLUDE_DIR NAMES umfpack.h DOC "Location of umfpack.h" PATHS /mingw/include)
#FIND_LIBRARY(AMD_LIBRARY NAMES amd DOC "Location of the AMD library" PATHS /mingw/lib)
#FIND_PATH(AMD_INCLUDE_DIR NAMES amd.h DOC "Location of amd.h" PATHS /mingw/include)
#IF( UMFPACK_LIBRARY AND UMFPACK_INCLUDE_DIR AND AMD_LIBRARY AND AMD_INCLUDE_DIR) 
#	ADD_DEFINITIONS( -DHAVE_UMFPACK )
#        INCLUDE_DIRECTORIES( ${UMFPACK_INCLUDE_DIR} )
#        INCLUDE_DIRECTORIES( ${AMD_INCLUDE_DIR} )
#ENDIF( UMFPACK_LIBRARY AND UMFPACK_INCLUDE_DIR AND AMD_LIBRARY AND AMD_INCLUDE_DIR )	

FIND_LIBRARY(ARPACK_LIBRARY NAMES arpack DOC "Location of the ARPACK library" PATHS /mingw/lib)
IF( ARPACK_LIBRARY ) 
  ADD_DEFINITIONS( -DHAVE_ARPACK )
ENDIF( ARPACK_LIBRARY )	

##############################################################################
#Extra libraries
IF(!WIN32)
	FIND_LIBRARY(NCURSES_LIBRARY ncurses DOC "Location of the ncurses library." PATH /mingw/lib)
	FIND_LIBRARY(SOCKS_LIBRARY NAMES socks wsock32 DOC "Location of the winsock library." PATH /mingw/lib)
	SET(EXTRA_LIBRARIES ${NCURSES_LIBRARY} ${SOCKS_LIBRARY} INTERNAL)  
ENDIF(!WIN32)

#IF( WIN32 )
#	FIND_LIBRARY(G2C_LIBRARY NAMES g2c gfortran DOC "Location of Fortran to C (g2c) library" PATHS /mingw/lib)
#ENDIF( WIN32 )

###########################################################################
# F2C setup
IF( WIN32 )
	FIND_PROGRAM( F2C_PROG NAMES f2c.exe DOC "Fortran to C translator" )
	FIND_LIBRARY( F2C_LIBRARY NAMES vcf2c f2c DOC "Fortran to C Library" )
	FIND_PATH( F2C_INCLUDE_DIR f2c.h DOC "Path to f2c.h" )
	SET( F2C_FLAGS STRING "-g -c" INTERNAL )
ENDIF( WIN32 )

INCLUDE_DIRECTORIES( libs/libFreeMat libs/libGraphics 
libs/libMex libs/libXP libs/libCore libs/libFN  ${QT_LIBRARIES} src)   

ADD_SUBDIRECTORY( dependencies )
#ADD_SUBDIRECTORY( libs )
#ADD_SUBDIRECTORY( src )

#ADD_SUBDIRECTORY( images )
#ADD_SUBDIRECTORY( tools )

#TODO Handle gfortran vs g77 issue: libgfortran vs libg2c
