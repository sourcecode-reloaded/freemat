PROJECT(FreeMat CXX Fortran C)

SET( VERSION "3.4")
SET( RESOURCEDIR "" )
SET( CMAKE_VERBOSE_MAKEFILE TRUE )

ADD_DEFINITIONS( -DVERSION=\"${VERSION}\" -DRESOURCEDIR=\"${RESOURCEDIR}\")
MACRO(WRITE_OUT_ALL_VARIABLES)
    SET(DstFile ${CMAKE_CURRENT_BINARY_DIR}/AllVariables.txt)
    
    MESSAGE("DEBUG -- Writing out all variables to file ${DstFile}")
    
    FILE(WRITE ${DstFile} "")
    GET_CMAKE_PROPERTY(VARS VARIABLES)
    FOREACH(var ${VARS})
        FILE(APPEND ${DstFile} "${var} \"${${var}}\"\n")
    ENDFOREACH(var ${VARS})
ENDMACRO(WRITE_OUT_ALL_VARIABLES)
WRITE_OUT_ALL_VARIABLES()

### End Debugging tools

FIND_LIBRARY(BLAS_LIBRARY NAMES blas atlas DOC "Location of BLAS or ATLAS library" PATHS /mingw/lib)
FIND_LIBRARY(LAPACK_LIBRARY NAMES lapack DOC "Location of LAPACK library" PATHS /mingw/lib)
FIND_LIBRARY(G2C_LIBRARY NAMES g2c DOC "Location of Fortran to C (g2c) library" PATHS /mingw/lib)

FIND_PACKAGE(OpenGL REQUIRED) 


SET(QT_USE_QTNETWORK TRUE)
SET(QT_USE_QTXML TRUE)
SET(QT_USE_QTOPENGL TRUE)

FIND_PACKAGE(Qt4 REQUIRED)
ADD_DEFINITIONS( ${QT_DEFINITIONS} ) 

INCLUDE_DIRECTORIES( libs/libFreeMat libs/libGraphics 
libs/libMex libs/libXP libs/libCore libs/libFN  ${QT_LIBRARIES} src)   

ADD_SUBDIRECTORY( libs )
ADD_SUBDIRECTORY( src )
#ADD_SUBDIRECTORY( images )
#ADD_SUBDIRECTORY( tools )
