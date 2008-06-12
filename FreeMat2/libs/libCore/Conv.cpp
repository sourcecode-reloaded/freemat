#include "Array.hpp"
#include "Math.hpp"
  
template <class T>
static void Conv2MainReal(T* C, const T* A, const T*B,
			  int Am, int An, int Bm, int Bn, 
			  int Cm, int Cn, int Cm_offset, int Cn_offset) {
  for (int n=0;n<Cn;n++)
    for (int m=0;m<Cm;m++) {
      T accum = 0;
      int iMin, iMax;
      int jMin, jMax;
      iMin = std::max(0,m+Cm_offset-Bm+1);
      iMax = std::min(Am-1,m+Cm_offset);
      jMin = std::max(0,n+Cn_offset-Bn+1);
      jMax = std::min(An-1,n+Cn_offset);
      for (int j=jMin;j<=jMax;j++)
	for (int i=iMin;i<=iMax;i++)
	  accum += A[i+j*Am]*B[(m+Cm_offset-i)+(n+Cn_offset-j)*Bm];
      C[m+n*Cm] = accum;
    }
}

template <class T>
static void Conv2MainComplex(T* Cr, T* Ci,
			     const T* Ar, const T* Ai,
			     const T* Br, const T* Bi,
			     int Am, int An, 
			     int Bm, int Bn, 
			     int Cm, int Cn, 
			     int Cm_offset, int Cn_offset) {
  for (int n=0;n<Cn;n++)
    for (int m=0;m<Cm;m++) {
      T accum_r = 0;
      T accum_i = 0;
      int iMin, iMax;
      int jMin, jMax;
      iMin = std::max(0,m+Cm_offset-Bm+1);
      iMax = std::min(Am-1,m+Cm_offset);
      jMin = std::max(0,n+Cn_offset-Bn+1);
      jMax = std::min(An-1,n+Cn_offset);
      for (int j=jMin;j<=jMax;j++)
	for (int i=iMin;i<=iMax;i++) {
	  T p_r, p_i;
	  complex_multiply(Ar[i+j*Am],Ai[i+j*Am],
			   Br[(m+Cm_offset-i)+(n+Cn_offset-j)*Bm],
			   Bi[(m+Cm_offset-i)+(n+Cn_offset-j)*Bm],
			   p_r,p_i);
	  accum_r += p_r;
	  accum_i += p_i;
	}
      Cr[m+n*Cm] = accum_r;
      Ci[m+n*Cm] = accum_i;
    }
}

template <typename T>
static Array Conv2FunctionDispatch(Array X, Array Y, int Cm, int Cn, 
				   int Cm_offset, int Cn_offset, DataClass dclass) {
  if (X.allReal() && Y.allReal()) {
    Array C(dclass,NTuple(Cm,Cn));
    Conv2MainReal<T>(C.real<T>().data(),X.constReal<T>().constData(),Y.constReal<T>().constData(),
		     int(X.rows()),int(X.cols()),int(Y.rows()),int(Y.cols()),Cm,Cn,Cm_offset,Cn_offset);
    return C;
  } else {
    Array C(dclass,NTuple(Cm,Cn));
    Conv2MainComplex<T>(C.real<T>().data(),C.imag<T>().data(),
			X.constReal<T>().constData(),X.constImag<T>().constData(),
			Y.constReal<T>().constData(),Y.constImag<T>().constData(),
			int(X.rows()),int(X.cols()),int(Y.rows()),int(Y.cols()),Cm,Cn,Cm_offset,Cn_offset);
    return C;
  }
}

#define MacroConv(ctype,cls) \
  case cls: return Conv2FunctionDispatch<ctype>(X,Y,Cm,Cn,Cm_offset,Cn_offset,cls);

Array Conv2FunctionDispatch(Array X, Array Y, int Cm, int Cn,
			    int Cm_offset, int Cn_offset) {
  switch (X.dataClass()) {
  default: throw Exception("illegal argument type to conv2");
    MacroConv(float,Float);
    MacroConv(double,Double);
  }
}

#undef MacroConv

static ArrayVector Conv2FunctionFullXY(Array X, Array Y) {
  int Cm, Cn, Cm_offset, Cn_offset;
  Cm = int(X.rows() + Y.rows() - 1);
  Cn = int(X.cols() + Y.cols() - 1);
  Cm_offset = 0;
  Cn_offset = 0;
  return ArrayVector(Conv2FunctionDispatch(X,Y,Cm,Cn,Cm_offset,Cn_offset));
}

static ArrayVector Conv2FunctionSameXY(Array X, Array Y) {
  int Cm, Cn, Cm_offset, Cn_offset;
  Cm = int(X.rows());
  Cn = int(X.cols());
  Cm_offset = (int) floor((double)((Y.rows()-1)/2));
  Cn_offset = (int) floor((double)((Y.cols()-1)/2));
  return ArrayVector(Conv2FunctionDispatch(X,Y,Cm,Cn,Cm_offset,Cn_offset));
}

static ArrayVector Conv2FunctionValidXY(Array X, Array Y) {
  int Cm, Cn, Cm_offset, Cn_offset;
  Cm = int(X.rows()-Y.rows()+1);
  Cn = int(X.cols()-Y.cols()+1);
  if ((Cm <= 0) || (Cn <= 0))
    return ArrayVector(EmptyConstructor());
  Cm_offset = int(Y.rows()-1);
  Cn_offset = int(Y.cols()-1);
  return ArrayVector(Conv2FunctionDispatch(X,Y,Cm,Cn,Cm_offset,Cn_offset));    
}

static ArrayVector Conv2FunctionXY(Array X, Array Y, QString type) {
  // Check the arguments
  if (X.isReferenceType() || Y.isReferenceType())
    throw Exception("cannot apply conv2 to reference types.");
  if (!X.is2D() || !Y.is2D())
    throw Exception("arguments must be matrices, not n-dimensional arrays.");
  if (type == "FULL")
    return Conv2FunctionFullXY(X,Y);
  if (type == "SAME")
    return Conv2FunctionSameXY(X,Y);
  if (type == "VALID")
    return Conv2FunctionValidXY(X,Y);
  throw Exception("could not recognize the arguments to conv2");
}

static ArrayVector Conv2FunctionRCX(Array hcol, Array hrow, Array X, QString type) {
  if (hcol.isReferenceType() || hrow.isReferenceType() ||
      X.isReferenceType())
    throw Exception("cannot apply conv2 to reference types.");
  if (!hcol.is2D() || !hrow.is2D() || !X.is2D())
    throw Exception("arguments must be matrices, not n-dimensional arrays.");
  hcol.reshape(NTuple(hcol.length(),1));
  hrow.reshape(NTuple(1,hrow.length()));
  ArrayVector rvec;
  rvec = Conv2FunctionXY(X,hcol,type);
  rvec = Conv2FunctionXY(rvec.back(),hrow,type);
  return rvec;
}

//!
//@Module CONV2 Matrix Convolution
//@@Section SIGNAL
//@@Usage
//The @|conv2| function performs a two-dimensional convolution of
//matrix arguments.  The syntax for its use is
//@[
//    Z = conv2(X,Y)
//@]
//which performs the full 2-D convolution of @|X| and @|Y|.  If the 
//input matrices are of size @|[xm,xn]| and @|[ym,yn]| respectively,
//then the output is of size @|[xm+ym-1,xn+yn-1]|.  Another form is
//@[
//    Z = conv2(hcol,hrow,X)
//@]
//where @|hcol| and @|hrow| are vectors.  In this form, @|conv2|
//first convolves @|Y| along the columns with @|hcol|, and then 
//convolves @|Y| along the rows with @|hrow|.  This is equivalent
//to @|conv2(hcol(:)*hrow(:)',Y)|.
//
//You can also provide an optional @|shape| argument to @|conv2|
//via either
//@[
//    Z = conv2(X,Y,'shape')
//    Z = conv2(hcol,hrow,X,'shape')
//@]
//where @|shape| is one of the following strings
//\begin{itemize}
//\item @|'full'| - compute the full convolution result - this is the default if no @|shape| argument is provided.
//\item @|'same'| - returns the central part of the result that is the same size as @|X|.
//\item @|'valid'| - returns the portion of the convolution that is computed without the zero-padded edges.  In this situation, @|Z| has 
//size @|[xm-ym+1,xn-yn+1]| when @|xm>=ym| and @|xn>=yn|.  Otherwise
//@|conv2| returns an empty matrix.
//\end{itemize}
//@@Function Internals
//The convolution is computed explicitly using the definition:
//\[
//  Z(m,n) = \sum_{k} \sum_{j} X(k,j) Y(m-k,n-j)
//\]
//If the full output is requested, then @|m| ranges over @|0 <= m < xm+ym-1|
//and @|n| ranges over @|0 <= n < xn+yn-1|.  For the case where @|shape|
//is @|'same'|, the output ranges over @|(ym-1)/2 <= m < xm + (ym-1)/2|
//and @|(yn-1)/2 <= n < xn + (yn-1)/2|.
//@@Signature
//function conv2 Conv2Function
//inputs hcol hrow X shape
//outputs Z
//!
ArrayVector Conv2Function(int nargout, const ArrayVector& arg) {
  // Call the right function based on the arguments
  if (arg.size() < 2) 
    throw Exception("conv2 requires at least 2 arguments");
  Array X(arg[0]);
  Array Y(arg[1]);
  if (((X.dataClass() == Float) && (Y.dataClass() == Float)) ||
      ((X.dataClass() == Float) && (Y.dataClass() == Double)) ||
      ((X.dataClass() == Double) && (Y.dataClass() == Float))) {
    X = X.asDenseArray().toClass(Float);
    Y = Y.asDenseArray().toClass(Float);
  } else {
    X = X.asDenseArray().toClass(Double);
    Y = Y.asDenseArray().toClass(Double);
  }
  if (arg.size() == 2)
    return Conv2FunctionXY(X,Y,"FULL");
  if ((arg.size() == 3) && (arg[2].isString()))
    return Conv2FunctionXY(X,Y,arg[2].asString().toUpper());
  if (arg.size() == 3)
    return Conv2FunctionRCX(X,Y,arg[2],"FULL");
  if ((arg.size() == 4) && (arg[3].isString()))
    return Conv2FunctionRCX(X,Y,arg[2],
			    arg[3].asString().toUpper());
  throw Exception("could not recognize which form of conv2 was requested - check help conv2 for details");
}
