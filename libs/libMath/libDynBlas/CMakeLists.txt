
ADD_DEFINITIONS( -DDYN_BLAS )
ADD_LIBRARY( dynblas
 	blas_wrap.cpp blas_dyn_link.cpp
	)



