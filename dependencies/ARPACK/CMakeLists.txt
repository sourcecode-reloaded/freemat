
FILE( GLOB f_src SRC/*.f UTIL/*.f )

CONVERT_F2C( arpack c_src ${f_src} )
ADD_LIBRARY( arpack ${c_src} )
ADD_DEFINITIONS( -DHAVE_ARPACK )

#WRITE_OUT_ALL_VARIABLES()