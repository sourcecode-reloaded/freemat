FILE( GLOB f_src src/*.f )
CONVERT_F2C( lapack c_src ${f_src} )
ADD_LIBRARY( lapack ${c_src} )

GET_TARGET_PROPERTY(LAPACK_LIBRARY lapack LOCATION)

IF( !BLAS_LIBRARY )
	FILE( GLOB bf_src blas/src/*.f )
	CONVERT_F2C( lapack bc_src ${bf_src} )
	ADD_LIBRARY( blas ${bc_src} )
	GET_TARGET_PROPERTY(BLAS_LIBRARY blas LOCATION)
ENDIF( !BLAS_LIBRARY )
WRITE_OUT_ALL_VARIABLES()


