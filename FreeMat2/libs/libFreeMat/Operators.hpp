#ifndef __Operators_hpp__
#define __Operators_hpp__

#include "Array.hpp"
#include "SparseMatrix.hpp"
#include "Algorithms.hpp"

// Real, Real --> Real
template <typename S, typename T, class Op>
static inline SparseMatrix<S> DotOp(const SparseMatrix<T>& A, 
				    const SparseMatrix<T>& B) {
  ConstSparseIterator<T> aspin(&A);
  ConstSparseIterator<T> bspin(&B);
  SparseMatrix<S> retval(A.dimensions());
  // While more columns...
  while (aspin.isValid() || bspin.isValid()) {
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
    S value_c_real, value_c_imag;
    if (a_spin.pos() == b_spin.pos()) {
      Op::func(a_spin.realValue(),a_spin.imagValue(),
	       b_spin.realValue(),b_spin.imagValue(),
	       value_c_real,value_c_imag);
      C_real.set(a_spin.pos(),value_c_real);
      C_imag.set(a_spin.pos(),value_c_imag);
      a_spin.next(); b_spin.next();
    } else if (dim.map(a_spin.pos()) < dim.map(b_spin.pos())) {
      Op::func(a_spin.realValue(),a_spin.imagValue(),T(0),T(0),
	       value_c_real,value_c_imag);
      C_real.set(a_spin.pos(),value_c_real);
      C_imag.set(a_spin.pos(),value_c_imag);
      a_spin.next();
    } else {
      Op::func(T(0),T(0),b_spin.realValue(),b_spin.imagValue(),
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
  SparseMatrix<S> C(A_real.dimensions());
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
      C.set(a_spin.pos(),Op::func(a_spin.realValue(),a_spin.imagValue(),T(0),T(0)));
      a_spin.next();
    } else {
      C.set(b_spin.pos(),Op::func(T(0),T(0),b_spin.realValue(),b_spin.imagValue()));
      b_spin.next();
    }
  }
  return C;
}

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
}

// Real,Real --> Real
template <typename T, typename Atype, typename Btype, class Op>
static inline void DotOp(const Atype& A, const Btype& B, 
			 BasicArray<T> &retvec, const NTuple& dims) {
  retvec = BasicArray<T>(dims);
  for (index_t i=1;i<=dims.count();i++) {
    retvec.set(i,Op::func(A.get(i),B.get(i)));
  }
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
    Op::func(A.get(i),T(0),B.get(i),T(0),real,imag);
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
  Array F(Tclass,NTuple(0,0));
  if (Acast.isSparse() && Bcast.isSparse()) {
    if (Acast.dimensions() != Bcast.dimensions())
      throw Exception("size mismatch in arguments to binary operator");
    if (Bcast.allReal() && Acast.allReal()) {
      F = DotOp<T,T,Op>(Acast.constRealSparse<T>(),
			Bcast.constRealSparse<T>());
    } else {
      Acast.forceComplex(); Bcast.forceComplex();
      SparseMatrix<T> Freal(Acast.dimensions());
      SparseMatrix<T> Fimag(Acast.dimensions());;
      DotOp<T,T,Op>(Acast.constRealSparse<T>(),
		    Acast.constImagSparse<T>(),
		    Bcast.constRealSparse<T>(),
		    Bcast.constImagSparse<T>(),
		    Freal, Fimag);
      F = Array(Freal,Fimag);
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
      Acast.forceComplex(); Bcast.forceComplex();
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
      Acast.forceComplex(); Bcast.forceComplex();
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
      Acast.forceComplex(); Bcast.forceComplex();
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
      Acast.forceComplex(); Bcast.forceComplex();
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
//
// So the table looks like this (order is unimportant)
// a  b  c v
// s  s  s s
// s  d  s s
// s  c  s s
// s  l  s s
// s  i  e 
// d  d  d d
// d  c  d d
// d  l  d d
// d  i  i d
// i1 i2 e 
// c  i  e
// c  l  d d
// c  c  d d
// l  l  d d
//
// The resulting rule:
//   default: c is double, v is double
//   if (a or b is single), c is single, v is single
//   if (a or b is an integer)
//     if (same type, or one is a double), 
//       c is integer
//     else illegal.
//
// if (a or b single) --> v single, c single
// else {
//   v double
//   if (a and b not integer) --> c double, else c integer
// }
//
// Arithmetic Operations on Floating-Point Numbers
// This section describes which classes you can use in arithmetic operations with floating-point numbers.
// Double-Precision.   You can perform basic arithmetic operations with double and any of the following other classes. When one or more operands is an integer (scalar or array), the double operand must be a scalar. The result is of type double, except where noted otherwise:
//       single  The result is of type single
//       double
//       int* or uint*  The result has the same data type as the integer operand
//       char
//       logical
// This example performs arithmetic on data of types char and double. The result is of type double:
// c = 'uppercase' - 32;
// class(c)
// ans =
//    double
// char(c)
// ans =
//    UPPERCASE
// Single-Precision.   You can perform basic arithmetic operations with single and any of the following other classes. The result is always single:
//       single
//       double
//       char
//       logical
// Arithmetic Operations on Integer Classes
// MATLAB can perform integer arithmetic on the following types of data:
//       Integers or integer arrays of the same integer data type. This yields a result that has the same data type as the operands:
//          x = uint32([132 347 528]) .* uint32(75);
//       Integers or integer arrays and scalar double-precision floating-point numbers. This yields a result that has the same data type as the integer operands:
//          x = uint32([132 347 528]) .* 75.49;
// For all binary operations in which one operand is an array of integer data type and the other is a scalar double, MATLAB computes the operation using elementwise double-precision arithmetic, and then converts the result back to the original integer data type.

static inline bool IsIntegerDataClass(const Array &Ain) {
  return ((Ain.dataClass() >= Int8) && (Ain.dataClass() <= UInt64));
}

template <class Op>
static inline Array DotOp(const Array &Ain, const Array &Bin) {
  DataClass via_type = Double;
  DataClass out_type = Double;

  if (Ain.dataClass() == Float || Bin.dataClass() == Float) {
    if (IsIntegerDataClass(Ain) || IsIntegerDataClass(Bin))
      throw Exception("Cannot combine single precision and integer class data");
    via_type = Float;
    out_type = Float;
  } else {
    if (IsIntegerDataClass(Ain) && IsIntegerDataClass(Bin) &&
	Ain.dataClass() != Bin.dataClass()) 
      throw Exception("Cannot combine data of different integer data classes");
    if (IsIntegerDataClass(Ain)) out_type = Ain.dataClass();
    if (IsIntegerDataClass(Bin)) out_type = Bin.dataClass();
  }
  Array F;
  if (via_type == Float)
    F = DotOp<float,Op>(Ain,Bin,Float);
  else
    F = DotOp<double,Op>(Ain,Bin,Double);
  return F.toClass(out_type);
}


template <typename T, class Op>
static inline Array CmpOp(const Array &Ain, const Array &Bin, DataClass Tclass) {
  Array Acast(Ain.toClass(Tclass));
  Array Bcast(Bin.toClass(Tclass));
  Array F(Bool,NTuple(0,0));
  if (Acast.isSparse() && Bcast.isSparse()) {
    if (Acast.dimensions() != Bcast.dimensions())
      throw Exception("size mismatch in arguments to binary operator");
    if (Bcast.allReal() && Acast.allReal()) {
      F = DotOp<bool,T,Op>(Acast.constRealSparse<T>(),
			   Bcast.constRealSparse<T>());
    } else {
      Acast.forceComplex(); Bcast.forceComplex();
      F = DotOp<bool,T,Op>(Acast.constRealSparse<T>(),
			   Acast.constImagSparse<T>(),
			   Bcast.constRealSparse<T>(),
			   Bcast.constImagSparse<T>());
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
      Acast.forceComplex(); Bcast.forceComplex();
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
      Acast.forceComplex(); Bcast.forceComplex();
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
      Acast.forceComplex(); Bcast.forceComplex();
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
      Acast.forceComplex(); Bcast.forceComplex();
      DotOp<bool,BasicArray<T>,BasicArray<T>,Op>
	(Acast.constReal<T>(), Acast.constImag<T>(),
	 Bcast.constReal<T>(), Bcast.constImag<T>(),
	 F.real<bool>(), Acast.dimensions());
    }
  }
  return F;
}

template <class Op>
static inline Array CmpOp(const Array &Ain, const Array &Bin) {
  DataClass via_type = Double;

  if (Ain.dataClass() == Float || Bin.dataClass() == Float) 
    via_type = Float;
  else
    via_type = Double;
  Array F;
  if (via_type == Float) 
    F = CmpOp<float,Op>(Ain,Bin,Float);
  else
    F = CmpOp<double,Op>(Ain,Bin,Double);
  return F.toClass(Bool);
}

// Real --> Real
template <typename S, typename T, class Op>
static inline SparseMatrix<S> UnaryOp(const SparseMatrix<T>& A) {
  ConstSparseIterator<T> aspin(&A);
  SparseMatrix<S> retval(A.dimensions());
  while (aspin.isValid()) {
    retval.set(aspin.pos(),Op::func(aspin.value()));
    aspin.next();
  }
  return retval;
}
	
// Complex --> Complex
template <typename S, typename T, class Op>
static inline void UnaryOp(const SparseMatrix<T>& A_real, 
			   const SparseMatrix<T>& A_imag,
			   SparseMatrix<S>& C_real,
			   SparseMatrix<S>& C_imag) {
  C_real = SparseMatrix<S>(A_real.dimensions());
  C_imag = SparseMatrix<S>(A_imag.dimensions());
  ConstComplexSparseIterator<T> a_spin(&A_real, &A_imag);
  NTuple dim(A_real.dimensions());
  while (a_spin.isValid()) {
    S value_c_real, value_c_imag;
    Op::func(a_spin.realValue(),a_spin.imagValue(),
	     value_c_real,value_c_imag);
    C_real.set(a_spin.pos(),value_c_real);
    C_imag.set(a_spin.pos(),value_c_imag);
    a_spin.next();
  }
}

// Real --> Real
template <typename T, typename Atype, class Op>
static inline void UnaryOp(const Atype& A, 
			   BasicArray<T> &retvec, const NTuple& dims) {
  retvec = BasicArray<T>(dims);
  for (index_t i=1;i<=dims.count();i++) {
    retvec.set(i,Op::func(A.get(i)));
  }
}

// Complex --> Complex
template <typename T, typename Atype, class Op>
static inline void UnaryOp(const Atype& A_real, const Atype& A_imag,
			   BasicArray<T>& C_real, BasicArray<T>& C_imag, 
			   const NTuple& dims) {
  C_real = BasicArray<T>(dims);
  C_imag = BasicArray<T>(dims);
  for (index_t i=1;i<=dims.count();++i) {
    T real, imag;
    Op::func(A_real.get(i),A_imag.get(i),real,imag);
    C_real.set(i,real);
    C_imag.set(i,imag);
  }
}

// Perform the operation via a typed intermediary
template <typename T, class Op>
static inline Array UnaryOp(const Array &Ain, DataClass Tclass) {
  Array Acast(Ain.toClass(Tclass));
  Array F(Tclass,NTuple(0,0));
  if (Acast.isSparse()) {
    if (Acast.allReal()) {
      F = UnaryOp<T,T,Op>(Acast.constRealSparse<T>());
    } else {
      SparseMatrix<T> Freal(Acast.dimensions());
      SparseMatrix<T> Fimag(Acast.dimensions());
      UnaryOp<T,T,Op>(Acast.constRealSparse<T>(),
		      Acast.constImagSparse<T>(),
		      Freal,Fimag);
      F = Array(Freal,Fimag);
    }
    return F;
  }
  if (!Acast.isScalar()) Acast = Acast.asDenseArray();
  if (Acast.isScalar()) {
    if (Acast.allReal()) {
      F = Array::Array(Op::func(Acast.constRealScalar<T>()));
    } else {
      F = Array::Array(T(0),T(0));
      Op::func(Acast.constRealScalar<T>(),
	       Acast.constImagScalar<T>(),
	       F.realScalar<T>(),F.imagScalar<T>());
    }
  } else {
    if (Acast.allReal()) {
      UnaryOp<T,BasicArray<T>,Op>
	(Acast.constReal<T>(),
	 F.real<T>(),
	 Acast.dimensions());
    } else {
      UnaryOp<T,BasicArray<T>,Op>
	(Acast.constReal<T>(),
	 Acast.constImag<T>(),
	 F.real<T>(),
	 F.imag<T>(),
	 Acast.dimensions());
    }
  }
  return F;
}

template <class Op>
static inline Array UnaryOp(const Array &Ain) {
  if (Ain.dataClass() == Float)
    return UnaryOp<float,Op>(Ain,Float);
  else
    return UnaryOp<double,Op>(Ain,Double);
}

template <typename T, class Op>
static inline Array VectorOp(const BasicArray<T> &real,
			     const BasicArray<T> &imag, 
			     index_t out, int dim) {
  NTuple outdims(real.dimensions()); outdims[dim] = out;
  BasicArray<T> F_real(outdims); 
  BasicArray<T> F_imag(outdims);
  ConstBasicIterator<T> source_real(&real,dim);
  ConstBasicIterator<T> source_imag(&imag,dim);
  BasicIterator<T> dest_real(&F_real,dim);
  BasicIterator<T> dest_imag(&F_imag,dim);
  BasicArray<T> in_buffer_real(NTuple(real.dimensions()[dim],1));
  BasicArray<T> in_buffer_imag(NTuple(imag.dimensions()[dim],1));
  BasicArray<T> out_buffer_real(NTuple(out,1));
  BasicArray<T> out_buffer_imag(NTuple(out,1));
  while (source_real.isValid() && dest_real.isValid()) {
    for (index_t i=1;i<=source_real.size();i++) {
      in_buffer_real[i] = source_real.get();
      in_buffer_imag[i] = source_imag.get();
      source_real.next(); source_imag.next();
    }
    Op::func(in_buffer_real,in_buffer_imag,
	     out_buffer_real,out_buffer_imag);
    for (index_t i=1;i<=out;i++) {
      dest_real.set(out_buffer_real[i]);
      dest_imag.set(out_buffer_imag[i]);
      dest_real.next(); dest_imag.next();
    }
    source_real.nextSlice(); source_imag.nextSlice();
    dest_real.nextSlice(); dest_imag.nextSlice();
  }
  return Array(F_real,F_imag);
}

template <typename T, class Op>
static inline Array VectorOp(const SparseMatrix<T>& real, index_t out, int dim) {
  if (dim == 0) {
    ConstSparseIterator<T> spin_real(&real);
    NTuple outdims(real.dimensions()); outdims[dim] = out;
    SparseMatrix<T> retval(outdims);
    while (spin_real.isValid()) {
      SparseSlice<T> this_col;
      index_t col_number = spin_real.col();
      Op::func(spin_real,this_col);
      retval.data()[col_number] = this_col;
    }
    return Array(retval);
  } else
    return Transpose(VectorOp<T,Op>(Transpose(real),out,0));
}

template <typename T, class Op>
static inline Array VectorOp(const SparseMatrix<T> &real,
			     const SparseMatrix<T> &imag, 
			     index_t out, int dim) {
  if (dim == 0) {
    ConstComplexSparseIterator<T> spin_complex(&real,&imag);
    NTuple outdims(real.dimensions()); outdims[dim] = out;
    SparseMatrix<T> retval_real(outdims);
    SparseMatrix<T> retval_imag(outdims);
    while (spin_complex.isValid()) {
      SparseSlice<T> this_real_col;
      SparseSlice<T> this_imag_col;
      index_t col_number = spin_complex.col();
      Op::func(spin_complex,this_real_col,this_imag_col);
      retval_real.data()[col_number] = this_real_col;
      retval_imag.data()[col_number] = this_imag_col;
    }
    return Array(retval_real,retval_imag);
  } else
    return Transpose(VectorOp<T,Op>(Transpose(real),
				    Transpose(imag),out,0));
}

template <typename T, class Op>
static inline Array VectorOp(const BasicArray<T> &real, index_t out, int dim) {
  NTuple outdims(real.dimensions()); outdims[dim] = out;
  BasicArray<T> F(outdims); 
  ConstBasicIterator<T> source(&real,dim);
  BasicIterator<T> dest(&F,dim);
  BasicArray<T> in_buffer(NTuple(real.dimensions()[dim],1));
  BasicArray<T> out_buffer(NTuple(out,1));
  while (source.isValid() && dest.isValid()) {
    for (index_t i=1;i<=source.size();i++) {
      in_buffer[i] = source.get();
      source.next();
    }
    Op::func(in_buffer,out_buffer);
    for (index_t i=1;i<=out;i++) {
      dest.set(out_buffer[i]);
      dest.next();
    }
    source.nextSlice(); dest.nextSlice();
  }
  return Array(F);
}

template <typename T, class Op>
static inline Array VectorOp(const Array &Ain, index_t out, int dim, DataClass Tclass) {
  Array Acast(Ain.toClass(Tclass));
  Array F;
  if (Acast.isSparse()) {
    if (Acast.allReal())
      F = VectorOp<T,Op>(Acast.constRealSparse<T>(),out,dim);
    else
      F = VectorOp<T,Op>(Acast.constRealSparse<T>(),
			 Acast.constImagSparse<T>(),out,dim);
  }
  if (!Acast.isScalar()) Acast = Acast.asDenseArray();
  if (Acast.allReal()) {
    F = VectorOp<T,Op>(Acast.constReal<T>(),out,dim);
  } else {
    F = VectorOp<T,Op>(Acast.constReal<T>(),Acast.constImag<T>(),out,dim);
  }
  return F.toClass(Ain.dataClass());
}

template <class Op>
  static inline Array VectorOp(const Array &Ain, int out, int dim) {
  if (Ain.dataClass() == Float)
    return VectorOp<float,Op>(Ain,out,dim,Float).toClass(Ain.dataClass());
  else
    return VectorOp<double,Op>(Ain,out,dim,Double).toClass(Ain.dataClass());
}

template <typename T, class Op>
static inline Array BiVectorOp(const BasicArray<T> &real,
			       const BasicArray<T> &imag, 
			       index_t out, int dim,
			       Array &D) {
  NTuple outdims(real.dimensions()); outdims[dim] = out;
  BasicArray<T> F_real(outdims); 
  BasicArray<T> F_imag(outdims);
  BasicArray<index_t> Ddata(outdims);
  ConstBasicIterator<T> source_real(&real,dim);
  ConstBasicIterator<T> source_imag(&imag,dim);
  BasicIterator<T> dest_real(&F_real,dim);
  BasicIterator<T> dest_imag(&F_imag,dim);
  BasicArray<T> in_buffer_real(NTuple(real.dimensions()[dim],1));
  BasicArray<T> in_buffer_imag(NTuple(imag.dimensions()[dim],1));
  BasicArray<T> out_buffer_real(NTuple(out,1));
  BasicArray<T> out_buffer_imag(NTuple(out,1));
  BasicArray<index_t> out_buffer_index(NTuple(out,1));
  BasicIterator<index_t> D_iter(&Ddata,dim);
  while (source_real.isValid() && dest_real.isValid()) {
    for (index_t i=1;i<=source_real.size();i++) {
      in_buffer_real[i] = source_real.get();
      in_buffer_imag[i] = source_imag.get();
      source_real.next(); source_imag.next();
    }
    Op::func(in_buffer_real,in_buffer_imag,
	     out_buffer_real,out_buffer_imag,
	     out_buffer_index);
    for (index_t i=1;i<=out;i++) {
      dest_real.set(out_buffer_real[i]);
      dest_imag.set(out_buffer_imag[i]);
      D_iter.set(out_buffer_index[i]);
      dest_real.next(); dest_imag.next();
      D_iter.next();
    }
    source_real.nextSlice(); source_imag.nextSlice();
    dest_real.nextSlice(); dest_imag.nextSlice();
    D_iter.nextSlice();
  }
  D = Array(Ddata);
  return Array(F_real,F_imag);
}

template <typename T, class Op>
static inline Array BiVectorOp(const SparseMatrix<T>& real, index_t out, int dim,
			       Array &D) {
  if (dim == 0) {
    ConstSparseIterator<T> spin_real(&real);
    NTuple outdims(real.dimensions()); outdims[dim] = out;
    SparseMatrix<T> retval(outdims);
    SparseMatrix<index_t> Dval(outdims);
    while (spin_real.isValid()) {
      SparseSlice<T> this_col;
      SparseSlice<index_t> this_index;
      index_t col_number = spin_real.col();
      Op::func(spin_real,this_col,this_index);
      retval.data()[col_number] = this_col;
      Dval.data()[col_number] = this_index;
    }
    D = Array(Dval);
    return Array(retval);
  } else {
    Array F = Transpose(BiVectorOp<T,Op>(Transpose(real),out,0,D));
    D = Transpose(D);
    return F;
  }
}

template <typename T, class Op>
static inline Array BiVectorOp(const SparseMatrix<T> &real,
			       const SparseMatrix<T> &imag, index_t out, int dim,
			       Array &D) {
  if (dim == 0) {
    ConstComplexSparseIterator<T> spin_complex(&real,&imag);
    NTuple outdims(real.dimensions()); outdims[dim] = out;
    SparseMatrix<T> retval_real(outdims);
    SparseMatrix<T> retval_imag(outdims);
    SparseMatrix<index_t> Dval(outdims);
    while (spin_complex.isValid()) {
      SparseSlice<T> this_real_col;
      SparseSlice<T> this_imag_col;
      SparseSlice<index_t> this_index;
      index_t col_number = spin_complex.col();
      Op::func(spin_complex,this_real_col,this_imag_col,this_index);
      retval_real.data()[col_number] = this_real_col;
      retval_imag.data()[col_number] = this_imag_col;
      Dval.data()[col_number] = this_index;
    }
    D = Array(Dval);
    return Array(retval_real,retval_imag);
  } else {
    Array F = Transpose(BiVectorOp<T,Op>(Transpose(real),
					 Transpose(imag),out,0,D));
    D = Transpose(D);
    return F;
  }
}

template <typename T, class Op>
static inline Array BiVectorOp(const BasicArray<T> &real, index_t out, int dim,
			       Array &D) {
  NTuple outdims(real.dimensions()); outdims[dim] = out;
  BasicArray<T> F(outdims); 
  BasicArray<index_t> Ddata(outdims);
  ConstBasicIterator<T> source(&real,dim);
  BasicIterator<T> dest(&F,dim);
  BasicIterator<index_t> destD(&Ddata,dim);
  BasicArray<T> in_buffer(NTuple(real.dimensions()[dim],1));
  BasicArray<T> out_buffer1(NTuple(out,1));
  BasicArray<index_t> out_buffer2(NTuple(out,1));
  while (source.isValid() && dest.isValid() && destD.isValid()) {
    for (index_t i=1;i<=source.size();i++) {
      in_buffer[i] = source.get();
      source.next();
    }
    Op::func(in_buffer,out_buffer1,out_buffer2);
    for (index_t i=1;i<=out;i++) {
      dest.set(out_buffer1[i]);
      destD.set(out_buffer2[i]);
      dest.next();
      destD.next();
    }
    source.nextSlice(); dest.nextSlice(); destD.nextSlice();
  }
  D = Array(Ddata);
  return Array(F);
}

template <typename T, class Op>
static inline ArrayVector BiVectorOp(const Array &Ain, index_t out, 
				     int dim, DataClass Tclass) {
  Array Acast(Ain.toClass(Tclass));
  Array F, D;
  if (Acast.isSparse()) {
    if (Acast.allReal())
      F = BiVectorOp<T,Op>(Acast.constRealSparse<T>(),out,dim,D);
    else
      F = BiVectorOp<T,Op>(Acast.constRealSparse<T>(),
			 Acast.constImagSparse<T>(),out,dim,D);
  }
  if (Acast.isScalar()) Acast = Acast.asDenseArray();
  if (Acast.allReal()) {
    F = BiVectorOp<T,Op>(Acast.constReal<T>(),out,dim,D);
  } else {
    F = BiVectorOp<T,Op>(Acast.constReal<T>(),Acast.constImag<T>(),out,dim,D);
  }
  F.toClass(Ain.dataClass());
  ArrayVector ret;
  ret.push_back(F);
  ret.push_back(D);
  return ret;
}

template <class Op>
  static inline ArrayVector BiVectorOp(const Array &Ain, int out, int dim) {
  if (Ain.dataClass() == Float) {
    ArrayVector Ret(BiVectorOp<float,Op>(Ain,out,dim,Float));
    Ret[0] = Ret[0].toClass(Ain.dataClass());
    return Ret;
  } else {
    ArrayVector Ret(BiVectorOp<double,Op>(Ain,out,dim,Double));
    Ret[0] = Ret[0].toClass(Ain.dataClass());
    return Ret;
  }
}

#endif
