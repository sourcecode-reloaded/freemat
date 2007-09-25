
FILE( GLOB bf_src src/*.f )
CONVERT_F2C( blas bc_src ${bf_src} )
ADD_LIBRARY( blas ${bc_src} )
GET_TARGET_PROPERTY(BLAS_LIBRARY blas LOCATION)
WRITE_OUT_ALL_VARIABLES()


