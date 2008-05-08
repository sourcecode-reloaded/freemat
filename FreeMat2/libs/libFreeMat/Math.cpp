/*
 * Copyright (c) 2002-2006 Samit Basu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "Array.hpp"
#include "Exception.hpp"
#include "Math.hpp"
#include "MatrixMultiply.hpp"
#include "LinearEqSolver.hpp"
#include "LeastSquaresSolver.hpp"
#include "EigenDecompose.hpp"
#include "LUDecompose.hpp"
#include "Malloc.hpp"
#include "SparseMatrix.hpp"
#include "Complex.hpp"
#include <math.h>


void boolean_not(int N, logical* C, const logical *A) {
  for (int i=0;i<N;i++)
    C[i] = !A[i];
}

template <class T>
void neg(int N, T* C, const T*A) {
  for (int i=0;i<N;i++)
    C[i] = -A[i];
}


template <typename T>
static inline T powi(T a, int b) {
  T p, x;
  int n;
  unsigned long u;
    
  p = 1;
  x = a;
  n = b;
  
  if(n != 0) {
    if(n < 0) {
      n = -n;
      x = 1/x;
    }
    for(u = n; ; )  {
      if(u & 01)
	p *= x;
      if(u >>= 1)
	x *= x;
      else
	break;
    }
  }
  return(p);
}  

template <typename T>
static inline void powi(const T& ar, const T& ai, int b,
			T& pr, T& pi) {
  int n;
  unsigned long u;
  T t;
  T q[2], x[2];
  static T one[2] = {1.0, 0.0};
  n = b;
  q[0] = 1;
  q[1] = 0;
    
  if(n == 0)
    goto done;
  if(n < 0)
    {
      n = -n;
      complex_divide<T>(x[0],x[1],one[0],one[1],ar,ai);
    }
  else
    {
      x[0] = ar;
      x[1] = ai;
    }
  for(u = n; ; )
    {
      if(u & 01)
	{
	  t = q[0] * x[0] - q[1] * x[1];
	  q[1] = q[0] * x[1] + q[1] * x[0];
	  q[0] = t;
	}
      if(u >>= 1)
	{
	  t = x[0] * x[0] - x[1] * x[1];
	  x[1] = 2 * x[0] * x[1];
	  x[0] = t;
	}
      else
	break;
    }
 done:
  pi = q[1];
  pr = q[0];
}

template <typename T>
static inline void pow(const T& ar, const T& ai,
		       const T& br, const T& bi,
		       T& cr, T& ci) {
  T logr, logi, x, y;
  T mag = complex_abs<T>(ar, ai);
  if (mag == 0) {
    cr = 0;
    ci = 0;
    return;
  }
  logr = log(mag);
  logi = atan2(ai, ar);
  
  x = exp( logr * br - logi * bi );
  y = logr * bi + logi * br;
  
  cr = x * cos(y);
  ci = x * sin(y);
}

struct OpAdd {
  template <typename T>
  static inline T func(const T& v1, const T& v2) {
    return v1+v2;
  }
  template <typename T>
  static inline void func(const T& ar, const T& ai,
			  const T& br, const T& bi,
			  T& cr, T& ci) {
    cr = ar + br;
    ci = ai + bi;
  }
};

struct OpSubtract {
  template <typename T>
  static inline T func(const T& v1, const T& v2) {
    return v1-v2;
  }
  template <typename T>
  static inline void func(const T& ar, const T& ai,
			  const T& br, const T& bi,
			  T& cr, T& ci) {
    cr = ar - br;
    ci = ai - bi;
  }
};

struct OpMultiply {
  template <typename T>
  static inline T func(const T& v1, const T& v2) {
    return v1*v2;
  }
  template <typename T>
  static inline void func(const T& ar, const T& ai,
			  const T& br, const T& bi,
			  T& cr, T& ci) {
    cr = ar * br - ai * bi;
    ci = ar * bi + ai * br;
  }
};

struct OpDivide {
  template <typename T>
  static inline T func(const T& v1, const T& v2) {
    return v1/v2;
  }
  template <typename T>
  static inline void func(const T& ar, const T& ai,
			  const T& br, const T& bi,
			  T& c0, T& c1) {
    complex_divide<T>(ar,ai,br,bi,c0,c1);
  }
};

struct OpLessThan {
  template <typename T>
  static inline bool func(const T& v1, const T& v2) {
    return v1 < v2;
  }
  template <typename T>
  static inline bool func(const T& ar, const T& ai,
			  const T& br, const T& bi) {
    return complex_abs<T>(ar,ai) < complex_abs<T>(br,bi);
  }
};

struct OpLessEquals {
  template <typename T>
  static inline bool func(const T& v1, const T& v2) {
    return v1 <= v2;
  }
  template <typename T>
  static inline bool func(const T& ar, const T& ai,
			  const T& br, const T& bi) {
    return complex_abs<T>(ar,ai) <= complex_abs<T>(br,bi);
  }
};

struct OpEquals {
  template <typename T>
  static inline bool func(const T& v1, const T& v2) {
    return v1 == v2;
  }
  template <typename T>
  static inline bool func(const T& ar, const T& ai,
			  const T& br, const T& bi) {
    return ((ar == br) && (ai == bi));
  }
};

struct OpNotEquals {
  template <typename T>
  static inline bool func(const T& v1, const T& v2) {
    return v1 != v2;
  }
  template <typename T>
  static inline bool func(const T& ar, const T& ai,
			  const T& br, const T& bi) {
    return ((ar != br) || (ai != bi));
  }
};

struct OpGreaterThan {
  template <typename T>
  static inline bool func(const T& v1, const T& v2) {
    return v1 > v2;
  }
  template <typename T>
  static inline bool func(const T& ar, const T& ai,
			  const T& br, const T& bi) {
    return complex_abs<T>(ar,ai) > complex_abs<T>(br,bi);
  }
};

struct OpGreaterEquals {
  template <typename T>
  static inline bool func(const T& v1, const T& v2) {
    return v1 >= v2;
  }
  template <typename T>
  static inline bool func(const T& ar, const T& ai,
			  const T& br, const T& bi) {
    return complex_abs<T>(ar,ai) >= complex_abs<T>(br,bi);
  }
};

struct OpAnd {
  static inline bool func(const bool& v1, const bool& v2) {
    return (v1 && v2);
  }
};

struct OpOr {
  static inline bool func(const bool& v1, const bool& v2) {
    return (v1 || v2);
  }
};

struct OpPower {
  template <typename T>
  static inline T func(const T& v1, const T& v2) {
    if (v2 == int(v2))
      return powi(v1,int(v2));
    else
      return pow(v1,v2);
  }
  template <typename T>
  static inline void func(const T& ar, const T& ai,
			  const T& br, const T& bi,
			  T& cr, T& ci) {
    if ((br == int(br)) && (bi == 0))
      powi(ar,ai,int(br),cr,ci);
    else
      pow(ar,ai,br,bi,cr,ci);
  }
};

// Real, Real --> Real
template <typename S, typename T, class Op>
static inline SparseMatrix<S> DotOp(const SparseMatrix<T>& A, 
				    const SparseMatrix<T>& B) {
  ConstSparseIterator<T> aspin(&A);
  ConstSparseIterator<T> bspin(&B);
  SparseMatrix<S> retval;
  while (aspin.isValid() || bspin.isValid()) {
    while (aspin.moreInSlice() || bspin.moreInSlice()) {
      if (aspin.pos() == bspin.pos()) {
	retval.set(aspin.pos(),Op::func(aspin.value(),bspin.value()));
	aspin.next();
	bspin.next();
      } else if (A.dimensions().map(aspin.pos()) <
		 B.dimensions().map(bspin.pos())) {
	retval.set(aspin.pos(),Op::func(aspin.value(),T(0)));
	aspin.next();
      } else {
	retval.set(bspin.pos(),Op::func(T(0),bspin.value()));
	bspin.next();
      }
    }
    aspin.nextSlice();
    bspin.nextSlice();
  }
  return retval;
}
	
// Complex, Complex --> Complex
template <typename S, typename T, class Op>
static inline void DotOp(const SparseMatrix<T>& A_real, 
			 const SparseMatrix<T>& A_imag,
			 const SparseMatrix<T>& B_real,
			 const SparseMatrix<T>& B_imag,
			 SparseMatrix<S>& C_real,
			 SparseMatrix<S>& C_imag) {
  C_real = SparseMatrix<S>(A_real.dimensions());
  C_imag = SparseMatrix<S>(A_imag.dimensions());
  ConstComplexSparseIterator<T> a_spin(&A_real, &A_imag);
  ConstComplexSparseIterator<T> b_spin(&B_real, &B_imag);
  NTuple dim(A_real.dimensions());
  while (a_spin.isValid() || b_spin.isValid()) {
    T value_c_real, value_c_imag;
    if (a_spin.pos() == b_spin.pos()) {
      Op::func(a_spin.realValue(),a_spin.imagValue(),
	       b_spin.realValue(),b_spin.imagValue(),
	       value_c_real,value_c_imag);
      C_real.set(a_spin.pos(),value_c_real);
      C_imag.set(a_spin.pos(),value_c_imag);
      a_spin.next(); b_spin.next();
    } else if (dim.map(a_spin.pos()) < dim.map(b_spin.pos())) {
      Op::func(a_spin.realValue(),a_spin.imagValue(),0,0,
	       value_c_real,value_c_imag);
      C_real.set(a_spin.pos(),value_c_real);
      C_imag.set(a_spin.pos(),value_c_imag);
      a_spin.next();
    } else {
      Op::func(0,0,b_spin.realValue(),b_spin.imagValue(),
	       value_c_real,value_c_imag);
      C_real.set(b_spin.pos(),value_c_real);
      C_imag.set(b_spin.pos(),value_c_imag);
      b_spin.next();
    }
  }
}
	
// Complex, Complex --> Real
template <typename S, typename T, class Op>
static inline SparseMatrix<S> DotOp(const SparseMatrix<T>& A_real, 
				    const SparseMatrix<T>& A_imag,
				    const SparseMatrix<T>& B_real,
				    const SparseMatrix<T>& B_imag) {
  SparseMatrix<S> C;
  ConstComplexSparseIterator<T> a_spin(&A_real, &A_imag);
  ConstComplexSparseIterator<T> b_spin(&B_real, &B_imag);
  NTuple dim(A_real.dimensions());
  while (a_spin.isValid() || b_spin.isValid()) {
    if (a_spin.pos() == b_spin.pos()) {
      C.set(a_spin.pos(),
	    Op::func(a_spin.realValue(),a_spin.imagValue(),
		     b_spin.realValue(),b_spin.imagValue()));
      a_spin.next(); b_spin.next();
    } else if (dim.map(a_spin.pos()) < dim.map(b_spin.pos())) {
      C.set(a_spin.pos(),Op::func(a_spin.realValue(),a_spin.imagValue(),0,0));
      a_spin.next();
    } else {
      C.set(b_spin.pos(),Op::func(0,0,b_spin.realValue(),b_spin.imagValue()));
      b_spin.next();
    }
  }
}


template <typename T>
class SpinIterator {
  const T &m_value;
public:
  SpinIterator(const T& value) : m_value(value) {}
  inline T get(index_t) const {return m_value;}
};

// Complex,Complex --> Real
template <typename T, typename Atype, typename Btype, class Op>
static inline void DotOp(const Atype& A_real, const Atype& A_imag, 
			 const Btype& B_real, const Btype& B_imag,
			 BasicArray<T> &retvec,
			 const NTuple& dims) {
  retvec = BasicArray<T>(dims);
  for (index_t i=1;i<=dims.count();i++) {
    retvec.set(i,Op::func(A_real.get(i),A_imag.get(i),
			  B_real.get(i),B_imag.get(i)));
  }
  return retvec;  
}

// Real,Real --> Real
template <typename T, typename Atype, typename Btype, class Op>
static inline void DotOp(const Atype& A, const Btype& B, 
			 BasicArray<T> &retvec, const NTuple& dims) {
  retvec = BasicArray<T>(dims);
  for (index_t i=1;i<=dims.count();i++) {
    retvec.set(i,Op::func(A.get(i),B.get(i)));
  }
  return retvec;
}

// Real,Real --> Complex
template <typename T, typename Atype, typename Btype, class Op>
static inline void DotOp(const Atype& A, const Btype& B, 
			 BasicArray<T>& C_real, BasicArray<T>& C_imag,
			 const NTuple& dims) {
  C_real = BasicArray<T>(dims);
  C_imag = BasicArray<T>(dims);
  for (index_t i=1;i<=dims.count();++i) {
    T real, imag;
    Op::func(A.get(i),0,B.get(i),0,real,imag);
    C_real.set(i,real);
    C_imag.set(i,imag);
  }
}

// Complex,Complex --> Complex
template <typename T, typename Atype, typename Btype, class Op>
static inline void DotOp(const Atype& A_real, const Atype& A_imag,
			 const Btype& B_real, const Btype& B_imag,
			 BasicArray<T>& C_real, BasicArray<T>& C_imag, 
			 const NTuple& dims) {
  C_real = BasicArray<T>(dims);
  C_imag = BasicArray<T>(dims);
  for (index_t i=1;i<=dims.count();++i) {
    T real, imag;
    Op::func(A_real.get(i),A_imag.get(i),B_real.get(i),B_imag.get(i),real,imag);
    C_real.set(i,real);
    C_imag.set(i,imag);
  }
}

// Perform the operation via a typed intermediary
template <typename T, class Op>
static inline Array DotOp(const Array &Ain, const Array &Bin, DataClass Tclass) {
  Array Acast(Ain.toClass(Tclass));
  Array Bcast(Bin.toClass(Tclass));
  Array F(Tclass);
  if (Acast.isSparse() && Bcast.isSparse()) {
    if (Acast.dimensions() != Bcast.dimensions())
      throw Exception("size mismatch in arguments to binary operator");
    if (Bcast.allReal() && Acast.allReal()) {
      F = DotOp<T,T,Op>(Acast.constRealSparse<T>(),
			Bcast.constRealSparse<T>());
    } else {
      DotOp<T,T,Op>(Acast.constRealSparse<T>(),
		    Acast.constImagSparse<T>(),
		    Bcast.constRealSparse<T>(),
		    Bcast.constImagSparse<T>(),
		    F.realSparse<T>(),
		    F.imagSparse<T>());
    }
    return F;
  }
  if (!Acast.isScalar()) Acast = Acast.asDenseArray();
  if (!Bcast.isScalar()) Bcast = Bcast.asDenseArray();
  if (Acast.isScalar() && Bcast.isScalar()) {
    if (Acast.allReal() && Bcast.allReal()) {
      F = Array::Array(Op::func(Acast.constRealScalar<T>(),
				Bcast.constRealScalar<T>()));
    } else {
      F = Array::Array(T(0),T(0));
      Op::func(Acast.constRealScalar<T>(),
	       Acast.constImagScalar<T>(),
	       Bcast.constRealScalar<T>(),
	       Bcast.constImagScalar<T>(),
	       F.realScalar<T>(),F.imagScalar<T>());
    }
  } else if (Acast.isScalar()) {
    if (Acast.allReal() && Bcast.allReal()) {
      DotOp<T,SpinIterator<T>,BasicArray<T>,Op>
	(SpinIterator<T>(Acast.constRealScalar<T>()),
	 Bcast.constReal<T>(),
	 F.real<T>(),
	 Bcast.dimensions());
    } else {
      DotOp<T,SpinIterator<T>,BasicArray<T>,Op>
	(SpinIterator<T>(Acast.constRealScalar<T>()),
	 SpinIterator<T>(Acast.constImagScalar<T>()),
	 Bcast.constReal<T>(),
	 Bcast.constImag<T>(),
	 F.real<T>(),
	 F.imag<T>(),
	 Bcast.dimensions());
    }
  } else if (Bcast.isScalar()) {
    if (Bcast.allReal() && Acast.allReal()) {
      DotOp<T,BasicArray<T>,SpinIterator<T>,Op>
	(Acast.constReal<T>(),
	 SpinIterator<T>(Bcast.constRealScalar<T>()),
	 F.real<T>(),
	 Acast.dimensions());
    } else {
      DotOp<T,BasicArray<T>,SpinIterator<T>,Op>
	(Acast.constReal<T>(),
	 Acast.constImag<T>(),
	 SpinIterator<T>(Bcast.constRealScalar<T>()),
	 SpinIterator<T>(Bcast.constImagScalar<T>()),
	 F.real<T>(),
	 F.imag<T>(),
	 Acast.dimensions());
    }
  } else {
    if (Acast.dimensions() != Bcast.dimensions())
      throw Exception("size mismatch in arguments to binary operator");
    if (Bcast.allReal() && Acast.allReal()) {
      DotOp<T,BasicArray<T>,BasicArray<T>,Op>
	(Acast.constReal<T>(),
	 Bcast.constReal<T>(),
	 F.real<T>(),
	 Acast.dimensions());
    } else {
      DotOp<T,BasicArray<T>,BasicArray<T>,Op>
	(Acast.constReal<T>(),
	 Acast.constImag<T>(),
	 Bcast.constReal<T>(),
	 Bcast.constImag<T>(),
	 F.real<T>(),
	 F.imag<T>(),
	 Acast.dimensions());
    }
  }
  return F;
}

// Assumes that the operation cannot create complex values from real ones
template <class Op>
static inline Array DotOp(const Array &Ain, const Array &Bin) {
  if ((Ain.dataClass() != Bin.dataClass()) && 
      (!Ain.isDouble() && !Bin.isDouble()))
    throw Exception("Unsupported type combinations to binary operator");
  DataClass out_type;
  Array F;
  if (Ain.isDouble()) 
    out_type = Bin.dataClass();
  else
    out_type = Ain.dataClass();
  if (out_type == Float)
    F = DotOp<float,Op>(Ain,Bin,Float);
  else
    F = DotOp<double,Op>(Ain,Bin,Double);
  return F.toClass(out_type);
}


template <typename T, class Op>
static inline Array CmpOp(const Array &Ain, const Array &Bin, DataClass Tclass) {
  Array Acast(Ain.toClass(Tclass));
  Array Bcast(Bin.toClass(Tclass));
  Array F(Bool);
  if (Acast.isSparse() && Bcast.isSparse()) {
    if (Acast.dimensions() != Bcast.dimensions())
      throw Exception("size mismatch in arguments to binary operator");
    if (Bcast.allReal() && Acast.allReal()) {
      F = DotOp<bool,T,Op>(Acast.constRealSparse<T>(),
			   Bcast.constRealSparse<T>());
    } else {
      DotOp<bool,T,Op>(Acast.constRealSparse<T>(),
		       Acast.constImagSparse<T>(),
		       Bcast.constRealSparse<T>(),
		       Bcast.constImagSparse<T>(),
		       F.realSparse<bool>(),
		       F.imagSparse<bool>());
    }
    return F;
  }
  if (!Acast.isScalar()) Acast = Acast.asDenseArray();
  if (!Bcast.isScalar()) Bcast = Bcast.asDenseArray();
  if (Acast.isScalar() && Bcast.isScalar()) {
    if (Acast.allReal() && Bcast.allReal()) {
      F = Array::Array(Op::func(Acast.constRealScalar<T>(),
				Bcast.constRealScalar<T>()));
    } else {
      F = Array::Array(Op::func(Acast.constRealScalar<T>(),
				Acast.constImagScalar<T>(),
				Bcast.constRealScalar<T>(),
				Bcast.constImagScalar<T>()));
    }
  } else if (Acast.isScalar()) {
    if (Acast.allReal() && Bcast.allReal()) {
      DotOp<bool,SpinIterator<T>,BasicArray<T>,Op>
	(SpinIterator<T>(Acast.constRealScalar<T>()),
	 Bcast.constReal<T>(),F.real<bool>(),
	 Bcast.dimensions());
    } else {
      DotOp<bool,SpinIterator<T>,BasicArray<T>,Op>
	(SpinIterator<T>(Acast.constRealScalar<T>()),
	 SpinIterator<T>(Acast.constImagScalar<T>()),
	 Bcast.constReal<T>(), Bcast.constImag<T>(),
	 F.real<bool>(), Bcast.dimensions());
    }
  } else if (Bcast.isScalar()) {
    if (Bcast.allReal() && Acast.allReal()) {
      DotOp<bool,BasicArray<T>,SpinIterator<T>,Op>
	(Acast.constReal<T>(),
	 SpinIterator<T>(Bcast.constRealScalar<T>()),
	 F.real<bool>(), Acast.dimensions());
    } else {
      DotOp<bool,BasicArray<T>,SpinIterator<T>,Op>
	(Acast.constReal<T>(), Acast.constImag<T>(),
	 SpinIterator<T>(Bcast.constRealScalar<T>()),
	 SpinIterator<T>(Bcast.constImagScalar<T>()),
	 F.real<bool>(), Acast.dimensions());
    }
  } else {
    if (Acast.dimensions() != Bcast.dimensions())
      throw Exception("size mismatch in arguments to binary operator");
    if (Bcast.allReal() && Acast.allReal()) {
      DotOp<bool,BasicArray<T>,BasicArray<T>,Op>
	(Acast.constReal<T>(), Bcast.constReal<T>(),
	 F.real<bool>(), Acast.dimensions());
    } else {
      DotOp<bool,BasicArray<T>,BasicArray<T>,Op>
	(Acast.constReal<T>(), Acast.constImag<T>(),
	 Bcast.constReal<T>(), Bcast.constImag<T>(),
	 F.real<bool>(), F.imag<T>(), Acast.dimensions());
    }
  }
  return F;
}

template <class Op>
static inline Array CmpOp(const Array &Ain, const Array &Bin) {
  if ((Ain.dataClass() != Bin.dataClass()) && 
      (!Ain.isDouble() && !Bin.isDouble()))
    throw Exception("Unsupported type combinations to binary operator");
  DataClass out_type;
  Array F;
  if (Ain.isDouble()) 
    out_type = Bin.dataClass();
  else
    out_type = Ain.dataClass();
  if (out_type == Float) 
    F = CmpOp<float,Op>(Ain,Bin,Float);
  else
    F = CmpOp<double,Op>(Ain,Bin,Double);
  return F.toClass(Bool);
}

/**
 * Add two objects.
 */
//!
//@Module PLUS Addition Operator
//@@Section OPERATORS
//@@Usage
//Adds two numerical arrays (elementwise) together.  There are two forms
//for its use, both with the same general syntax:
//@[
//  y = a + b
//@]
//where @|a| and @|b| are @|n|-dimensional arrays of numerical type.  In the
//first case, the two arguments are the same size, in which case, the 
//output @|y| is the same size as the inputs, and is the element-wise the sum 
//of @|a| and @|b|.  In the second case, either @|a| or @|b| is a scalar, 
//in which case @|y| is the same size as the larger argument,
//and is the sum of the scalar added to each element of the other argument.
//
//The type of @|y| depends on the types of @|a| and @|b| using the type 
//promotion rules.  The types are ordered as:
//\begin{enumerate}
//\item @|uint8| - unsigned, 8-bit integers range @|[0,255]|
//\item @|int8| - signed, 8-bit integers @|[-127,128]|
//\item @|uint16| - unsigned, 16-bit integers @|[0,65535]|
//\item @|int16| - signed, 16-bit integers @|[-32768,32767]|
//\item @|uint32| - unsigned, 32-bit integers @|[0,4294967295]|
//\item @|int32| - signed, 32-bit integers @|[-2147483648,2147483647]|
//\item @|float| - 32-bit floating point
//\item @|double| - 64-bit floating point
//\item @|complex| - 32-bit complex floating point
//\item @|dcomplex| - 64-bit complex floating point
//\end{enumerate}
//Note that the type promotion and combination rules work similar to 
//@|C|.  Numerical overflow rules are also the same as @|C|.
//@@Function Internals
//There are three formulae for the addition operator, depending on the
//sizes of the three arguments.  In the most general case, in which 
//the two arguments are the same size, the output is computed via:
//\[
//y(m_1,\ldots,m_d) = a(m_1,\ldots,m_d) + b(m_1,\ldots,m_d)
//\]
//If @|a| is a scalar, then the output is computed via
//\[
//y(m_1,\ldots,m_d) = a + b(m_1,\ldots,m_d).
//\]
//On the other hand, if @|b| is a scalar, then the output is computed via
//\[
//y(m_1,\ldots,m_d) = a(m_1,\ldots,m_d) + b.
//\]
//@@Examples
//Here are some examples of using the addition operator.  First, a 
//straight-forward usage of the plus operator.  The first example
//is straightforward - the @|int32| is the default type used for
//integer constants (same as in @|C|), hence the output is the
//same type:
//@<
//3 + 8
//@>
//Next, we use the floating point syntax to force one of the arguments
//to be a @|double|, which results in the output being @|double|:
//@<
//3.1 + 2
//@>
//Note that if one of the arguments is complex-valued, the output will be
//complex also.
//@<
//a = 3 + 4*i
//b = a + 2.0f
//@>
//If a @|complex| value is added to a @|double|, the result is 
//promoted to @|dcomplex|.
//@<
//b = a + 2.0
//@>
//We can also demonstrate the three forms of the addition operator.  First
//the element-wise version:
//@<
//a = [1,2;3,4]
//b = [2,3;6,7]
//c = a + b
//@>
//Then the scalar versions
//@<
//c = a + 1
//c = 1 + b
//@>
//@@Tests
//@{ test_sparse37.m
//function x = test_sparse37
//a = [0 0 2 0 3 0
//     1 0 2 0 0 3
//     1 0 4 5 0 3
//     0 0 0 2 0 0];
//b = [1 0 0 0 0 0
//     0 1 0 0 0 0 
//     0 0 1 0 0 0 
//     0 0 0 1 0 0];
//A = sparse(a);
//B = sparse(b);
//C = A + B;
//c = a + b;
//x = testeq(c,C);
//@}
//@{ test_sparse38.m
//function x = test_sparse38
//ar = [0 0 2 0 3 0
//      1 0 2 0 0 3
//      1 0 4 5 0 3
//      0 0 0 2 0 0];
//
//ai = [1 0 1 2 0 0
//      1 0 0 3 0 0
//      0 0 2 0 0 4
//      0 0 0 3 0 0];
//
//br = [1 0 0 0 0 0
//      0 1 0 0 0 0 
//      0 0 1 0 0 0 
//      0 0 0 1 0 0];
//
//bi = [0 0 2 0 3 0
//      1 0 2 0 0 3
//      1 0 4 5 0 3
//      0 0 0 2 0 0];
//
//a = ar+i*ai;
//b = br+i*bi;
//
//A = sparse(a);
//B = sparse(b);
//
//C = A + B;
//c = a + b;
//x = testeq(c,C);
//@}
//@{ test_sparse78.m
//% Test sparse-sparse matrix array add
//function x = test_sparse78
//[yi1,zi1] = sparse_test_mat('int32',300,400);
//[yf1,zf1] = sparse_test_mat('float',300,400);
//[yd1,zd1] = sparse_test_mat('double',300,400);
//[yc1,zc1] = sparse_test_mat('complex',300,400);
//[yz1,zz1] = sparse_test_mat('dcomplex',300,400);
//[yi2,zi2] = sparse_test_mat('int32',300,400);
//[yf2,zf2] = sparse_test_mat('float',300,400);
//[yd2,zd2] = sparse_test_mat('double',300,400);
//[yc2,zc2] = sparse_test_mat('complex',300,400);
//[yz2,zz2] = sparse_test_mat('dcomplex',300,400);
//x = testeq(yi1+yi2,zi1+zi2) & testeq(yf1+yf2,zf1+zf2) & testeq(yd1+yd2,zd1+zd2) & testeq(yc1+yc2,zc1+zc2) & testeq(yz1+yz2,zz1+zz2);
//@}
//@@Tests
//@$"y=zeros(3,0,4)+zeros(3,0,4)","zeros(3,0,4)","exact"
//!
Array Add(const Array& A, const Array& B) {
  return DotOp<OpAdd>(A,B);
}


/**
 * Subtract two objects.
 */
//!
//@Module MINUS Subtraction Operator
//@@Section OPERATORS
//@@Usage
//Subtracts two numerical arrays (elementwise).  There are two forms
//for its use, both with the same general syntax:
//@[
//  y = a - b
//@]
//where @|a| and @|b| are @|n|-dimensional arrays of numerical type.  In the
//first case, the two arguments are the same size, in which case, the 
//output @|y| is the same size as the inputs, and is the element-wise
//difference of @|a| and @|b|.  In the second case, either @|a| or @|b| is a scalar, 
//in which case @|y| is the same size as the larger argument,
//and is the difference of the scalar to each element of the other argument.
//
//The type of @|y| depends on the types of @|a| and @|b| using the type 
//promotion rules.  The types are ordered as:
//\begin{enumerate}
//\item @|uint8| - unsigned, 8-bit integers range @|[0,255]|
//\item @|int8| - signed, 8-bit integers @|[-127,128]|
//\item @|uint16| - unsigned, 16-bit integers @|[0,65535]|
//\item @|int16| - signed, 16-bit integers @|[-32768,32767]|
//\item @|uint32| - unsigned, 32-bit integers @|[0,4294967295]|
//\item @|int32| - signed, 32-bit integers @|[-2147483648,2147483647]|
//\item @|float| - 32-bit floating point
//\item @|double| - 64-bit floating point
//\item @|complex| - 32-bit complex floating point
//\item @|dcomplex| - 64-bit complex floating point
//\end{enumerate}
//Note that the type promotion and combination rules work similar to 
//@|C|.  Numerical overflow rules are also the same as @|C|.
//@@Function Internals
//There are three formulae for the subtraction operator, depending on the
//sizes of the three arguments.  In the most general case, in which 
//the two arguments are the same size, the output is computed via:
//\[
//y(m_1,\ldots,m_d) = a(m_1,\ldots,m_d) - b(m_1,\ldots,m_d)
//\]
//If @|a| is a scalar, then the output is computed via
//\[
//y(m_1,\ldots,m_d) = a - b(m_1,\ldots,m_d).
//\]
//On the other hand, if @|b| is a scalar, then the output is computed via
//\[
//y(m_1,\ldots,m_d) = a(m_1,\ldots,m_d) - b.
//\]
//@@Examples
//Here are some examples of using the subtraction operator.  First, a 
//straight-forward usage of the minus operator.  The first example
//is straightforward - the @|int32| is the default type used for
//integer constants (same as in @|C|), hence the output is the
//same type:
//@<
//3 - 8
//@>
//Next, we use the floating point syntax to force one of the arguments
//to be a @|double|, which results in the output being @|double|:
//@<
//3.1 - 2
//@>
//Note that if one of the arguments is complex-valued, the output will be
//complex also.
//@<
//a = 3 + 4*i
//b = a - 2.0f
//@>
//If a @|double| value is subtracted from a @|complex|, the result is 
//promoted to @|dcomplex|.
//@<
//b = a - 2.0
//@>
//We can also demonstrate the three forms of the subtraction operator.  First
//the element-wise version:
//@<
//a = [1,2;3,4]
//b = [2,3;6,7]
//c = a - b
//@>
//Then the scalar versions
//@<
//c = a - 1
//c = 1 - b
//@>
//@@Tests
//@{ test_sparse39.m
//function x = test_sparse39
//a = [0 0 2 0 3 0
//     1 0 2 0 0 3
//     1 0 4 5 0 3
//     0 0 0 2 0 0];
//
//b = [1 0 0 0 0 0
//     0 1 0 0 0 0 
//     0 0 1 0 0 0 
//       0 0 0 1 0 0];
//
//A = sparse(a);
//B = sparse(b);
//
//C = A - B;
//c = a - b;
//x = testeq(c,C);
//@}
//@{ test_sparse40.m
//function x = test_sparse40
//ar = [0 0 2 0 3 0
//      1 0 2 0 0 3
//      1 0 4 5 0 3
//      0 0 0 2 0 0];
//
//ai = [1 0 1 2 0 0
//      1 0 0 3 0 0
//      0 0 2 0 0 4
//      0 0 0 3 0 0];
//
//br = [1 0 0 0 0 0
//      0 1 0 0 0 0 
//      0 0 1 0 0 0 
//      0 0 0 1 0 0];
//
//bi = [0 0 2 0 3 0
//      1 0 2 0 0 3
//      1 0 4 5 0 3
//      0 0 0 2 0 0];
//
//a = ar+i*ai;
//b = br+i*bi;
//
//A = sparse(a);
//B = sparse(b);
//
//C = A - B;
//c = a - b;
//x = testeq(c,C);
//@}
//@{ test_sparse79.m
//% Test sparse-sparse matrix array subtract
//function x = test_sparse79
//[yi1,zi1] = sparse_test_mat('int32',300,400);
//[yf1,zf1] = sparse_test_mat('float',300,400);
//[yd1,zd1] = sparse_test_mat('double',300,400);
//[yc1,zc1] = sparse_test_mat('complex',300,400);
//[yz1,zz1] = sparse_test_mat('dcomplex',300,400);
//[yi2,zi2] = sparse_test_mat('int32',300,400);
//[yf2,zf2] = sparse_test_mat('float',300,400);
//[yd2,zd2] = sparse_test_mat('double',300,400);
//[yc2,zc2] = sparse_test_mat('complex',300,400);
//[yz2,zz2] = sparse_test_mat('dcomplex',300,400);
//x = testeq(yi1-yi2,zi1-zi2) & testeq(yf1-yf2,zf1-zf2) & testeq(yd1-yd2,zd1-zd2) & testeq(yc1-yc2,zc1-zc2) & testeq(yz1-yz2,zz1-zz2);
//@}
//!
Array Subtract(const Array& A, const Array &B) {
  return DotOp<OpSubtract>(A,B);
}

/**
 * Element-wise multiplication.
 */
//!
//@Module DOTTIMES Element-wise Multiplication Operator
//@@Section OPERATORS
//@@Usage
//Multiplies two numerical arrays (elementwise).  There are two forms
//for its use, both with the same general syntax:
//@[
//  y = a .* b
//@]
//where @|a| and @|b| are @|n|-dimensional arrays of numerical type.  In the
//first case, the two arguments are the same size, in which case, the 
//output @|y| is the same size as the inputs, and is the element-wise
//product of @|a| and @|b|.  In the second case, either @|a| or @|b| is a scalar, 
//in which case @|y| is the same size as the larger argument,
//and is the product of the scalar with each element of the other argument.
//
//The type of @|y| depends on the types of @|a| and @|b| using type 
//promotion rules. All of the types are preserved under multiplication except
// for integer types, which are promoted to @|int32| prior to 
//multiplication (same as @|C|).
//@@Function Internals
//There are three formulae for the dot-times operator, depending on the
//sizes of the three arguments.  In the most general case, in which 
//the two arguments are the same size, the output is computed via:
//\[
//y(m_1,\ldots,m_d) = a(m_1,\ldots,m_d) \times b(m_1,\ldots,m_d)
//\]
//If @|a| is a scalar, then the output is computed via
//\[
//y(m_1,\ldots,m_d) = a \times b(m_1,\ldots,m_d).
//\]
//On the other hand, if @|b| is a scalar, then the output is computed via
//\[
//y(m_1,\ldots,m_d) = a(m_1,\ldots,m_d) \times b.
//\]
//@@Examples
//Here are some examples of using the dottimes operator.  First, a 
//straight-forward usage of the @|.*| operator.  The first example
//is straightforward:
//@<
//3 .* 8
//@>
//Note, however, that because of the way that input is parsed, eliminating
//the spaces @|3.*8| results in the input being parsed as @|3. * 8|,
//which yields a @|double| result:
//@<
//3.*8
//@>
//This is really an invokation of the @|times| operator.
//
//Next, we use the floating point syntax to force one of the arguments
//to be a @|double|, which results in the output being @|double|:
//@<
//3.1 .* 2
//@>
//Note that if one of the arguments is complex-valued, the output will be
//complex also.
//@<
//a = 3 + 4*i
//b = a .* 2.0f
//@>
//If a @|complex| value is multiplied by a @|double|, the result is 
//promoted to @|dcomplex|.
//@<
//b = a .* 2.0
//@>
//We can also demonstrate the three forms of the dottimes operator.  First
//the element-wise version:
//@<
//a = [1,2;3,4]
//b = [2,3;6,7]
//c = a .* b
//@>
//Then the scalar versions
//@<
//c = a .* 3
//c = 3 .* a
//@>
//@@Tests
//@{ test_sparse41.m
//function x = test_sparse41
//a = [0 0 2 0 3 0
//     1 0 2 0 0 3
//     1 0 4 5 0 3
//     0 0 0 2 0 0];
//
//b = [1 0 0 0 0 0
//     0 1 0 0 0 0 
//     0 0 1 0 0 0 
//       0 0 0 1 0 0];
//
//A = sparse(a);
//B = sparse(b);
//
//C = A .* B;
//c = a .* b;
//x = testeq(c,C);
//@}
//@{ test_sparse42.m
//function x = test_sparse42
//ar = [0 0 2 0 3 0
//      1 0 2 0 0 3
//      1 0 4 5 0 3
//      0 0 0 2 0 0];
//
//ai = [1 0 1 2 0 0
//      1 0 0 3 0 0
//      0 0 2 0 0 4
//      0 0 0 3 0 0];
//
//br = [1 0 0 0 0 0
//      0 1 0 0 0 0 
//      0 0 1 0 0 0 
//      0 0 0 1 0 0];
//
//bi = [0 0 2 0 3 0
//      1 0 2 0 0 3
//      1 0 4 5 0 3
//      0 0 0 2 0 0];
//
//a = ar+i*ai;
//b = br+i*bi;
//
//A = sparse(a);
//B = sparse(b);
//
//C = A .* B;
//c = a .* b;
//x = testeq(c,C);
//@}
//@{ test_sparse43.m
//function x = test_sparse43
//a = [0 0 2 0 3 0
//     1 0 2 0 0 3
//     1 0 4 5 0 3
//     0 0 0 2 0 0];
//
//b = pi;
//
//A = sparse(a);
//
//C = A .* b;
//c = a .* b;
//x = testeq(c,C);
//@}
//@{ test_sparse44.m
//function x = test_sparse44
//ar = [0 0 2 0 3 0
//      1 0 2 0 0 3
//      1 0 4 5 0 3
//      0 0 0 2 0 0];
//
//ai = [1 0 1 2 0 0
//      1 0 0 3 0 0
//      0 0 2 0 0 4
//      0 0 0 3 0 0];
//
//a = ar + i * ai;
//b = 3 + 4*i;
//
//A = sparse(a);
//
//C = A .* b;
//c = a .* b;
//x = testeq(c,C);
//@}
//!
Array DotMultiply(const Array& A, const Array &B) {
  return DotOp<OpMultiply>(A,B);
}
 
/**
 * Element-wise right divide.
 */
//!
//@Module DOTRIGHTDIVIDE Element-wise Right-Division Operator
//@@Section OPERATORS
//@@Usage
//Divides two numerical arrays (elementwise).  There are two forms
//for its use, both with the same general syntax:
//@[
//  y = a ./ b
//@]
//where @|a| and @|b| are @|n|-dimensional arrays of numerical type.  In the
//first case, the two arguments are the same size, in which case, the 
//output @|y| is the same size as the inputs, and is the element-wise
//division of @|b| by @|a|.  In the second case, either @|a| or @|b| is a scalar, 
//in which case @|y| is the same size as the larger argument,
//and is the division of the scalar with each element of the other argument.
//
//The type of @|y| depends on the types of @|a| and @|b| using type 
//promotion rules, with one important exception: unlike @|C|, integer
//types are promoted to @|double| prior to division.
//@@Function Internals
//There are three formulae for the dot-right-divide operator, depending on the
//sizes of the three arguments.  In the most general case, in which 
//the two arguments are the same size, the output is computed via:
//\[
//y(m_1,\ldots,m_d) = \frac{a(m_1,\ldots,m_d)}{b(m_1,\ldots,m_d)}
//\]
//If @|a| is a scalar, then the output is computed via
//\[
//y(m_1,\ldots,m_d) = \frac{a}{b(m_1,\ldots,m_d)}
//\]
//On the other hand, if @|b| is a scalar, then the output is computed via
//\[
//y(m_1,\ldots,m_d) = \frac{a(m_1,\ldots,m_d)}{b}.
//\]
//@@Examples
//Here are some examples of using the dot-right-divide operator.  First, a 
//straight-forward usage of the @|./| operator.  The first example
//is straightforward:
//@<
//3 ./ 8
//@>
//Note that this is not the same as evaluating @|3/8| in @|C| - there,
//the output would be @|0|, the result of the integer division.
//
//We can also divide complex arguments:
//@<
//a = 3 + 4*i
//b = 5 + 8*i
//c = a ./ b
//@>
//If a @|complex| value is divided by a @|double|, the result is 
//promoted to @|dcomplex|.
//@<
//b = a ./ 2.0
//@>
//We can also demonstrate the three forms of the dot-right-divide operator.  First
//the element-wise version:
//@<
//a = [1,2;3,4]
//b = [2,3;6,7]
//c = a ./ b
//@>
//Then the scalar versions
//@<
//c = a ./ 3
//c = 3 ./ a
//@>
//!
Array DotRightDivide(const Array& A, const Array& B) {
  return DotOp<OpDivide>(A,B);
}

/**
 * Element-wise left divide.
 */
//!
//@Module DOTLEFTDIVIDE Element-wise Left-Division Operator
//@@Section OPERATORS
//@@Usage
//Divides two numerical arrays (elementwise) - gets its name from the 
//fact that the divisor is on the left.  There are two forms
//for its use, both with the same general syntax:
//@[
//  y = a .\ b
//@]
//where @|a| and @|b| are @|n|-dimensional arrays of numerical type.  In the
//first case, the two arguments are the same size, in which case, the 
//output @|y| is the same size as the inputs, and is the element-wise
//division of @|b| by @|a|.  In the second case, either @|a| or @|b| is a scalar, 
//in which case @|y| is the same size as the larger argument,
//and is the division of the scalar with each element of the other argument.
//
//The type of @|y| depends on the types of @|a| and @|b| using type 
//promotion rules, with one important exception: unlike @|C|, integer
//types are promoted to @|double| prior to division.
//@@Function Internals
//There are three formulae for the dot-left-divide operator, depending on the
//sizes of the three arguments.  In the most general case, in which 
//the two arguments are the same size, the output is computed via:
//\[
//y(m_1,\ldots,m_d) = \frac{b(m_1,\ldots,m_d)}{a(m_1,\ldots,m_d)}
//\]
//If @|a| is a scalar, then the output is computed via
//\[
//y(m_1,\ldots,m_d) = \frac{b(m_1,\ldots,m_d)}{a}
//\]
//On the other hand, if @|b| is a scalar, then the output is computed via
//\[
//y(m_1,\ldots,m_d) = \frac{b}{a(m_1,\ldots,m_d)}.
//\]
//@@Examples
//Here are some examples of using the dot-left-divide operator.  First, a 
//straight-forward usage of the @|.\\| operator.  The first example
//is straightforward:
//@<
//3 .\ 8
//@>
//Note that this is not the same as evaluating @|8/3| in @|C| - there,
//the output would be @|2|, the result of the integer division.
//
//We can also divide complex arguments:
//@<
//a = 3 + 4*i
//b = 5 + 8*i
//c = b .\ a
//@>
//If a @|complex| value is divided by a @|double|, the result is 
//promoted to @|dcomplex|.
//@<
//b = a .\ 2.0
//@>
//We can also demonstrate the three forms of the dot-left-divide operator.  First
//the element-wise version:
//@<
//a = [1,2;3,4]
//b = [2,3;6,7]
//c = a .\ b
//@>
//Then the scalar versions
//@<
//c = a .\ 3
//c = 3 .\ a
//@>
//!
Array DotLeftDivide(const Array& A, const Array& B) {
  return DotOp<OpDivide>(B,A);
}

/**
 * Element-wise power.
 */
//!
//@Module DOTPOWER Element-wise Power Operator
//@@Section OPERATORS
//@@Usage
//Raises one numerical array to another array (elementwise).  There are three operators all with the same general syntax:
//@[
//  y = a .^ b
//@]
//The result @|y| depends on which of the following three situations applies to the arguments @|a| and @|b|:
//\begin{enumerate}
//  \item @|a| is a scalar, @|b| is an arbitrary @|n|-dimensional numerical array, in which case the output is @|a| raised to the power of each element of @|b|, and the output is the same size as @|b|.
//  \item @|a| is an @|n|-dimensional numerical array, and @|b| is a scalar, then the output is the same size as @|a|, and is defined by each element of @|a| raised to the power @|b|.
//  \item @|a| and @|b| are both @|n|-dimensional numerical arrays of \emph{the same size}.  In this case, each element of the output is the corresponding element of @|a| raised to the power defined by the corresponding element of @|b|.
//\end{enumerate}
//The output follows the standard type promotion rules, although types are not generally preserved under the power operation.  In particular, integers are automatically converted to @|double| type, and negative numbers raised to fractional powers can return complex values.
//@@Function Internals
//There are three formulae for this operator.  For the first form
//\[
//y(m_1,\ldots,m_d) = a^{b(m_1,\ldots,m_d)},
//\]
//and the second form
//\[
//y(m_1,\ldots,m_d) = a(m_1,\ldots,m_d)^b,
//\]
//and in the third form
//\[
//y(m_1,\ldots,m_d) = a(m_1,\ldots,m_d)^{b(m_1,\ldots,m_d)}.
//\]
//@@Examples
//We demonstrate the three forms of the dot-power operator using some simple examples.  First, the case of a scalar raised to a series of values.
//@<
//a = 2
//b = 1:4
//c = a.^b
//@>
//The second case shows a vector raised to a scalar.
//@<
//c = b.^a
//@>
//The third case shows the most general use of the dot-power operator.
//@<
//A = [1,2;3,2]
//B = [2,1.5;0.5,0.6]
//C = A.^B
//@>
//@@Tests
//@$"y=2.^[1:5]","[2,4,8,16,32]","exact"
//@$"y=2..^[1:5]","[2,4,8,16,32]","exact"
//@{ test_power1.m
//function x = test_power1
//invD_11 = [0.3529    0.4028    0.5812    0.3333];
//invD_12 = [0.0707   -0.0334   -0.8818    0.0000];
//invD_22 = [0.4942    0.4028    2.3447    0.5000];
//d1_k = [-1.2335   -0.3520   -1.5988   -1.8315];
//d2_k = [1.5265   -1.5861    0.0067    1.0221];
//vec_max = d1_k.^2.0.*invD_11+2*d1_k.*d2_k.*invD_12+d2_k.^2.0.*invD_22;
//[value, pos1] = max(vec_max);
//vec_max = d1_k  .^  2.0   .*invD_11+2*d1_k.*d2_k.*invD_12+d2_k  .^  2.0   .*invD_22;
//[value, pos2] = max(vec_max);
//vec_max = d1_k.^2.*invD_11+2*d1_k.*d2_k.*invD_12+d2_k.^2.*invD_22;
//[value, pos3] = max(vec_max);
//vec_max = d1_k.^2    .* invD_11+2*d1_k.*d2_k.*invD_12+d2_k.^2   .*invD_22;
//[value, pos4] = max(vec_max);
//x = (pos1 == 4) && (pos2 == 4) && (pos3 == 4) && (pos4 == 4);
//@}
//!

// Simplified test -- if A & B are both real
// then if A is negative and B is non-integer,
// then the complex case is used.
Array DotPower(const Array& A, const Array& B) {
  if (A.allReal() && B.allReal() &&
      !IsNonNegative(A) && !IsInteger(B)) {
    return DotOp<OpPower>(A.asComplex(),B.asComplex());
  }
  return DotOp<OpPower>(A,B);
}

/**
 * Element-wise less than.
 */
//!
//@Module COMPARISONOPS Array Comparison Operators
//@@Section OPERATORS
//@@Usage
//There are a total of six comparison operators available in FreeMat, all of which are binary operators with the following syntax
//@[
//  y = a < b
//  y = a <= b
//  y = a > b
//  y = a >= b
//  y = a ~= b
//  y = a == b
//@]
//where @|a| and @|b| are numerical arrays or scalars, and @|y| is a @|logical| array of the appropriate size.  Each of the operators has three modes of operation, summarized in the following list:
//\begin{enumerate}
//  \item @|a| is a scalar, @|b| is an n-dimensional array - the output is then the same size as @|b|, and contains the result of comparing each element in @|b| to the scalar @|a|.
//  \item @|a| is an n-dimensional array, @|b| is a scalar - the output is the same size as @|a|, and contains the result of comparing each element in @|a| to the scalar @|b|.
//  \item @|a| and @|b| are both n-dimensional arrays of the same size - the output is then the same size as both @|a| and @|b|, and contains the result of an element-wise comparison between @|a| and @|b|.
//\end{enumerate}
//The operators behave the same way as in @|C|, with unequal types meing promoted using the standard type promotion rules prior to comparisons.  The only difference is that in FreeMat, the not-equals operator is @|~=| instead of @|!=|.
//@@Examples
//Some simple examples of comparison operations.  First a comparison with a scalar:
//@<
//a = randn(1,5)
//a>0
//@>
//Next, we construct two vectors, and test for equality:
//@<
//a = [1,2,5,7,3]
//b = [2,2,5,9,4]
//c = a == b
//@>
//@@Tests
//@{ test_sparse84.m
//function x = test_sparse84
//A = int32(10*sprandn(10,10,0.3));
//B = int32(10*sprandn(10,10,0.3));
//C = A < B;
//c = full(A) < full(B);
//x = testeq(c,C);
//C = A < -2;
//c = full(A) < -2;
//x = x & testeq(c,C);
//C = 2 < A;
//c = 2 < full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse85.m
//function x = test_sparse85
//A = int32(10*sprandn(10,10,0.3));
//B = int32(10*sprandn(10,10,0.3));
//C = A > B;
//c = full(A) > full(B);
//x = testeq(c,C);
//C = A > -2;
//c = full(A) > -2;
//x = x & testeq(c,C);
//C = 2 > A;
//c = 2 > full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse86.m
//function x = test_sparse86
//A = int32(10*sprandn(10,10,0.3));
//B = int32(10*sprandn(10,10,0.3));
//C = A <= B;
//c = full(A) <= full(B);
//x = testeq(c,C);
//C = A <= -2;
//c = full(A) <= -2;
//x = x & testeq(c,C);
//C = 2 <= A;
//c = 2 <= full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse87.m
//function x = test_sparse87
//A = int32(10*sprandn(10,10,0.3));
//B = int32(10*sprandn(10,10,0.3));
//C = A >= B;
//c = full(A) >= full(B);
//x = testeq(c,C);
//C = A >= -2;
//c = full(A) >= -2;
//x = x & testeq(c,C);
//C = 2 >= A;
//c = 2 >= full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse88.m
//function x = test_sparse88
//A = int32(10*sprandn(10,10,0.3));
//B = int32(10*sprandn(10,10,0.3));
//C = A ~= B;
//c = full(A) ~= full(B);
//x = testeq(c,C);
//C = A ~= -2;
//c = full(A) ~= -2;
//x = x & testeq(c,C);
//C = 2 ~= A;
//c = 2 ~= full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse89.m
//function x = test_sparse89
//A = int32(10*sprandn(10,10,0.3));
//B = int32(10*sprandn(10,10,0.3));
//C = A == B;
//c = full(A) == full(B);
//x = testeq(c,C);
//C = A == -2;
//c = full(A) == -2;
//x = x & testeq(c,C);
//C = 2 == A;
//c = 2 == full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse90.m
//function x = test_sparse90
//A = float(int32(10*sprandn(10,10,0.3)));
//B = float(int32(10*sprandn(10,10,0.3)));
//C = A < B;
//c = full(A) < full(B);
//x = testeq(c,C);
//C = A < -2;
//c = full(A) < -2;
//x = x & testeq(c,C);
//C = 2 < A;
//c = 2 < full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse91.m
//function x = test_sparse91
//A = float(int32(10*sprandn(10,10,0.3)));
//B = float(int32(10*sprandn(10,10,0.3)));
//C = A > B;
//c = full(A) > full(B);
//x = testeq(c,C);
//C = A > -2;
//c = full(A) > -2;
//x = x & testeq(c,C);
//C = 2 > A;
//c = 2 > full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse92.m
//function x = test_sparse92
//A = float(int32(10*sprandn(10,10,0.3)));
//B = float(int32(10*sprandn(10,10,0.3)));
//C = A <= B;
//c = full(A) <= full(B);
//x = testeq(c,C);
//C = A <= -2;
//c = full(A) <= -2;
//x = x & testeq(c,C);
//C = 2 <= A;
//c = 2 <= full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse93.m
//function x = test_sparse93
//A = float(int32(10*sprandn(10,10,0.3)));
//B = float(int32(10*sprandn(10,10,0.3)));
//C = A >= B;
//c = full(A) >= full(B);
//x = testeq(c,C);
//C = A >= -2;
//c = full(A) >= -2;
//x = x & testeq(c,C);
//C = 2 >= A;
//c = 2 >= full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse94.m
//function x = test_sparse94
//A = float(int32(10*sprandn(10,10,0.3)));
//B = float(int32(10*sprandn(10,10,0.3)));
//C = A ~= B;
//c = full(A) ~= full(B);
//x = testeq(c,C);
//C = A ~= -2;
//c = full(A) ~= -2;
//x = x & testeq(c,C);
//C = 2 ~= A;
//c = 2 ~= full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse95.m
//function x = test_sparse95
//A = float(int32(10*sprandn(10,10,0.3)));
//B = float(int32(10*sprandn(10,10,0.3)));
//C = A == B;
//c = full(A) == full(B);
//x = testeq(c,C);
//C = A == -2;
//c = full(A) == -2;
//x = x & testeq(c,C);
//C = 2 == A;
//c = 2 == full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse96.m
//function x = test_sparse96
//A = double(int32(10*sprandn(10,10,0.3)));
//B = double(int32(10*sprandn(10,10,0.3)));
//C = A < B;
//c = full(A) < full(B);
//x = testeq(c,C);
//C = A < -2;
//c = full(A) < -2;
//x = x & testeq(c,C);
//C = 2 < A;
//c = 2 < full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse97.m
//function x = test_sparse97
//A = double(int32(10*sprandn(10,10,0.3)));
//B = double(int32(10*sprandn(10,10,0.3)));
//C = A > B;
//c = full(A) > full(B);
//x = testeq(c,C);
//C = A > -2;
//c = full(A) > -2;
//x = x & testeq(c,C);
//C = 2 > A;
//c = 2 > full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse98.m
//function x = test_sparse98
//A = double(int32(10*sprandn(10,10,0.3)));
//B = double(int32(10*sprandn(10,10,0.3)));
//C = A >= B;
//c = full(A) >= full(B);
//x = testeq(c,C);
//C = A >= -2;
//c = full(A) >= -2;
//x = x & testeq(c,C);
//C = 2 >= A;
//c = 2 >= full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse99.m
//function x = test_sparse99
//A = double(int32(10*sprandn(10,10,0.3)));
//B = double(int32(10*sprandn(10,10,0.3)));
//C = A <= B;
//c = full(A) <= full(B);
//x = testeq(c,C);
//C = A <= -2;
//c = full(A) <= -2;
//x = x & testeq(c,C);
//C = 2 <= A;
//c = 2 <= full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse100.m
//function x = test_sparse100
//A = double(int32(10*sprandn(10,10,0.3)));
//B = double(int32(10*sprandn(10,10,0.3)));
//C = A == B;
//c = full(A) == full(B);
//x = testeq(c,C);
//C = A == -2;
//c = full(A) == -2;
//x = x & testeq(c,C);
//C = 2 == A;
//c = 2 == full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse101.m
//function x = test_sparse101
//A = double(int32(10*sprandn(10,10,0.3)));
//B = double(int32(10*sprandn(10,10,0.3)));
//C = A ~= B;
//c = full(A) ~= full(B);
//x = testeq(c,C);
//C = A ~= -2;
//c = full(A) ~= -2;
//x = x & testeq(c,C);
//C = 2 ~= A;
//c = 2 ~= full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse102.m
//function x = test_sparse102
//A = double(int32(10*sprandn(10,10,0.3)))+i*double(int32(10*sprandn(10,10,0.3)));
//B = double(int32(10*sprandn(10,10,0.3)))+i*double(int32(10*sprandn(10,10,0.3)));
//C = A ~= B;
//c = full(A) ~= full(B);
//x = testeq(c,C);
//C = A ~= -2;
//c = full(A) ~= -2;
//x = x & testeq(c,C);
//C = 2 ~= A;
//c = 2 ~= full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse103.m
//function x = test_sparse103
//A = double(int32(10*sprandn(10,10,0.3)))+i*double(int32(10*sprandn(10,10,0.3)));
//B = double(int32(10*sprandn(10,10,0.3)))+i*double(int32(10*sprandn(10,10,0.3)));
//C = A == B;
//c = full(A) == full(B);
//x = testeq(c,C);
//C = A == -2;
//c = full(A) == -2;
//x = x & testeq(c,C);
//C = 2 == A;
//c = 2 == full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse104.m
//function x = test_sparse104
//A = double(int32(10*sprandn(10,10,0.3)))+i*double(int32(10*sprandn(10,10,0.3)));
//B = double(int32(10*sprandn(10,10,0.3)))+i*double(int32(10*sprandn(10,10,0.3)));
//C = A <= B;
//c = full(A) <= full(B);
//x = testeq(c,C);
//C = A <= -2;
//c = full(A) <= -2;
//x = x & testeq(c,C);
//C = 2 <= A;
//c = 2 <= full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse105.m
//function x = test_sparse105
//A = double(int32(10*sprandn(10,10,0.3)))+i*double(int32(10*sprandn(10,10,0.3)));
//B = double(int32(10*sprandn(10,10,0.3)))+i*double(int32(10*sprandn(10,10,0.3)));
//C = A >= B;
//c = full(A) >= full(B);
//x = testeq(c,C);
//C = A >= -2;
//c = full(A) >= -2;
//x = x & testeq(c,C);
//C = 2 >= A;
//c = 2 >= full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse106.m
//function x = test_sparse106
//A = double(int32(10*sprandn(10,10,0.3)))+i*double(int32(10*sprandn(10,10,0.3)));
//B = double(int32(10*sprandn(10,10,0.3)))+i*double(int32(10*sprandn(10,10,0.3)));
//C = A < B;
//c = full(A) < full(B);
//x = testeq(c,C);
//C = A < -2;
//c = full(A) < -2;
//x = x & testeq(c,C);
//C = 2 < A;
//c = 2 < full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse107.m
//function x = test_sparse107
//A = double(int32(10*sprandn(10,10,0.3)))+i*double(int32(10*sprandn(10,10,0.3)));
//B = double(int32(10*sprandn(10,10,0.3)))+i*double(int32(10*sprandn(10,10,0.3)));
//C = A > B;
//c = full(A) > full(B);
//x = testeq(c,C);
//C = A > -2;
//c = full(A) > -2;
//x = x & testeq(c,C);
//C = 2 > A;
//c = 2 > full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse108.m
//function x = test_sparse108
//A = float(int32(10*sprandn(10,10,0.3)))+i*float(int32(10*sprandn(10,10,0.3)));
//B = float(int32(10*sprandn(10,10,0.3)))+i*float(int32(10*sprandn(10,10,0.3)));
//C = A > B;
//c = full(A) > full(B);
//x = testeq(c,C);
//C = A > -2;
//c = full(A) > -2;
//x = x & testeq(c,C);
//C = 2 > A;
//c = 2 > full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse109.m
//function x = test_sparse109
//A = float(int32(10*sprandn(10,10,0.3)))+i*float(int32(10*sprandn(10,10,0.3)));
//B = float(int32(10*sprandn(10,10,0.3)))+i*float(int32(10*sprandn(10,10,0.3)));
//C = A < B;
//c = full(A) < full(B);
//x = testeq(c,C);
//C = A < -2;
//c = full(A) < -2;
//x = x & testeq(c,C);
//C = 2 < A;
//c = 2 < full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse110.m
//function x = test_sparse110
//A = float(int32(10*sprandn(10,10,0.3)))+i*float(int32(10*sprandn(10,10,0.3)));
//B = float(int32(10*sprandn(10,10,0.3)))+i*float(int32(10*sprandn(10,10,0.3)));
//C = A >= B;
//c = full(A) >= full(B);
//x = testeq(c,C);
//C = A >= -2;
//c = full(A) >= -2;
//x = x & testeq(c,C);
//C = 2 >= A;
//c = 2 >= full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse111.m
//function x = test_sparse111
//A = float(int32(10*sprandn(10,10,0.3)))+i*float(int32(10*sprandn(10,10,0.3)));
//B = float(int32(10*sprandn(10,10,0.3)))+i*float(int32(10*sprandn(10,10,0.3)));
//C = A <= B;
//c = full(A) <= full(B);
//x = testeq(c,C);
//C = A <= -2;
//c = full(A) <= -2;
//x = x & testeq(c,C);
//C = 2 <= A;
//c = 2 <= full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse112.m
//function x = test_sparse112
//A = float(int32(10*sprandn(10,10,0.3)))+i*float(int32(10*sprandn(10,10,0.3)));
//B = float(int32(10*sprandn(10,10,0.3)))+i*float(int32(10*sprandn(10,10,0.3)));
//C = A == B;
//c = full(A) == full(B);
//x = testeq(c,C);
//C = A == -2;
//c = full(A) == -2;
//x = x & testeq(c,C);
//C = 2 == A;
//c = 2 == full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse113.m
//function x = test_sparse113
//A = float(int32(10*sprandn(10,10,0.3)))+i*float(int32(10*sprandn(10,10,0.3)));
//B = float(int32(10*sprandn(10,10,0.3)))+i*float(int32(10*sprandn(10,10,0.3)));
//C = A ~= B;
//c = full(A) ~= full(B);
//x = testeq(c,C);
//C = A ~= -2;
//c = full(A) ~= -2;
//x = x & testeq(c,C);
//C = 2 ~= A;
//c = 2 ~= full(A);
//x = x & testeq(c,C);
//@}
//!
Array LessThan(const Array& A, const Array& B) {
  return CmpOp<OpLessThan>(A,B);
}

/**
 * Element-wise less equals.
 */
Array LessEquals(const Array& A, const Array& B) {
  return CmpOp<OpLessEquals>(A,B);
}
  
/**
 * Element-wise greater than.
 */
Array GreaterThan(const Array& A, const Array& B) {
  return CmpOp<OpGreaterThan>(A,B);
}

/**
 * Element-wise greater equals.
 */
Array GreaterEquals(const Array& A, const Array& B) {
  return CmpOp<OpGreaterEquals>(A,B);
}

/**
 * Element-wise equals.
 */
Array Equals(const Array& A, const Array& B) {
  return CmpOp<OpEquals>(A,B);
}

/**
 * Element-wise notEquals.
 */
Array NotEquals(const Array& A, const Array& B) {
  return CmpOp<OpNotEquals>(A,B);
}

/**
 * Element-wise and
 */
//!
//@Module LOGICALOPS Logical Array Operators
//@@Section OPERATORS
//@@Usage
//There are three Boolean operators available in FreeMat.  The syntax for their use is:
//@[
//  y = ~x
//  y = a & b
//  y = a | b
//@]
//where @|x|, @|a| and @|b| are @|logical| arrays.  The operators are
//\begin{itemize}
//\item NOT (@|~|) - output @|y| is true if the corresponding element of @|x| is false, and ouput @|y| is false if the corresponding element of @|x| is true.
//\item OR (@|||) - output @|y| is true if corresponding element of @|a| is true or if corresponding element of @|b| is true (or if both are true).
//\item AND (@|\&|) - output @|y| is true only if both the corresponding elements of @|a| and @|b| are both true.
//\end{itemize}
//The binary operators AND and OR can take scalar arguments as well as vector arguments, in which case, the scalar is operated on with each element of the vector.
//As of version 1.10, FreeMat supports @|shortcut| evaluation.  This means that
//if we have two expressions
//@[
//  if (expr1 & expr2)
//@]
//then if @|expr1| evaluates to @|false|, then @|expr2| is not evaluated at all.
//Similarly, for the expression
//@[
//  if (expr1 | expr2)
//@]
//then if @|expr1| evaluates to @|true|, then @|expr2| is not evaluated at all.
//Shortcut evaluation is useful for doing a sequence of tests, each of which is
//not valid unless the prior test is successful.  For example,
//@[
//  if isa(p,'string') & strcmp(p,'fro')
//@]
//is not valid without shortcut evaluation (if @|p| is an integer, for example,
//the first test returns false, and an attempt to evaluate the second expression
//would lead to an error).  Note that shortcut evaluation only works with scalar
//expressions.
//@@Examples
//Some simple examples of logical operators.  Suppose we want to calculate the exclusive-or (XOR) of two vectors of logical variables.  First, we create a pair of vectors to perform the XOR operation on:
//@<
//a = (randn(1,6)>0)
//b = (randn(1,6)>0)
//@>
//Next, we can compute the OR of @|a| and @|b|:
//@<
//c = a | b
//@>
//However, the XOR and OR operations differ on the fifth entry - the XOR would be false, since it is true if and only if exactly one of the two inputs is true.  To isolate this case, we can AND the two vectors, to find exactly those entries that appear as true in both @|a| and @|b|:
//@<
//d = a & b
//@>
//At this point, we can modify the contents of @|c| in two ways -- the Boolean way is to AND @|\sim d| with @|c|, like so
//@<
//xor = c & (~d)
//@>
//The other way to do this is simply force @|c(d) = 0|, which uses the logical indexing mode of FreeMat (see the chapter on indexing for more details).  This, however, will cause @|c| to become an @|int32| type, as opposed to a logical type.
//@<
//c(d) = 0
//@>
//@@Tests
//@{ test_sparse114.m
//function x = test_sparse114
//A = sprandn(10,10,0.3)>0;
//B = sprandn(10,10,0.3)>0;
//C = A & B;
//c = full(A) & full(B);
//x = testeq(c,C);
//C = A & 0;
//c = full(A) & 0;
//x = x & testeq(c,C);
//C = 1 & A;
//c = 1 & full(A);
//x = x & testeq(c,C);
//@}
//@{ test_sparse115.m
//function x = test_sparse115
//A = sprandn(10,10,0.3)>0;
//B = sprandn(10,10,0.3)>0;
//C = A | B;
//c = full(A) | full(B);
//x = testeq(c,C);
//C = A | 0;
//c = full(A) | 0;
//x = x | testeq(c,C);
//C = 1 | A;
//c = 1 | full(A);
//x = x | testeq(c,C);
//@}
//!
Array And(const Array& A, const Array& B) {
  return DotOp<bool,OpAnd>(A,B,Bool);
}

/**
 * Element-wise or
 */
Array Or(const Array& A, const Array& B) {
  return DotOp<bool,OpOr>(A,B,Bool);
}

/**
 * Element-wise not
 */
static bool notfunc(bool t) {
  return (!t);
}

Array Not(const Array& A) {
  if (A.isScalar())
    return Array::Array(!A.toClass(Bool).constRealScalar<bool>());
  return Array::Array(Apply(A.toClass(Bool).constReal<bool>(),notfunc));
}

Array Plus(const Array& A) {
  return A;
}

/**
 * Element-wise negate - this one is a custom job, so to speak.
 * 
 */
Array Negate(const Array& A){
  if (A.isReferenceType())
    throw Exception("Cannot negate non-numeric types.");
  if (IsUnsigned(A))
    throw Exception("negation not supported for unsigned types.");
  Array B(double(0.0));
  return DotOp<OpSubtract>(B,A);
}


//!
//@Module TYPERULES Type Rules
//@@Section FreeMat
//@@Usage
//FreeMat follows an extended form of C's type rules (the extension
//is to handle complex data types.  The general rules are as follows:
//\begin{itemize}
//  \item Integer types are promoted to @|int32| types, except
//        for matrix operations and division operations.
//  \item Mixtures of @|float| and @|complex| types produce @|complex|
//        outputs.
//  \item Mixtures of @|double| or @|int32| types and @|dcomplex|
//        types produce @|dcomplex| outputs.
//  \item Arguments to operators are promoted to the largest type
//        present among the operands.
//  \item Type promotion is not allowed to reduce the information
//        content of the variable.  The only exception to this is
//        64-bit integers, which can lose information when they
//        are promoted to 64-bit @|double| values.  
//\end{itemize}
//These rules look tricky, but in reality, they are designed so that
//you do not actively have to worry about the types when performing
//mathematical operations in FreeMat.  The flip side of this, of course
//is that unlike C, the output of numerical operations is not automatically
//typecast to the type of the variable you assign the value to. 
//@@Tests
//@$"y=1+2","3","exact"
//@$"y=1f*i","i","exact"
//@$"y=pi+i","dcomplex(pi+i)","close"
//@$"y=1/2","0.5","exact"
//!

/**
 * We want to perform a matrix-matrix operation between two data objects.
 * The following checks are required:
 *  1. If A or B is a scalar, then return false - this is really a 
 *     vector operation, and the arguments should be passed to a 
 *     vector checker (like VectorCheck).
 *  2. Both A & B must be numeric
 *  3. Both A & B must be the same type (if not, the lesser type is
 *     automatically promoted).
 *  4. Both A & B must be 2-Dimensional.
 *  5. A & B must be conformant, i.e. the number of columns in A must
 *     match the number of rows in B.
 */


// Invert a square matrix - Should check for diagonal matrices
// as a special case
Array InvertMatrix(Array a) {
  if (!a.is2D())
    throw Exception("Cannot invert N-dimensional arrays.");
  if (a.isScalar())
    return DotRightDivide(Array(1.0),a);
  return Invert(a);
}
  
// Handle matrix powers for sparse matrices
Array MatrixPowerSparse(Array a, Array b, Interpreter* m_eval) {
  // The expression a^B where a is a scalar, and B is sparse is not handled
  if (a.isScalar() && !b.isScalar())
    throw Exception("expression a^B, where a is a scalar and B is a sparse matrix is not supported (use full to convert B to non-sparse matrix");
  // The expression A^B is not supported
  if (!a.isScalar() && !b.isScalar())
    throw Exception("expression A^B where A and B are both sparse matrices is not supported (or defined)");
  // The expression A^b where b is not an integer is not supported
  if (!b.isReal())
    throw Exception("expression A^b where b is complex and A is sparse is not supported (use full to convert A to a non-sparse matrix)");
  b.promoteType(FM_DOUBLE);
  const double*dp = (const double*) b.getDataPointer();
  if ((*dp) != rint(*dp))
    throw Exception("expression A^b where b is non-integer and A is sparse is not supported (use full to convert A to a non-sparse matrix)");
  if (a.getDimensionLength(0) != a.getDimensionLength(1))
    throw Exception("expression A^b requires A to be square.");
  int power = (int) *dp;
  if (power < 0) {
    a = InvertMatrix(a,m_eval);
    power = -power;
  }
  if (power == 0) {
    Array r(FM_FLOAT,a.dimensions(),
	    SparseOnesFunc(a.dataClass(),a.getDimensionLength(0),a.getDimensionLength(1),
			   a.getSparseDataPointer()),true);
    r.promoteType(a.dataClass());
    return r;
  }
  Array c(a);
  for (int i=1;i<power;i++)
    c = Multiply(c,a,m_eval);
  return c;
}


/**
 * Matrix-matrix multiply
 */
//!
//@Module TIMES Matrix Multiply Operator
//@@Section OPERATORS
//@@Usage
//Multiplies two numerical arrays.  This operator is really a combination
//of three operators, all of which have the same general syntax:
//@[
//  y = a * b
//@]
//where @|a| and @|b| are arrays of numerical type.  The result @|y| depends
//on which of the following three situations applies to the arguments
//@|a| and @|b|:
//\begin{enumerate}
//  \item @|a| is a scalar, @|b| is an arbitrary @|n|-dimensional numerical array, in which case the output is the element-wise product of @|b| with the scalar @|a|.
//  \item @|b| is a scalar, @|a| is an arbitrary @|n|-dimensional numerical array, in which case the output is the element-wise product of @|a| with the scalar @|b|.
//  \item @|a,b| are conformant matrices, i.e., @|a| is of size @|M x K|, and @|b| is of size @|K x N|, in which case the output is of size @|M x N| and is the matrix product of @|a|, and @|b|.
//\end{enumerate}
//The output follows the standard type promotion rules, although in the first two cases, if @|a| and @|b| are integers, the output is an integer also, while in the third case if @|a| and @|b| are integers, ,the output is of type @|double|.
//@@Function Internals
//There are three formulae for the times operator.  For the first form
//\[
//y(m_1,\ldots,m_d) = a \times b(m_1,\ldots,m_d),
//\]
//and the second form
//\[
//y(m_1,\ldots,m_d) = a(m_1,\ldots,m_d) \times b.
//\]
//In the third form, the output is the matrix product of the arguments
//\[
//y(m,n) = \sum_{k=1}^{K} a(m,k) b(k,n)
//\]
//@@Examples
//Here are some examples of using the matrix multiplication operator.  First,
//the scalar examples (types 1 and 2 from the list above):
//@<
//a = [1,3,4;0,2,1]
//b = a * 2
//@>
//The matrix form, where the first argument is @|2 x 3|, and the
//second argument is @|3 x 1|, so that the product is size 
//@|2 x 1|.
//@<
//a = [1,2,0;4,2,3]
//b = [5;3;1]
//c = a*b
//@>
//Note that the output is double precision.
//@@Tests
//@{ test_sparse50.m
//function x = test_sparse50
//a = rand(200,100);
//b = rand(100,300);
//a(a>0.1) = 0;
//b(b>0.1) = 0;
//c = a*b;
//A = sparse(a);
//B = sparse(b);
//C = A*B;
//x = testeq(c,C);
//@}
//@{ test_sparse51.m
//function x = test_sparse51
//a = rand(200,100);
//b = rand(100,300);
//c = rand(200,100);
//d = rand(100,300);
//a(a>0.1) = 0;
//b(b>0.1) = 0;
//c(c>0.1) = 0;
//d(d>0.1) = 0;
//f = a + i*c;
//g = b + i*d;
//h = f*g;
//F = sparse(f);
//G = sparse(g);
//H = F*G;
//x = testeq(h,H);
//@}
//@{ test_sparse52.m
//function x = test_sparse52
//a = rand(200,100);
//b = rand(100,300);
//a(a>0.1) = 0;
//c = a*b;
//A = sparse(a);
//C = A*b;
//x = testeq(c,C);
//@}
//@{ test_sparse53.m
//function x = test_sparse53
//a = rand(200,100);
//b = rand(100,300);
//c = rand(200,100);
//d = rand(100,300);
//a(a>0.1) = 0;
//b(b>0.1) = 0;
//c(c>0.1) = 0;
//d(d>0.1) = 0;
//f = a + i*c;
//g = b + i*d;
//h = f*g;
//F = sparse(f);
//H = F*g;
//x = testeq(h,H);
//@}
//@{ test_sparse54.m
//function x = test_sparse54
//a = rand(200,100);
//b = rand(100,300);
//b(b>0.1) = 0;
//c = a*b;
//B = sparse(b);
//C = a*B;
//x = testeq(c,C);
//@}
//@{ test_sparse55.m
//function x = test_sparse55
//a = randn(200,100);
//b = randn(100,300);
//c = randn(200,100);
//d = randn(100,300);
//c(c>0.1) = 0;
//d(d>0.1) = 0;
//f = a + i*c;
//g = b + i*d;
//h = f*g;
//G = sparse(g);
//H = f*G;
//e = h - H;
//t = max(abs(e(:)));
//x = (t < eps*600);
//@}
//@{ test_sparse59.m
//% Test sparse-dense matrix multiplication
//function x = test_sparse59
//[yi1,zi1] = sparse_test_mat('int32',300,400);
//[yf1,zf1] = sparse_test_mat('float',300,400);
//[yd1,zd1] = sparse_test_mat('double',300,400);
//[yc1,zc1] = sparse_test_mat('complex',300,400);
//[yz1,zz1] = sparse_test_mat('dcomplex',300,400);
//[yi2,zi2] = sparse_test_mat('int32',400,450);
//[yf2,zf2] = sparse_test_mat('float',400,450);
//[yd2,zd2] = sparse_test_mat('double',400,450);
//[yc2,zc2] = sparse_test_mat('complex',400,450);
//[yz2,zz2] = sparse_test_mat('dcomplex',400,450);
//x = testeq(yi1*zi2,zi1*zi2) & ...
//    testeq(yf1*zf2,zf1*zf2) & ...
//    testeq(yd1*zd2,zd1*zd2) & ...
//    testeq(yc1*zc2,zc1*zc2) & ...
//    testeq(yz1*zz2,zz1*zz2);
//@}
//@{ test_sparse60.m
//% Test dense-sparse matrix multiplication
//function x = test_sparse60
//[yi1,zi1] = sparse_test_mat('int32',300,400);
//[yf1,zf1] = sparse_test_mat('float',300,400);
//[yd1,zd1] = sparse_test_mat('double',300,400);
//[yc1,zc1] = sparse_test_mat('complex',300,400);
//[yz1,zz1] = sparse_test_mat('dcomplex',300,400);
//[yi2,zi2] = sparse_test_mat('int32',400,450);
//[yf2,zf2] = sparse_test_mat('float',400,450);
//[yd2,zd2] = sparse_test_mat('double',400,450);
//[yc2,zc2] = sparse_test_mat('complex',400,450);
//[yz2,zz2] = sparse_test_mat('dcomplex',400,450);
//x = testeq(zi1*yi2,zi1*zi2) & ...
//    testeq(zf1*yf2,zf1*zf2) & ...
//    testeq(zd1*yd2,zd1*zd2) & ...
//    testeq(zc1*yc2,zc1*zc2) & ...
//    testeq(zz1*yz2,zz1*zz2);
//@}
//@{ test_sparse61.m
//% Test sparse-sparse matrix multiplication
//function x = test_sparse61
//[yi1,zi1] = sparse_test_mat('int32',300,400);
//[yf1,zf1] = sparse_test_mat('float',300,400);
//[yd1,zd1] = sparse_test_mat('double',300,400);
//[yc1,zc1] = sparse_test_mat('complex',300,400);
//[yz1,zz1] = sparse_test_mat('dcomplex',300,400);
//[yi2,zi2] = sparse_test_mat('int32',400,450);
//[yf2,zf2] = sparse_test_mat('float',400,450);
//[yd2,zd2] = sparse_test_mat('double',400,450);
//[yc2,zc2] = sparse_test_mat('complex',400,450);
//[yz2,zz2] = sparse_test_mat('dcomplex',400,450);
//x = testeq(yi1*yi2,zi1*zi2) & ...
//    testeq(yf1*yf2,zf1*zf2) & ...
//    testeq(yd1*yd2,zd1*zd2) & ...
//    testeq(yc1*yc2,zc1*zc2) & ...
//    testeq(yz1*yz2,zz1*zz2);
//@}
//@{ test_sparse80.m
//% Test sparse-sparse matrix array multiply
//function x = test_sparse80
//[yi1,zi1] = sparse_test_mat('int32',300,400);
//[yf1,zf1] = sparse_test_mat('float',300,400);
//[yd1,zd1] = sparse_test_mat('double',300,400);
//[yc1,zc1] = sparse_test_mat('complex',300,400);
//[yz1,zz1] = sparse_test_mat('dcomplex',300,400);
//[yi2,zi2] = sparse_test_mat('int32',400,600);
//[yf2,zf2] = sparse_test_mat('float',400,600);
//[yd2,zd2] = sparse_test_mat('double',400,600);
//[yc2,zc2] = sparse_test_mat('complex',400,600);
//[yz2,zz2] = sparse_test_mat('dcomplex',400,600);
//x = testeq(yi1*yi2,zi1*zi2) & testeq(yf1*yf2,zf1*zf2) & testeq(yd1*yd2,zd1*zd2) & testeq(yc1*yc2,zc1*zc2) & testeq(yz1*yz2,zz1*zz2);
//@}
//!

template <typename T>
static inline Array Multiply(const Array& A, const Array& B) {
  if (A.isSparse() && !B.isSparse()) {
    if (A.allReal() && B.allReal()) {
      return Array(SparseMatrixMultiply<T>(A.constRealSparse<T>(),
					   B.constReal<T>()));
    } else {
      return Array(SparseMatrixMultiply<T>(A.constRealSparse<T>(),
					   A.constImagSparse<T>(),
					   
    }
  } else if (!A.isSparse() && B.isSparse()) {
  } else if (A.isSparse() && B.isSparse()) {
  } else {
    if (A.allReal() && B.allReal())  {
      BasicArray<T> C(NTuple(A.rows(),B.columns()));
      return Array(realMatrixMatrixMultiply<T>(A.rows(),
					       B.columns(),
					       A.columns(),
					       C.real<T>().data(),
					       A.constReal<T>().data(),
					       B.constReal<T>().data()));
      return Array(C);
    } else {
      BasicArray<T> C(NTuple(A.rows()*2,B.columns()));
      return Array(complexMatrixMatrixMultiply<T>(A.rows(),
						  B.columns(),
						  A.columns(),
						  C.real<T>().data(),
						  A.fortran<T>().data(),
						  B.fortran<T>().data()));
      return Array(SplitReal<T>(C),SplitImag<T>(C));
    }
  }
    }
  }
}

template <typename T>
static inline Array RealMultiply(const Array & A, const Array& B) {
  
}

template <typename T>
static inline Array Multiply(const Array& A, const Array& B) {
  if (A.allReal() && B.allReal())
    return RealMultiply<T>(A,B);
  else
    return ComplexMultiply<T>(A.asComplex(),B.asComplex());
}

Array Multiply(const Array& A, const Array& B){
  // Process our arguments
  if (!MatrixCheck(A,B,"*"))
    // Its really a vector product, pass...
    return DotMultiply(A,B);
  
  // Test for conformancy
  if (A.columns() != B.rows()) 
    throw Exception("Requested matrix multiplication requires arguments to be conformant.");

  if ((A.dataClass() == Float) && (B.dataClass() == Float)) {
    return Multiply<float>(A,B);
  } else if ((A.dataClass() == Double) && (B.dataClass() == Double)) {
    return Multiply<double>(A,B);
  } else
    throw Exception("Cannot multiply matrices of different types");
}
    
/**
 * Matrix-matrix divide
 */
//!
//@Module LEFTDIVIDE Matrix Equation Solver/Divide Operator
//@@Section OPERATORS
//@@Usage
//The divide operator @|\| is really a combination of three
//operators, all of which have the same general syntax:
//@[
//  Y = A \ B
//@]
//where @|A| and @|B| are arrays of numerical type.  The result @|Y| depends
//on which of the following three situations applies to the arguments
//@|A| and @|B|:
//\begin{enumerate}
//  \item @|A| is a scalar, @|B| is an arbitrary @|n|-dimensional numerical array, in which case the output is each element of @|B| divided by the scalar @|A|.
//  \item @|B| is a scalar, @|A| is an arbitrary @|n|-dimensional numerical array, in which case the output is the scalar @|B| divided by each element of @|A|.
//  \item @|A,B| are matrices with the same number of rows, i.e., @|A| is of size @|M x K|, and @|B| is of size @|M x L|, in which case the output is of size @|K x L|.
//\end{enumerate}
//The output follows the standard type promotion rules, although in the first two cases, if @|A| and @|B| are integers, the output is an integer also, while in the third case if @|A| and @|B| are integers, the output is of type @|double|.
//
//A few additional words about the third version, in which @|A| and @|B| are matrices.  Very loosely speaking, @|Y| is the matrix that satisfies @|A * Y = B|.  In cases where such a matrix exists.  If such a matrix does not exist, then a matrix @|Y| is returned that approximates @|A * Y \approx B|.
//@@Function Internals
//There are three formulae for the times operator.  For the first form
//\[
//Y(m_1,\ldots,m_d) = \frac{B(m_1,\ldots,m_d)}{A},
//\]
//and the second form
//\[
//Y(m_1,\ldots,m_d) = \frac{B}{A(m_1,\ldots,m_d)}.
//\]
//In the third form, the calculation of the output depends on the size of @|A|. Because each column of @|B| is treated independantly, we can rewrite the equation @|A Y = B| as
//\[
//  A [y_1, y_2,\ldots, y_l] = [b_1, b_2, \ldots, b_l]
//\]
//where @|y_i| are the columns of @|Y|, and @|b_i| are the columns of the matrix @|B|. If @|A| is a square matrix, then the LAPACK routine @|*gesvx| (where the @|*| is replaced with @|sdcz| depending on the type of the arguments) is used, which uses an LU decomposition of @|A| to solve the sequence of equations sequentially.  If @|A| is singular, then a warning is emitted. 
//
//On the other hand, if @|A| is rectangular, then the LAPACK routine @|*gelsy| is used.  Note that these routines are designed to work with matrices @|A| that are full rank - either full column rank or full row rank.  If @|A| fails to satisfy this assumption, a warning is emitted.  If @|A| has full column rank (and thus necessarily has more rows than columns), then theoretically, this operator finds the columns @|y_i| that satisfy:
//\[
//  y_i = \arg \min_y \| A y - b_i \|_2
//\]
//and each column is thus the Least Squares solution of @|A y = b_i|.  On the other hand, if @|A| has full row rank (and thus necessarily has more columns than rows), then theoretically, this operator finds the columns @|y_i| that satisfy
//\[
//  y_i = \arg \min_{A y = b_i} \| y \|_2
//\]
//and each column is thus the Minimum Norm vector @|y_i| that satisfies @|A y_i = b_i|.  
//In the event that the matrix @|A| is neither full row rank nor full column rank, a solution is returned, that is the minimum norm least squares solution.  The solution is computed using an orthogonal factorization technique that is documented in the LAPACK User's Guide (see the References section for details).
//@@Examples
//Here are some simple examples of the divide operator.  We start with a simple example of a full rank, square matrix:
//@<
//A = [1,1;0,1]
//@>
//Suppose we wish to solve
//\[
//  \begin{bmatrix} 1 & 1 \\ 0 & 1 \end{bmatrix}
//  \begin{bmatrix} y_1 \\ y_2 \end{bmatrix}
// = 
//  \begin{bmatrix} 3 \\ 2 \end{bmatrix}
//\]
//(which by inspection has the solution @|y_1 = 1|, @|y_2 = 2|).  Thus we compute:
//@<
//B = [3;2]
//Y = A\B
//@>
//
//Suppose we wish to solve a trivial Least Squares (LS) problem.  We want to find a simple scaling of the vector @|[1;1]| that is closest to the point @|[2,1]|.  This is equivalent to solving
//\[
//\begin{bmatrix} 1 \\ 1 \end{bmatrix} y = \begin{bmatrix} 2 \\ 1 \end{bmatrix}
//\]
//in a least squares sense.  For fun, we can calculate the solution using calculus by hand.  The error we wish to minimize is
//\[
//  \varepsilon(y) = (y - 2)^2 + (y-1)^2.
//\]
//Taking a derivative with respect to @|y|, and setting to zero (which we must have for an extrema when @|y| is unconstrained)
//\[
//  2 (y-2) + 2 (y-1) = 0
//\]
//which we can simplify to @|4y = 6| or @|y = 3/2| (we must, technically, check to make sure this is a minimum, and not a maximum or an inflection point).  Here is the same calculation performed using FreeMat:
//@<
//A = [1;1]
//B = [2;1]
//A\B
//@>
//which is the same solution.
//!
Array LeftDivide(Array A, Array B, Interpreter* m_eval) {
//   if (A.isEmpty() || B.isEmpty())
//     return Array::emptyConstructor();
  StringVector dummySV;
  // Process our arguments
  if (!MatrixCheck(A,B,"\\"))
    // Its really a vector product, pass...
    return DotLeftDivide(A,B,m_eval);
  
  // Test for conformancy
  if (A.getDimensionLength(0) != B.getDimensionLength(0)) 
    throw Exception("Requested divide operation requires arguments to have correct dimensions.");

  int Arows, Acols;
  int Brows, Bcols;
  
  Arows = A.getDimensionLength(0);
  Acols = A.getDimensionLength(1);
  Brows = B.getDimensionLength(0);
  Bcols = B.getDimensionLength(1);

  // Check for sparse case...
  if (A.sparse()) {
    // Make sure B is _not_ sparse
    B.makeDense();
    // Make sure A is square
    if (Arows != Acols)
      throw Exception("FreeMat currently only supports A\\b for square matrices A");
    // Make sure A is either double or dcomplex
    if ((A.dataClass() == FM_FLOAT) || (A.dataClass() == FM_COMPLEX))
      throw Exception("FreeMat currently only supports A\\b for double and dcomplex matrices A");
    Dimensions outDim(Arows,Bcols);
    return Array(A.dataClass(),outDim,
		 SparseSolveLinEq(A.dataClass(),Arows,Acols,A.getSparseDataPointer(),
				  Brows,Bcols,B.getDataPointer()),false);
  }
  
  // Its really a matrix-matrix operation, and the arguments are
  // satisfactory.  Check for the type.
  void *Cp;
  Dimensions outDim(2);
  if (Arows == Acols) {
    // Square matrix case - A is N x N, B is N x K - use 
    // linear equation solver.  Output is N x K.
    Cp = Malloc(Arows*Bcols*A.getElementSize());
    if (A.dataClass() == FM_FLOAT)
      floatSolveLinEq(m_eval,
		      Arows,Bcols,(float*)Cp,
		      (float*)A.getReadWriteDataPointer(),
		      (float*)B.getReadWriteDataPointer());
    else if (A.dataClass() == FM_COMPLEX)
      complexSolveLinEq(m_eval,
			Arows,Bcols,(float*)Cp,
			(float*)A.getReadWriteDataPointer(),
			(float*)B.getReadWriteDataPointer());
    else if (A.dataClass() == FM_DOUBLE)
      doubleSolveLinEq(m_eval,
		       Arows,Bcols,(double*)Cp,
		       (double*)A.getReadWriteDataPointer(),
		       (double*)B.getReadWriteDataPointer());
    else if (A.dataClass() == FM_DCOMPLEX)
      dcomplexSolveLinEq(m_eval,
			 Arows,Bcols,(double*)Cp,
			 (double*)A.getReadWriteDataPointer(),
			 (double*)B.getReadWriteDataPointer());
    outDim = Dimensions(Arows,Bcols);
  } else {
    // Rectangular matrix case - A is M x N, B must be M x K - use
    // lease squares equation solver.  Output is N x K.
    Cp = Malloc(Acols*Bcols*A.getElementSize());
    if (A.dataClass() == FM_FLOAT)
      floatSolveLeastSq(m_eval,
			Arows,Acols,Bcols,(float*)Cp,
			(float*)A.getReadWriteDataPointer(),
			(float*)B.getReadWriteDataPointer());
    else if (A.dataClass() == FM_COMPLEX)
      complexSolveLeastSq(m_eval,
			  Arows,Acols,Bcols,(float*)Cp,
			  (float*)A.getReadWriteDataPointer(),
			  (float*)B.getReadWriteDataPointer());
    else if (A.dataClass() == FM_DOUBLE)
      doubleSolveLeastSq(m_eval,
			 Arows,Acols,Bcols,(double*)Cp,
			 (double*)A.getReadWriteDataPointer(),
			 (double*)B.getReadWriteDataPointer());
    else if (A.dataClass() == FM_DCOMPLEX)
      dcomplexSolveLeastSq(m_eval,
			   Arows,Acols,Bcols,(double*)Cp,
			   (double*)A.getReadWriteDataPointer(),
			   (double*)B.getReadWriteDataPointer());
    outDim = Dimensions(Acols,Bcols);
  }
  return Array(A.dataClass(),outDim,Cp);
}
    
/**
 * Matrix-matrix divide
 */
//!
//@Module RIGHTDIVIDE Matrix Equation Solver/Divide Operator
//@@Section OPERATORS
//@@Usage
//The divide operator @|/| is really a combination of three
//operators, all of which have the same general syntax:
//@[
//  Y = A / B
//@]
//where @|A| and @|B| are arrays of numerical type.  The result @|Y| depends
//on which of the following three situations applies to the arguments
//@|A| and @|B|:
//\begin{enumerate}
//  \item @|A| is a scalar, @|B| is an arbitrary @|n|-dimensional numerical array, in which case the output is the scalar @|A| divided into each element of @|B|.
//  \item @|B| is a scalar, @|A| is an arbitrary @|n|-dimensional numerical array, in which case the output is each element of @|A| divided by the scalar @|B|.
//  \item @|A,B| are matrices with the same number of columns, i.e., @|A| is of size @|K x M|, and @|B| is of size @|L x M|, in which case the output is of size @|K x L|.
//\end{enumerate}
//The output follows the standard type promotion rules, although in the first two cases, if @|A| and @|B| are integers, the output is an integer also, while in the third case if @|A| and @|B| are integers, the output is of type @|double|.
//
//@@Function Internals
//There are three formulae for the times operator.  For the first form
//\[
//Y(m_1,\ldots,m_d) = \frac{A}{B(m_1,\ldots,m_d)},
//\]
//and the second form
//\[
//Y(m_1,\ldots,m_d) = \frac{A(m_1,\ldots,m_d)}{B}.
//\]
//In the third form, the output is defined as:
//\[
//  Y = (B' \backslash A')'
//\]
//and is used in the equation @|Y B = A|.
//@@Examples
//The right-divide operator is much less frequently used than the left-divide operator, but the concepts are similar.  It can be used to find least-squares and minimum norm solutions.  It can also be used to solve systems of equations in much the same way.  Here's a simple example:
//@<
//B = [1,1;0,1];
//A = [4,5]
//A/B
//@>
//@@Tests
//@{ test_sparse82.m
//% Test sparse-sparse matrix array solution
//function x = test_sparse82
//[yd1,zd1] = sparse_test_mat('double',100,100);
//[yz1,zz1] = sparse_test_mat('dcomplex',100,100);
//[yd2,zd2] = sparse_test_mat('double',100,60);
//[yz2,zz2] = sparse_test_mat('dcomplex',100,60);
//x = testeq3(yd1\yd2,zd1\zd2) & testeq3(yz1\yz2,zz1\zz2);
//
//function x = testeq3(a,b)
//  k = abs(a-b);
//  x = max(k(:)) < (size(a,2)*size(a,1)*eps*4);
//@}
//!
Array RightDivide(Array A, Array B, Interpreter* m_eval) {
  Array C;

  // Process our arguments
  if (!MatrixCheck(A,B,"/"))
    // Its really a vector product, pass...
    return DotRightDivide(A,B,m_eval);

  A.transpose();
  B.transpose();

  C = LeftDivide(B,A,m_eval);
  C.transpose();

  return C;
}


//!
//@Module HERMITIAN Matrix Hermitian (Conjugate Transpose) Operator
//@@Section OPERATORS
//@@Usage
//Computes the Hermitian of the argument (a 2D matrix).  The syntax for its use is
//@[
//  y = a';
//@]
//where @|a| is a @|M x N| numerical matrix.  The output @|y| is a numerical matrix
//of the same type of size @|N x M|.  This operator is the conjugating transpose,
//which is different from the transpose operator @|.'| (which does not 
//conjugate complex values).
//@@Function Internals
//The Hermitian operator is defined simply as
//\[
//  y_{i,j} = \overline{a_{j,i}}
//\]
//where @|y_ij| is the element in the @|i|th row and @|j|th column of the output matrix @|y|.
//@@Examples
//A simple transpose example:
//@<
//A = [1,2,0;4,1,-1]
//A'
//@>
//Here, we use a complex matrix to demonstrate how the Hermitian operator conjugates the entries.
//@<
//A = [1+i,2-i]
//A.'
//@>
//@@Tests
//@{ test_sparse76.m
//% Test sparse matrix array hermitian 
//function x = test_sparse76
//[yi1,zi1] = sparse_test_mat('int32',300,400);
//[yf1,zf1] = sparse_test_mat('float',300,400);
//[yd1,zd1] = sparse_test_mat('double',300,400);
//[yc1,zc1] = sparse_test_mat('complex',300,400);
//[yz1,zz1] = sparse_test_mat('dcomplex',300,400);
//x = testeq(yi1',zi1') & testeq(yf1',zf1') & testeq(yd1',zd1') & testeq(yc1',zc1') & testeq(yz1',zz1');
//@}
//!
Array Transpose(Array A, Interpreter* m_eval) {
  A.hermitian();
  return A;
}

//!
//@Module TRANSPOSE Matrix Transpose Operator
//@@Section OPERATORS
//@@Usage
//Performs a transpose of the argument (a 2D matrix).  The syntax for its use is
//@[
//  y = a.';
//@]
//where @|a| is a @|M x N| numerical matrix.  The output @|y| is a numerical matrix
//of the same type of size @|N x M|.  This operator is the non-conjugating transpose,
//which is different from the Hermitian operator @|'| (which conjugates complex values).
//@@Function Internals
//The transpose operator is defined simply as
//\[
//  y_{i,j} = a_{j,i}
//\]
//where @|y_ij| is the element in the @|i|th row and @|j|th column of the output matrix @|y|.
//@@Examples
//A simple transpose example:
//@<
//A = [1,2,0;4,1,-1]
//A.'
//@>
//Here, we use a complex matrix to demonstrate how the transpose does \emph{not} conjugate the entries.
//@<
//A = [1+i,2-i]
//A.'
//@>
//@@Tests
//@{ test_sparse77.m
//% Test sparse matrix array transpose
//function x = test_sparse77
//[yi1,zi1] = sparse_test_mat('int32',300,400);
//[yf1,zf1] = sparse_test_mat('float',300,400);
//[yd1,zd1] = sparse_test_mat('double',300,400);
//[yc1,zc1] = sparse_test_mat('complex',300,400);
//[yz1,zz1] = sparse_test_mat('dcomplex',300,400);
//x = testeq(yi1.',zi1.') & testeq(yf1.',zf1.') & testeq(yd1.',zd1.') & testeq(yc1.',zc1.') & testeq(yz1.',zz1.');
//@}
//!
Array DotTranspose(Array A, Interpreter* m_eval) {
  A.transpose();
  return A;
}



/**
 * Matrix-matrix power - These are the cases to consider:
 *   1. scalar-scalar defer to dotpower
 *   2. square-scalar - if an integer, iteratively multiply
 *          To raise A^N power, we do a successive factorization
 *          A -> A^2 -> A^4
 * 13 -> 8 + 4 + 2 + 1
 *  5 -> 4 + 2 + 1
 *  1 -> 1
 *
 * 23 -> 16 + 8 + 4 + 2 + 1
 *  7 -> 4 + 2 + 1
 *  3 -> 2 + 1
 *  1 -> 1
 *
 * 
 *   3. scalar-square - Decompose 
 */
//!
//@Module POWER Matrix Power Operator
//@@Section OPERATORS
//@@Usage
//The power operator for scalars and square matrices.  This operator is really a 
//combination of two operators, both of which have the same general syntax:
//@[
//  y = a ^ b
//@]
//The exact action taken by this operator, and the size and type of the output, 
//depends on which of the two configurations of @|a| and @|b| is present:
//\begin{enumerate}
//  \item @|a| is a scalar, @|b| is a square matrix
//  \item @|a| is a square matrix, @|b| is a scalar
//\end{enumerate}
//@@Function Internals
//In the first case that @|a| is a scalar, and @|b| is a square matrix, the matrix power is defined in terms of the eigenvalue decomposition of @|b|.  Let @|b| have the following eigen-decomposition (problems arise with non-symmetric matrices @|b|, so let us assume that @|b| is symmetric):
//\[
//  b = E \begin{bmatrix} \lambda_1 & 0          & \cdots  & 0 \\                            0   & \lambda_2  &  \ddots & \vdots \\                                         \vdots & \ddots & \ddots & 0 \\                                                 0   & \hdots & 0 & \lambda_n \end{bmatrix}
//      E^{-1}
//\]
//Then @|a| raised to the power @|b| is defined as
//\[
//  a^{b} = E \begin{bmatrix} a^{\lambda_1} & 0          & \cdots  & 0 \\                                0   & a^{\lambda_2}  &  \ddots & \vdots \\                                    \vdots & \ddots & \ddots & 0 \\                                             0   & \hdots & 0 & a^{\lambda_n} \end{bmatrix}
//      E^{-1}
//\]
//Similarly, if @|a| is a square matrix, then @|a| has the following eigen-decomposition (again, suppose @|a| is symmetric):
//\[
//  a = E \begin{bmatrix} \lambda_1 & 0          & \cdots  & 0 \\                                0   & \lambda_2  &  \ddots & \vdots \\                                         \vdots & \ddots & \ddots & 0 \\                                          0   & \hdots & 0 & \lambda_n \end{bmatrix}
//      E^{-1}
//\]
//Then @|a| raised to the power @|b| is defined as
//\[
//  a^{b} = E \begin{bmatrix} \lambda_1^b & 0          & \cdots  & 0 \\                              0   & \lambda_2^b  &  \ddots & \vdots \\                              \vdots & \ddots & \ddots & 0 \\                              0   & \hdots & 0 & \lambda_n^b \end{bmatrix}
//      E^{-1}
//\]
//@@Examples
//We first define a simple @|2 x 2| symmetric matrix
//@<
//A = 1.5
//B = [1,.2;.2,1]
//@>
//First, we raise @|B| to the (scalar power) @|A|:
//@<
//C = B^A
//@>
//Next, we raise @|A| to the matrix power @|B|:
//@<
//C = A^B
//@>
//!

// Raises Scalar^Matrix
static Array PowerScalarMatrix(const Array &A, const Array &B) {
  // Do an eigendecomposition of B
  Array V, D;
  if (IsSymmetric(B))
    EigenDecomposeFullSymmetric(B,V,D);
  else
    EigenDecomposeFullGeneral(B,V,D,false);
  // Get the diagonal part of D
  Array E = GetDiagonal(D);
  // Call the vector version of the exponential
  Array F = DotPower(A,E); // B, V, D, E, F
  // Construct a diagonal matrix from F
  Array G = DiagonalArray(F); // B, V, D, G, E, F
  // The output is (V*G)/V
  E = Multiply(V,G); // B, V, D, E, F
  return RightDivide(E,V); // B, D, F
}

// Raises Matrix^Scalar
static Array PowerMatrixScalar(const Array &A, const Array &B) {
  // Do an eigendecomposition of A
  Array V, D;
  if (IsSymmetric(A))
    EigenDecomposeFullSymmetric(A,V,D); //A, B, V, D
  else
    EigenDecomposeFullGeneral(A,V,D,false);
  // Get the diagonal part of D
  Array E = GetDiagonal(D); // A, B, V, D, E
  // Call the vector version of the exponential
  Array F = DotPower(E,B); // F, A, V, D
  // Construct a diagonal matrix from F
  Array G = DiagonalArray(F); // G, A, V, D, F
  // The output is (V*G)/V
  E = Multiply(V,G); // A, V, D, E, F
  return RightDivide(E,V); // C, A, D, F
}

Array Power(const Array& A, const Array& B){
  if (A.isEmpty() || B.isEmpty())
    return Array(Double);
  if (A.isScalar() && B.isScalar()) return DotPower(A,B);
  if (!A.is2D() || !B.is2D()) 
    throw Exception("Cannot apply exponential operator to N-Dimensional arrays.");
  if (B.isReal() && B.isScalar() && (B.toClass(Double).constRealScalar() == -1))
    return InvertMatrix(A);
  // Both arguments must be square
  if (!(A.isSquare() && B.isSquare()))
    throw Exception("Power (^) operator can only be applied to scalar and square arguments.");
  if (A.isSparse() || B.isSparse())
    return MatrixPowerSparse(A,B);
  // OK - check for A a scalar - if so, do a decomposition of B
  if (A.isScalar())
    return PowerScalarMatrix(A,B);
  else if (B.isScalar())
    return PowerMatrixScalar(A,B);
  else 
    throw Exception("One of the arguments to (^) must be a scalar.");
}

Array UnitColon(Array A, Array B) {
  Array C;
  if (!A.isScalar() || !B.isScalar())
    throw Exception("Both arguments to (:) operator must be scalars.");
  if (A.isComplex() || B.isComplex())
    throw Exception("Both arguments to (:) operator must be real.");
  if (!((A.dataClass() == Double) && (B.dataClass() == Double)))
    throw Exception("All arguments to (:) operator must be of class double.");
  return RangeConstructor(A.constScalar<double>(),1,B.constScalar<double>(),false);
}

Array DoubleColon(Array A, Array B, Array C){
  Array D;
  if (!A.isScalar() || !B.isScalar() || !C.isScalar())
    throw Exception("All three arguments to (:) operator must be scalars.");
  if (A.isComplex() || B.isComplex() || C.isComplex())
    throw Exception("All arguments to (:) operator must be real.");
  if (!((A.dataClass() == Double) && (B.dataClass() == Double) && (C.dataClass() == Double)))
    throw Exception("All arguments to (:) operator must be of class double.");
  return RangeConstructor(A.constScalar<double>(),B.constScalar<double>(),
			  C.constScalar<double>(),false);
}
