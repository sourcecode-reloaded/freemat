FILE( GLOB f_src src/*.f )
CONVERT_F2C( lapack c_src ${f_src} )
ADD_LIBRARY( lapack ${c_src} src/dsecnd.c src/second.c )

GET_TARGET_PROPERTY(LAPACK_LIBRARY lapack LOCATION)
WRITE_OUT_ALL_VARIABLES()


