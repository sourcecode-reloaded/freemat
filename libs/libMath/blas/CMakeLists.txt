###########################################################################
# F2C setup
IF( MSVC )
	FIND_PROGRAM( F2C_PROG NAMES f2c.exe f2c PATHS ${LOCAL_PATH} DOC "Fortran to C translator" )
	FIND_LIBRARY( F2C_LIBRARY NAMES vcf2c f2c PATHS ${LOCAL_PATH} DOC "Fortran to C Library" )
	FIND_PATH( F2C_INCLUDE_DIR f2c.h PATHS ${LOCAL_PATH} DOC "Path to f2c.h" )
	INCLUDE_DIRECTORIES(${F2C_INCLUDE_DIR}) 
	SET( EXTRA_LIBRARIES ${EXTRA_LIBRARIES} ${F2C_LIBRARY} )
ENDIF( MSVC )

###########################################################################
# Macro for f2c conversion
MACRO( CONVERT_F2C dep C_FILES )
	IF( MSVC AND F2C_PROG )
		FOREACH( nm ${ARGN} )
			GET_FILENAME_COMPONENT(outfile ${nm} NAME_WE)
			ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${outfile}.c 
			  COMMAND ${F2C_PROG} 
			  ARGS -d${CMAKE_CURRENT_BINARY_DIR} -g -c ${nm}
			  #MAIN_DEPENDENCY ${dep}
			  DEPENDS ${nm}
			  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			  VERBATIM
			) 
			MACRO_ADD_FILE_DEPENDENCIES(${nm} ${CMAKE_CURRENT_BINARY_DIR}/${outfile}.c )
			SET( ${C_FILES} ${${C_FILES}} ${CMAKE_CURRENT_BINARY_DIR}/${outfile}.c )
		ENDFOREACH( nm ${ARGN} )
	ENDIF( MSVC AND F2C_PROG )
ENDMACRO( CONVERT_F2C dep C_FILES )

FILE( GLOB bf_src src/*.f )

IF( USE_DYNLINK )
  IF( MSVC )
    CONVERT_F2C( blas_ref bc_src ${bf_src} )
    ADD_LIBRARY( blas_ref SHARED ${bc_src} blas_ref.def)
  ELSE( MSVC )
    ADD_LIBRARY( blas_ref SHARED ${bf_src})
  ENDIF( MSVC )
    
      TARGET_LINK_LIBRARIES( blas_ref 
      ${G2C_LIBRARY}
      ${F2C_LIBRARY}
      )
      INSTALL(TARGETS blas_ref RUNTIME DESTINATION bin LIBRARY DESTINATION bin )
  ELSE( USE_DYNLINK )
    IF( MSVC )
      CONVERT_F2C( blas_ref bc_src ${bf_src} )
      ADD_LIBRARY( blas ${bc_src} )
    ELSE( MSVC )
      ADD_LIBRARY( blas ${bf_src} )
    ENDIF( MSVC )
    
    GET_TARGET_PROPERTY(BLAS_LIBRARY blas LOCATION)
    
ENDIF( USE_DYNLINK )



