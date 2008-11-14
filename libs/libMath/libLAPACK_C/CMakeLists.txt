FILE( GLOB c_src *.c )
ADD_LIBRARY( lapack_c ${c_src} )
