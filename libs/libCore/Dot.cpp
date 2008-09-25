#include "Array.hpp"
#include "Math.hpp"

//!
//@Module DOT Dot Product Function
//@@Section ELEMENTARY
//@@Usage
//Computes the scalar dot product of its two arguments.  The general
//syntax for its use is
//@[
//  y = dot(x,z)
//@]
//where @|x| and @|z| are numerical vectors of the same length.  If 
//@|x| and @|z| are multi-dimensional arrays of the same size, then
//the dot product is taken along the first non-singleton dimension.
//You can also specify the dimension to take the dot product along using
//the alternate form
//@[
//  y = dot(x,z,dim)
//@]
//where @|dim| specifies the dimension to take the dot product along.
//@@Tests
//@$y1=dot(x1,x2)
//@$y1=dot(x1,x2,2)
//@@Signature
//function dot DotFunction
//inputs x1 x2 dim
//outputs y
//!


template <typename T>
static ArrayVector DotFunction(const BasicArray<T> &xreal,
			       const BasicArray<T> &ximag,
			       const BasicArray<T> &yreal,
			       const BasicArray<T> &yimag,
			       int DotDim) {
  BasicArray<T> rvec_real(xreal.dimensions().forceOne(DotDim));
  BasicArray<T> rvec_imag(ximag.dimensions().forceOne(DotDim));
  ConstBasicIterator<T> xsource_real(&xreal,DotDim);
  ConstBasicIterator<T> xsource_imag(&ximag,DotDim);
  ConstBasicIterator<T> ysource_real(&yreal,DotDim);
  ConstBasicIterator<T> ysource_imag(&yimag,DotDim);
  BasicIterator<T> dest_real(&rvec_real,DotDim);
  BasicIterator<T> dest_imag(&rvec_imag,DotDim);
  index_t dimLen = xreal.dimensions()[DotDim];
  while (xsource_real.isValid() && 
	 ysource_real.isValid() && 
	 xsource_imag.isValid() &&
	 ysource_imag.isValid() &&
	 dest_real.isValid() &&
	 dest_imag.isValid()) {
    double accum_real = 0;
    double accum_imag = 0;
    for (index_t i=1;i<=dimLen;i++) {
      accum_real += xsource_real.get()*ysource_real.get() +
	xsource_imag.get()*ysource_imag.get();
      accum_imag += xsource_real.get()*ysource_imag.get() - 
	xsource_imag.get()*ysource_real.get();
      xsource_real.next(); ysource_real.next();
      xsource_imag.next(); ysource_imag.next();
    }
    dest_real.set(accum_real);
    dest_imag.set(accum_imag);
    dest_real.next(); dest_imag.next();
    xsource_real.nextSlice(); ysource_real.nextSlice(); dest_real.nextSlice();
    xsource_imag.nextSlice(); ysource_imag.nextSlice(); dest_imag.nextSlice();
  }
  return Array(rvec_real,rvec_imag);
}

template <typename T>
static ArrayVector DotFunction(const BasicArray<T> &xreal,
			       const BasicArray<T> &yreal,
			       int DotDim) {
  BasicArray<T> rvec(xreal.dimensions().forceOne(DotDim));
  ConstBasicIterator<T> xsource(&xreal,DotDim);
  ConstBasicIterator<T> ysource(&yreal,DotDim);
  BasicIterator<T> dest(&rvec,DotDim);
  index_t dimLen = xreal.dimensions()[DotDim];
  while (xsource.isValid() && ysource.isValid() && dest.isValid()) {
    double accum = 0;
    for (index_t i=1;i<=dimLen;i++) {
      accum += xsource.get()*ysource.get();
      xsource.next(); ysource.next();
    }
    dest.set(accum);
    dest.next();
    xsource.nextSlice(); ysource.nextSlice(); dest.nextSlice();
  }
  return Array(rvec);
}

ArrayVector SumFunction(int, const ArrayVector&);

template <typename T>
static ArrayVector DotFunction(const Array &x, const Array &y, int DotDim) {
  if (x.isSparse()) {
    Array z(DotMultiply(x,y));
    ArrayVector v;
    v.push_back(z);
    v.push_back(Array(DotDim+1));
    return SumFunction(1,v);
  } else {
    if (x.allReal()) 
      return DotFunction(x.constReal<T>(),y.constReal<T>(),DotDim);
    else 
      return DotFunction(x.constReal<T>(),x.constImag<T>(),
			 y.constReal<T>(),y.constImag<T>(),DotDim);
  }
}

ArrayVector DotFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 2) throw Exception("dot requires at least two arguments");
  Array x(arg[0]);
  Array y(arg[1]);
  if (x.isVector() && y.isVector() && arg.size() == 2) {
    x.reshape(NTuple(x.length(),1));
    y.reshape(NTuple(y.length(),1));
  }
  int DotDim;
  if (arg.size() > 2) {
    DotDim = arg[2].asInteger() - 1;
    if (DotDim < 0) throw Exception("dimension argument to dot should be positive");
  } else
    DotDim = x.dimensions().firstNonsingular();
  if (x.dimensions() != y.dimensions()) 
    throw Exception("arguments to dot should be the same size");
  if (x.isSparse() ^ y.isSparse()) {
    x = x.asDenseArray();
    y = y.asDenseArray();
  }
  if (!x.allReal() || !y.allReal()) {
    x.forceComplex();
    y.forceComplex();
  }
  if ((x.dataClass() == Float) || (y.dataClass() == Float)) {
    x = x.toClass(Float);
    y = y.toClass(Float);
  }
  if (x.dataClass() == Float)
    return DotFunction<float>(x,y,DotDim);
  else
    return DotFunction<double>(x,y,DotDim);
}
