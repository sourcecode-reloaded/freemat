
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

WRITE_OUT_ALL_VARIABLES()


