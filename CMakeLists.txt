PROJECT(FreeMat)

SET( VERSION "3.4")
ADD_DEFINITIONS( -DVERSION=${VERSION} )
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


ADD_SUBDIRECTORY( libs )
#ADD_SUBDIRECTORY( src )
#ADD_SUBDIRECTORY( images )
#ADD_SUBDIRECTORY( tools )

