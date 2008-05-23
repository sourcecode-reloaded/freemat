#ifndef __Operators_hpp__
#define __Operators_hpp__

// Real, Real --> Real
template <typename S, typename T, class Op>
static inline SparseMatrix<S> DotOp(const SparseMatrix<T>& A, 
				    const SparseMatrix<T>& B) {
  ConstSparseIterator<T> aspin(&A);
  ConstSparseIterator<T> bspin(&B);
  SparseMatrix<S> retval(A.dimensions());
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
	 F.real<bool>(), Acast.dimensions());
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

// Real --> Real
template <typename S, typename T, class Op>
static inline SparseMatrix<S> UnaryOp(const SparseMatrix<T>& A) {
  ConstSparseIterator<T> aspin(&A);
  SparseMatrix<S> retval(A.dimensions());
  while (aspin.isValid()) {
    while (aspin.moreInSlice()) {
      retval.set(aspin.pos(),Op::func(aspin.value()));
      aspin.next();
    }
    aspin.nextSlice();
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
  Array F(Tclass);
  if (Acast.isSparse()) {
    if (Acast.allReal()) {
      F = UnaryOp<T,T,Op>(Acast.constRealSparse<T>());
    } else {
      UnaryOp<T,T,Op>(Acast.constRealSparse<T>(),
		      Acast.constImagSparse<T>(),
		      F.realSparse<T>(),
		      F.imagSparse<T>());
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
    return UnaryOp<float,Op>(Ain,Float).toClass(Ain.dataClass());
  else
    return UnaryOp<double,Op>(Ain,Double).toClass(Ain.dataClass());
}

// Real --> Real
template <typename S, typename T, class Op>
static inline SparseMatrix<S> ReductionOp(const SparseMatrix<T>& A, int dim) {
  ConstSparseIterator<T> aspin(&A);
  SparseMatrix<S> retval(A.dimensions().forceOne(dim));
  // Reduce down the columns
  if (dim == 0) {
    while (aspin.isValid()) {
      T accum = Op::init();
      while (aspin.moreInSlice()) {
	accum = Op::func(accum,aspin.value());
	aspin.next();
      }
      retval.set(aspin.pos().forceOne(0),accum);
      aspin.nextSlice();
    }
  } else {
    for (index_t i=1;i<=A.rows();i++)
      retval.set(NTuple(i,1),Op::init());
    while (aspin.isValid()) {
      while (aspin.moreInSlice()) {
	retval.set(aspin.pos().forceOne(1),
		   Op::func(retval.get(aspin.pos().forceOne(1),
				       aspin.value())));
	aspin.next();
      }
      aspin.nextSlice();
    }
  }
  return retval;
}

// Complex --> Complex
template <typename S, typename T, class Op>
static inline void ReductionOp(const SparseMatrix<T>& A_real, 
			       const SparseMatrix<T>& A_imag,
			       int dim,
			       SparseMatrix<S>& C_real,
			       SparseMatrix<S>& C_imag) {
  C_real = SparseMatrix<S>(A_real.dimensions().forceOne(dim));
  C_imag = SparseMatrix<S>(A_imag.dimensions().forceOne(dim));
  ConstComplexSparseIterator<T> a_spin(&A_real, &A_imag);
  if (dim == 0) {
    while (a_spin.isValid()) {
      T accum_real = Op::init_real();
      T accum_imag = Op::init_imag();
      while (a_spin.moreInSlice()) {
	Op::func(accum_real,accum_imag,
		 a_spin.realValue(),a_spin.imagValue());
	a_spin.next();
      }
      C_real.set(a_spin.pos().forceOne(dim),accum_real);
      C_imag.set(a_spin.pos().forceOne(dim),accum_imag);
      a_spin.nextSlice();
    }
  } else {
    for (index_t i=1;i<=C_real.rows();i++) {
      C_real.set(NTuple(i,1),Op::init_real());
      C_imag.set(NTuple(i,1),Op::init_imag());
    }
    while (a_spin.isValid()) {
      while (a_spin.moreInSlice()) {
	T accum_real = C_real.get(a_spin.pos().forceOne(dim));
	T accum_imag = C_imag.get(a_spin.pos().forceOne(dim));
	Op::func(accum_real,accum_imag,
		    a_spin.realValue(),a_spin.imagValue());
	C_real.set(a_spin.pos().forceOne(dim),accum_real);
	C_imag.set(a_spin.pos().forceOne(dim),accum_imag);
	a_spin.next();
      }
      a_spin.nextSlice();
    }
  }
}

// Real --> Real
template <typename T, typename Atype, class Op>
static inline void ReductionOp(const Atype& A, 
			       int dim,
			       BasicArray<T> &retvec, const NTuple& dims) {
  retvec = BasicArray<T>(dims.forceOne(dim));
  ConstBaseIterator<Atype,T> m_source(&A,dim);
  BaseIterator<BasicArray<T>,T> m_dest(&retvec,dim);
  while (m_dest.isValid() && m_source.isValid()) {
    T accum = Op::init();
    for (int i=0;i<m_source.size();i++) {
      accum = Op::func(accum,m_source.get());
      m_source.next();
    }
    m_dest.set(accum);
    m_dest.nextSlice();
    m_source.nextSlice();
  }
}

// Complex --> Complex
template <typename T, typename Atype, class Op>
static inline void ReductionOp(const Atype& A_real, const Atype& A_imag,
			       int dim,
			       BasicArray<T>& C_real, BasicArray<T>& C_imag, 
			       const NTuple& dims) {
  C_real = BasicArray<T>(dims.forceOne(dim));
  C_imag = BasicArray<T>(dims.forceOne(dim));
  ConstBaseIterator<Atype,T> m_source_real(&A_real,dim);
  ConstBaseIterator<Atype,T> m_source_imag(&A_imag,dim);
  BaseIterator<BasicArray<T>,T> m_dest_real(&C_real,dim);
  BaseIterator<BasicArray<T>,T> m_dest_imag(&C_imag,dim);
  while (m_dest_real.isValid() && m_dest_imag.isValid() &&
	 m_source_real.isValid() && m_source_imag.isValid()) {
    T accum_real = Op::init_real();
    T accum_imag = Op::init_imag();
    for (int i=0;i<m_source_real.size();i++) {
      Op::func(accum_real,accum_imag,
	       m_source_real.get(),m_source_imag.get());
      m_source_real.next(); m_source_imag.next();
    }
    m_dest_real.set(accum_real); m_dest_imag.set(accum_imag);
    m_dest_real.nextSlice(); m_dest_imag.nextSlice();
    m_source_real.nextSlice(); m_source_imag.nextSlice();
  }
}

// Perform the reduction via a typed intermediary
template <typename T, class Op>
static inline Array ReductionOp(const Array &Ain, int dim, DataClass Tclass) {
  Array Acast(Ain.toClass(Tclass));
  Array F(Tclass);
  if (Acast.isSparse()) {
    if (Acast.allReal()) {
      F = ReductionOp<T,T,Op>(Acast.constRealSparse<T>(),dim);
    } else {
      ReductionOp<T,T,Op>(Acast.constRealSparse<T>(),
			  Acast.constImagSparse<T>(),
			  dim,
			  F.realSparse<T>(),
			  F.imagSparse<T>());
    }
    return F;
  }
  if (!Acast.isScalar()) Acast = Acast.asDenseArray();
  if (Acast.allReal()) {
    ReductionOp<T,BasicArray<T>,Op>
      (Acast.constReal<T>(),dim,F.real<T>(),Acast.dimensions());
  } else {
    ReductionOp<T,BasicArray<T>,Op>
      (Acast.constReal<T>(),Acast.constImag<T>(),dim,
       F.real<T>(),F.imag<T>(),Acast.dimensions());
  }
  return F;
}

// The main template for a reduction operation
// Requires an operator to apply as the reduction,
// and a dimension along which to apply it
template <class Op>
static inline Array ReductionOp(const Array &Ain, int dim) {
  if (Ain.dataClass() == Float)
    return ReductionOp<float,Op>(Ain,dim,Float).toClass(Ain.dataClass());
  else
    return ReductionOp<double,Op>(Ain,dim,Double).toClass(Ain.dataClass());
}

// Real --> Real
template <typename T, typename Atype, class Op>
static inline void SimpleFilterOp(const Atype& A, int dim,
				  BasicArray<T> &retvec, const NTuple& dims) {
  retvec = BasicArray<T>(dims);
  ConstBaseIterator<Atype,T> m_source(&A,dim);
  BaseIterator<BasicArray<T>,T> m_dest(&retvec,dim);
  while (m_dest.isValid() && m_source.isValid()) {
    T accum = Op::init();
    for (int i=0;i<m_source.size();i++) {
      accum = Op::func(accum,m_source.get());
      m_dest.set(accum);
      m_source.next();
      m_dest.next();
    }
    m_dest.nextSlice();
    m_source.nextSlice();
  }
}

// Complex --> Complex
template <typename T, typename Atype, class Op>
static inline void SimpleFilterOp(const Atype& A_real, const Atype& A_imag, int dim,
			    BasicArray<T>& C_real, BasicArray<T>& C_imag, 
			    const NTuple& dims) {
  C_real = BasicArray<T>(dims.forceOne(dim));
  C_imag = BasicArray<T>(dims.forceOne(dim));
  ConstBaseIterator<Atype,T> m_source_real(&A_real,dim);
  ConstBaseIterator<Atype,T> m_source_imag(&A_imag,dim);
  BaseIterator<BasicArray<T>,T> m_dest_real(&C_real,dim);
  BaseIterator<BasicArray<T>,T> m_dest_imag(&C_imag,dim);
  while (m_dest_real.isValid() && m_dest_imag.isValid() &&
	 m_source_real.isValid() && m_source_imag.isValid()) {
    T accum_real = Op::init_real();
    T accum_imag = Op::init_imag();
    for (int i=0;i<m_source_real.size();i++) {
      Op::func(accum_real,accum_imag,
	       m_source_real.get(),m_source_imag.get());
      m_dest_real.set(accum_real); m_dest_imag.set(accum_imag);
      m_source_real.next(); m_source_imag.next();
      m_dest_real.next(); m_dest_imag.next();
    }
    m_dest_real.nextSlice(); m_dest_imag.nextSlice();
    m_source_real.nextSlice(); m_source_imag.nextSlice();
  }
}

// Perform the filter via a typed intermediary
template <typename T, class Op>
static inline Array SimpleFilterOp(const Array &Ain, int dim, DataClass Tclass) {
  Array Acast(Ain.toClass(Tclass));
  Array F(Tclass);
  if (Acast.isSparse())
    throw Exception("Operation does not support sparse arrays");
  if (!Acast.isScalar()) Acast = Acast.asDenseArray();
  if (Acast.allReal()) {
    SimpleFilterOp<T,BasicArray<T>,Op>
      (Acast.constReal<T>(),dim,F.real<T>(),Acast.dimensions());
  } else {
    SimpleFilterOp<T,BasicArray<T>,Op>
      (Acast.constReal<T>(),Acast.constImag<T>(),dim,
       F.real<T>(),F.imag<T>(),Acast.dimensions());
  }
  return F;
}

template <class Op>
static inline Array SimpleFilterOp(const Array &Ain, int dim) {
  if (Ain.dataClass() == Float)
    return SimpleFilterOp<float,Op>(Ain,dim,Float).toClass(Ain.dataClass());
  else
    return SimpleFilterOp<double,Op>(Ain,dim,Double).toClass(Ain.dataClass());
}


template <typename T, class Op>
static inline Array VectorOp(const BasicArray<T> &real, index_t out, int dim) {
  NTuple outdims(real.dimensions()); outdims[dim] = out;
  BasicArray<T> F(outdims); 
  if (dim == 0) {
    ConstBasicIterator<T> source(&real,dim);
    BasicIterator<T> dest(&F.real<T>(),dim);
    while (source.isValid() && dest.isValid()) {
      Op::func(real.slice(source.pos()),F.slice(dest.pos()));
      source.nextSlice(); dest.nextSlice();
    }
  } else {
    ConstBasicIterator<T> source(&real,dim);
    BasicIterator<T> dest(&F.real<T>(),dim);
    while (source.isValid() && dest.isValid()) {
#error finishme      
    }
  }
}

template <typename T, class Op>
static inline Array VectorOp(const Array &Ain, index_t out, int dim, DataClass Tclass) {
  Array Acast(Ain.toClass(Tclass));
  Array F(Tclass);
  if (Acast.isSparse())
    throw Exception("Operation does not support sparse arrays");
  if (!Acast.isScalar()) Acast = Acast.asDenseArray();
  if (Acast.allReal()) {
    VectorOp<T,Op>(Acast.constReal<T>(),out,dim);
  } else {
    VectorOp<T,Op>(Acast.constReal<T>(),Acast.constImag<T>(),out,dim);
  }
}

template <class Op>
  static inline Array VectorOp(const Array &Ain, int out, int dim) {
  if (Ain.dataClass() == Float)
    return VectorOp<float,Op>(Ain,dim,Float).toClass(Ain.dataClass());
  else
    return VectorOp<double,Op>(Ain,dim,Double).toClass(Ain.dataClass());
}

#endif
