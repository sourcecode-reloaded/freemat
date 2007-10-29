
FILE( GLOB bf_src src/*.f )
CONVERT_F2C( blas_ref bc_src ${bf_src} )
IF( USE_DYNLINK )
	ADD_LIBRARY( blas_ref SHARED ${bc_src} blas_ref.def)
	TARGET_LINK_LIBRARIES( blas_ref 
	${G2C_LIBRARY}
	${F2C_LIBRARY}
	)
ELSE( USE_DYNLINK )
	ADD_LIBRARY( blas ${bc_src} )
	GET_TARGET_PROPERTY(BLAS_LIBRARY blas LOCATION)
ENDIF( USE_DYNLINK )

WRITE_OUT_ALL_VARIABLES()


