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
#include "LAPACK.hpp"
#include "LUDecompose.hpp"

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

int* FixupPivotVector(int nrows, int *piv, int p) {
  // Adjust the pivot vector
  int *fullpivot = (int*) malloc(sizeof(int)*nrows);
  int i;
  for (i=0;i<nrows;i++)
    fullpivot[i] = i;
  for (i=0;i<p;i++) 
    if (piv[i] != (i+1)) {
      int tmp = fullpivot[i];
      fullpivot[i] = fullpivot[piv[i]-1];
      fullpivot[piv[i]-1] = tmp;
    }
  free(piv);
  return fullpivot;
}

// In all cases, l is m x n  and   u is n x n if m > n
//               l is m x m  and   u is m x n if m < n
//           or  l is m x p  and   u is p x n

template <class T>
void RealLUP(int nrows, int ncols, T *l, T *u,
	     T *pmat, T *a, 
	     void (*lapack_fn)(int*,int*,T*,int*,int*,int*)) {
  int info;
  int i, j;
  int *piv;
  int p = min(nrows,ncols);
  piv = (int*) malloc(sizeof(int)*p);
  //  sgetrf_(&nrows,&ncols,a,&nrows,piv,&info);
  lapack_fn(&nrows,&ncols,a,&nrows,piv,&info);
  int *fullpivot = FixupPivotVector(nrows,piv,p);
  for (i=0;i<nrows;i++)
    pmat[i+fullpivot[i]*nrows] = 1;
  free(fullpivot);
  int lrows, lcols, urows, ucols;
  if (nrows > ncols) {
    lrows = nrows; lcols = ncols; urows = ncols; ucols = ncols;
    // If A is tall (more rows than columns), then we fill in the 
    // diagonal of L with 1s
    for (i=0;i<lcols;i++)
      l[i+i*lrows] = 1.0;
    // Then we copy the decomposition out of A (everything below
    // the main diagonal)
    for (i=1;i<lrows;i++)
      for (j=0;j<min(i,lcols);j++)
	l[i+j*lrows] = a[i+j*nrows];
    // And we copy the U matrix out of A (everything above and 
    // including the main diagonal).
    for (i=0;i<urows;i++)
      for (j=i;j<ucols;j++)
	u[i+j*urows] = a[i+j*nrows];
  } else {
    lrows = nrows; lcols = nrows; urows = nrows; ucols = ncols;
    // If A is short (more columns than rows) then L is square
    // and U is short.  We fill in the main diagonal of L with
    // 1s.
    for (i=0;i<lcols;i++)
      l[i+i*lrows] = 1.0;
    // Then we copy the decomposition out of A (everything below
    // the main diagonal
    for (i=1;i<lrows;i++)
      for (j=0;j<i;j++)
	l[i+j*lrows] = a[i+j*nrows];
    // Then we copy the remainder (upper trapezoid) out of A into
    // U
    for (i=0;i<nrows;i++)
      for (j=i;j<ncols;j++)
	u[i+j*urows] = a[i+j*nrows];
  }
}

template <class T>
void RealLU(int nrows, int ncols, T *l, T *u, T*a, 
	    void (*lapack_fn)(int*,int*,T*,int*,int*,int*)) {
  int info;
  int i, j;
  int *piv;
  int p = min(nrows,ncols);
  piv = (int*) malloc(sizeof(int)*p);
  lapack_fn(&nrows,&ncols,a,&nrows,piv,&info);
  //  dgetrf_(&nrows,&ncols,a,&nrows,piv,&info);
  // Adjust the pivot vector
  int *fullpivot = FixupPivotVector(nrows,piv,p);
  int lrows, lcols, urows, ucols;
  if (nrows > ncols) {
    lrows = nrows; lcols = ncols; urows = ncols; ucols = ncols;
    // If A is tall (more rows than columns), then we fill in the 
    // diagonal of L with 1s
    for (i=0;i<lcols;i++)
      l[fullpivot[i]+i*lrows] = 1.0;
    // Then we copy the decomposition out of A (everything below
    // the main diagonal)
    for (i=1;i<lrows;i++)
      for (j=0;j<min(i,lcols);j++)
	l[fullpivot[i]+j*lrows] = a[i+j*nrows];
    // And we copy the U matrix out of A (everything above and 
    // including the main diagonal).
    for (i=0;i<urows;i++)
      for (j=i;j<ucols;j++)
	u[i+j*urows] = a[i+j*nrows];
  } else {
    lrows = nrows; lcols = nrows; urows = nrows; ucols = ncols;
    // If A is short (more columns than rows) then L is square
    // and U is short.  We fill in the main diagonal of L with
    // 1s.
    for (i=0;i<lcols;i++)
      l[fullpivot[i]+i*lrows] = 1.0;
    // Then we copy the decomposition out of A (everything below
    // the main diagonal
    for (i=1;i<lrows;i++)
      for (j=0;j<i;j++)
	l[fullpivot[i]+j*lrows] = a[i+j*nrows];
    // Then we copy the remainder (upper trapezoid) out of A into
    // U
    for (i=0;i<nrows;i++)
      for (j=i;j<ncols;j++)
	u[i+j*urows] = a[i+j*nrows];
  }
  free(fullpivot);
}

template <class T>
void ComplexLUP(int nrows, int ncols, T *l, T *u,
		T *pmat, T *a, 
		void (*lapack_fn)(int*,int*,T*,int*,int*,int*)) {
  int info;
  int i, j;
  int *piv;
  int p = min(nrows,ncols);
  piv = (int*) malloc(sizeof(int)*p);
  lapack_fn(&nrows,&ncols,a,&nrows,piv,&info);
  int *fullpivot = FixupPivotVector(nrows,piv,p);
  for (i=0;i<nrows;i++)
    pmat[i+fullpivot[i]*nrows] = 1;
  free(fullpivot);
  int lrows, lcols, urows, ucols;
  if (nrows > ncols) {
    lrows = nrows; lcols = ncols; urows = ncols; ucols = ncols;
    // If A is tall (more rows than columns), then we fill in the 
    // diagonal of L with 1s
    for (i=0;i<lcols;i++) {
      l[2*(i+i*lrows)] = 1.0;
      l[2*(i+i*lrows)+1] = 0.0;
    }
    // Then we copy the decomposition out of A (everything below
    // the main diagonal)
    for (i=1;i<lrows;i++)
      for (j=0;j<min(i,lcols);j++) {
	l[2*(i+j*lrows)] = a[2*(i+j*nrows)];
	l[2*(i+j*lrows)+1] = a[2*(i+j*nrows)+1];
      }
    // And we copy the U matrix out of A (everything above and 
    // including the main diagonal).
    for (i=0;i<urows;i++)
      for (j=i;j<ucols;j++) {
	u[2*(i+j*urows)] = a[2*(i+j*nrows)];
	u[2*(i+j*urows)+1] = a[2*(i+j*nrows)+1];
      }
  } else {
    lrows = nrows; lcols = nrows; urows = nrows; ucols = ncols;
    // If A is short (more columns than rows) then L is square
    // and U is short.  We fill in the main diagonal of L with
    // 1s.
    for (i=0;i<lcols;i++) {
      l[2*(i+i*lrows)] = 1.0;
      l[2*(i+i*lrows)+1] = 0.0;
    }
    // Then we copy the decomposition out of A (everything below
    // the main diagonal
    for (i=1;i<lrows;i++)
      for (j=0;j<i;j++) {
	l[2*(i+j*lrows)] = a[2*(i+j*nrows)];
	l[2*(i+j*lrows)+1] = a[2*(i+j*nrows)+1];
      }
    // Then we copy the remainder (upper trapezoid) out of A into
    // U
    for (i=0;i<nrows;i++)
      for (j=i;j<ncols;j++) {
	u[2*(i+j*urows)] = a[2*(i+j*nrows)];
	u[2*(i+j*urows)+1] = a[2*(i+j*nrows)+1];
      }
  }
}

template <class T>
void ComplexLU(int nrows, int ncols, T *l, T *u, T*a, 
	       void (*lapack_fn)(int*,int*,T*,int*,int*,int*)) {
  int info;
  int i, j;
  int *piv;
  int p = min(nrows,ncols);
  piv = (int*) malloc(sizeof(int)*p);
  lapack_fn(&nrows,&ncols,a,&nrows,piv,&info);
  // Adjust the pivot vector
  int *fullpivot = FixupPivotVector(nrows,piv,p);
  int lrows, lcols, urows, ucols;
  if (nrows > ncols) {
    lrows = nrows; lcols = ncols; urows = ncols; ucols = ncols;
    // If A is tall (more rows than columns), then we fill in the 
    // diagonal of L with 1s
    for (i=0;i<lcols;i++) {
      l[2*(fullpivot[i]+i*lrows)] = 1.0;
      l[2*(fullpivot[i]+i*lrows)+1] = 0.0;
    }
    // Then we copy the decomposition out of A (everything below
    // the main diagonal)
    for (i=1;i<lrows;i++)
      for (j=0;j<min(i,lcols);j++) {
	l[2*(fullpivot[i]+j*lrows)] = a[2*(i+j*nrows)];
	l[2*(fullpivot[i]+j*lrows)+1] = a[2*(i+j*nrows)+1];
      }
    // And we copy the U matrix out of A (everything above and 
    // including the main diagonal).
    for (i=0;i<urows;i++)
      for (j=i;j<ucols;j++) {
	u[2*(i+j*urows)] = a[2*(i+j*nrows)];
	u[2*(i+j*urows)+1] = a[2*(i+j*nrows)+1];
      }
  } else {
    lrows = nrows; lcols = nrows; urows = nrows; ucols = ncols;
    // If A is short (more columns than rows) then L is square
    // and U is short.  We fill in the main diagonal of L with
    // 1s.
    for (i=0;i<lcols;i++) {
      l[2*(fullpivot[i]+i*lrows)] = 1.0;
      l[2*(fullpivot[i]+i*lrows)+1] = 0.0;
    }
    // Then we copy the decomposition out of A (everything below
    // the main diagonal
    for (i=1;i<lrows;i++)
      for (j=0;j<i;j++) {
	l[2*(fullpivot[i]+j*lrows)] = a[2*(i+j*nrows)];
	l[2*(fullpivot[i]+j*lrows)+1] = a[2*(i+j*nrows)+1];
      }
    // Then we copy the remainder (upper trapezoid) out of A into
    // U
    for (i=0;i<nrows;i++)
      for (j=i;j<ncols;j++) {
	u[2*(i+j*urows)] = a[2*(i+j*nrows)];
	u[2*(i+j*urows)+1] = a[2*(i+j*nrows)+1];
      }
  }
  free(fullpivot);
}


ArrayVector LUDecompose(int nargout, Array A) {
  if (nargout > 3)
    throw Exception("illegal usage of lu function - for dense matrices, maximum number of outputs is 3");
  int nrows = int(A.rows());
  int ncols = int(A.columns());
  int p = min(nrows,ncols);
  ArrayVector retval;
  if (A.isSparse())
    throw Exception("lu function does not work on sparse inputs");
  A = A.asDenseArray();
  switch (A.dataClass()) {
  default:
    throw Exception("unhandled type for lu function");
  case Float:
    {
      if (A.allReal()) {
	BasicArray<float> l(NTuple(nrows,p));
	BasicArray<float> u(NTuple(p,ncols));
	if (nargout <= 2) {
	  RealLU<float>(nrows,ncols,l.data(),u.data(),A.real<float>().data(),sgetrf_);
	  retval.push_back(Array(l));
	  retval.push_back(Array(u));
	} else if (nargout == 3) {
	  BasicArray<float> piv(NTuple(nrows,ncols));
	  RealLUP<float>(nrows,ncols,l.data(),u.data(),piv.data(),A.real<float>().data(),sgetrf_);
	  retval.push_back(Array(l));
	  retval.push_back(Array(u));
	  retval.push_back(Array(piv));
	}
	return retval;
      } else {
	BasicArray<float> l(NTuple(2*nrows,p));
	BasicArray<float> u(NTuple(2*p,ncols));
	if (nargout <= 2) {
	  ComplexLU<float>(nrows,ncols,l.data(),u.data(),
			   A.fortran<float>().data(),cgetrf_);
	  retval.push_back(Array(SplitReal(l),SplitImag(l)));
	  retval.push_back(Array(SplitReal(u),SplitImag(u)));
	} else if (nargout == 3) {
	  BasicArray<float> piv(NTuple(nrows,nrows));
	  ComplexLUP<float>(nrows,ncols,l.data(),u.data(),piv.data(),
			    A.fortran<float>().data(),cgetrf_);
	  retval.push_back(Array(SplitReal(l),SplitImag(l)));
	  retval.push_back(Array(SplitReal(u),SplitImag(u)));
	  retval.push_back(Array(piv));
	}
      }
    }
  case Double: 
    {
      if (A.allReal()) {
	BasicArray<double> l(NTuple(nrows,p));
	BasicArray<double> u(NTuple(p,ncols));
	if (nargout <= 2) {
	  RealLU<double>(nrows,ncols,l.data(),u.data(),A.real<double>().data(),dgetrf_);
	  retval.push_back(Array(l));
	  retval.push_back(Array(u));
	} else if (nargout == 3) {
	  BasicArray<double> piv(NTuple(nrows,ncols));
	  RealLUP<double>(nrows,ncols,l.data(),u.data(),piv.data(),
			  A.real<double>().data(),dgetrf_);
	  retval.push_back(Array(l));
	  retval.push_back(Array(u));
	  retval.push_back(Array(piv));
	}
	return retval;
      } else {
	BasicArray<double> l(NTuple(2*nrows,p));
	BasicArray<double> u(NTuple(2*p,ncols));
	if (nargout <= 2) {
	  ComplexLU<double>(nrows,ncols,l.data(),u.data(),
			    A.fortran<double>().data(),zgetrf_);
	  retval.push_back(Array(SplitReal(l),SplitImag(l)));
	  retval.push_back(Array(SplitReal(u),SplitImag(u)));
	} else if (nargout == 3) {
	  BasicArray<double> piv(NTuple(nrows,nrows));
	  ComplexLUP<double>(nrows,ncols,l.data(),u.data(),piv.data(),
			     A.fortran<double>().data(),zgetrf_);
	  retval.push_back(Array(SplitReal(l),SplitImag(l)));
	  retval.push_back(Array(SplitReal(u),SplitImag(u)));
	  retval.push_back(Array(piv));
	}
      }
    }
  }
  return retval;
}

template <typename T>
static Array InvertMatrixReal(const BasicArray<T> &A,
			      void (*getrf)(int*,int*,T*,int*,int*,int*),
			      void (*getri)(int*,T*,int*,int*,T*,int*,int*)) {
  int M = int(A.rows());
  int N = int(A.columns());
  BasicArray<T> Acopy(A);
  int LDA = M;
  BasicArray<int> IPIV(NTuple(qMin(M,N),1));
  int INFO;
  // Do the decomposition
  getrf(&M,&N,Acopy.data(),&LDA,IPIV.data(),&INFO);
  // Compute the inverse
  T WORKSIZE;
  int IWORKSIZE = -1;
  getri(&N,Acopy.data(),&LDA,IPIV.data(),&WORKSIZE,&IWORKSIZE,&INFO);
  IWORKSIZE = int(WORKSIZE);
  BasicArray<T> WORK(NTuple(IWORKSIZE,1));
  getri(&N,Acopy.data(),&LDA,IPIV.data(),WORK.data(),&IWORKSIZE,&INFO);
  return Array(Acopy);
}

template <typename T>
static Array InvertMatrixComplex(const BasicArray<T> &A,
				 void (*getrf)(int*,int*,T*,int*,int*,int*),
				 void (*getri)(int*,T*,int*,int*,T*,int*,int*)) {
  int M = int(A.rows()/2);
  int N = int(A.columns());
  BasicArray<T> Acopy(A);
  int LDA = M;
  BasicArray<int> IPIV(NTuple(qMin(M,N),1));
  int INFO;
  // Do the decomposition
  getrf(&M,&N,Acopy.data(),&LDA,IPIV.data(),&INFO);
  // Compute the inverse
  T WORKSIZE[2];
  int IWORKSIZE = -1;
  getri(&N,Acopy.data(),&LDA,IPIV.data(),WORKSIZE,&IWORKSIZE,&INFO);
  IWORKSIZE = int(WORKSIZE[0]);
  BasicArray<T> WORK(NTuple(IWORKSIZE*2,1));
  getri(&N,Acopy.data(),&LDA,IPIV.data(),WORK.data(),&IWORKSIZE,&INFO);
  return Array(SplitReal<T>(Acopy),SplitImag<T>(Acopy));
}

Array Invert(const Array &A) {
  if (A.isSparse()) 
    throw Exception("Sparse matrix inverse not currently supported");
  switch (A.dataClass()) {
  default: throw Exception("Invert does not support this data class");
  case Float:
    if (A.allReal())
      return InvertMatrixReal(A.constReal<float>(),sgetrf_,sgetri_);
    else
      return InvertMatrixComplex(A.fortran<float>(),cgetrf_,cgetri_);
  case Double:
    if (A.allReal())
      return InvertMatrixReal(A.constReal<double>(),dgetrf_,dgetri_);
    else
      return InvertMatrixComplex(A.fortran<double>(),zgetrf_,zgetri_);
  }
}

static ArrayVector SparseLUDecomposeReal(const SparseMatrix<double> &A) {
#if 0
  //#if HAVE_UMFPACK
  // Convert A into CCS form
  CCSForm Accs;
  ConvertSparseToCCS(A,Accs);
  double *null = (double *) NULL ;
  void *Symbolic, *Numeric ;
  (void) umfpack_di_symbolic(int(A.cols()), int(A.cols()), Accs.colstart(), 
			      Accs.rowindx(), Accs.data(), &Symbolic, null, null);
  (void) umfpack_di_numeric(Accs.colstart(), Accs.rowindx(), Accs.data(), Symbolic, 
			    &Numeric, null, null);

  // Set up the output arrays for the LU Decomposition.
  // The first matrix is L, which is stored in comprssed row form.
  int lnz;
  int unz;
  int n_row;
  int n_col;
  int nz_udiag;

  (void) umfpack_di_get_lunz(&lnz,&unz,&n_row,&n_col,&nz_udiag,Numeric);

  MemBlock<int> Lpb(Arows+1); int *Lp = &Lpb;
  MemBlock<int> Ljb(lnz); int *Lj = &Ljb;
  MemBlock<double> Lxb(lnz); double *Lx = &Lxb;
  MemBlock<int> Upb(Acols+1); int *Up = &Upb;
  MemBlock<int> Uib(unz); double *Ui = &Uib;
  MemBlock<double> Uxb(unz); double *Ux = &Uxb;
  MemBlock<int> Pb(Arows); int *P = &Pb;
  MemBlock<int> Qb(Acols); int *Q = &Qb;
  MemBlock<double> Rsb(Arows); double *Rs = &Rsb;
    
  int do_recip;
  umfpack_di_get_numeric(Lp, Lj, Lx, Up, Ui, Ux, P, Q, NULL, &do_recip, Rs, Numeric);
#error FinishMe
  for (int i=0;i<Arows;i++) P[i]++;
  for (int i=0;i<Acols;i++) Q[i]++;

  IJVEntry<double>* llist = ConvertCCSToIJVListReal(Lp,Lj,Lx,Arows,lnz);
  for (int j=0;j<lnz;j++) {
    int tmp;
    tmp = llist[j].I;
    llist[j].I = llist[j].J;
    llist[j].J = tmp;
  }
  IJVEntry<double>* ulist = ConvertCCSToIJVListReal(Up,Ui,Ux,Acols,unz);
  IJVEntry<double>* rlist = new IJVEntry<double>[Arows];
  std::sort(llist,llist+lnz);
  for (int i=0;i<Arows;i++) {
    rlist[i].I = i;
    rlist[i].J = i;
    if (do_recip)
      rlist[i].Vreal = Rs[i];
    else
      rlist[i].Vreal = 1.0/Rs[i];
  }
  ArrayVector retval;
  // Push L, U, P, Q, R
  int Amid;
  Amid = (Arows < Acols) ? Arows : Acols;
  retval.push_back(Array(FM_DOUBLE,Dimensions(Arows,Amid),
			 ConvertIJVtoRLEReal<double>(llist,lnz,Arows,Amid),true));
  retval.push_back(Array(FM_DOUBLE,Dimensions(Amid,Acols),
			 ConvertIJVtoRLEReal<double>(ulist,unz,Amid,Acols),true));
  retval.push_back(Array(FM_INT32,Dimensions(1,Arows),P,false));
  retval.push_back(Array(FM_INT32,Dimensions(1,Acols),Q,false));
  retval.push_back(Array(FM_DOUBLE,Dimensions(Arows,Arows),
			 ConvertIJVtoRLEReal<double>(rlist,Arows,Arows,Arows),true));
  umfpack_di_free_symbolic(&Symbolic);
  umfpack_di_free_numeric(&Numeric);
  delete[] rlist;
  delete[] ulist;
  delete[] llist;
  delete[] Rs;
  delete[] Ux;
  delete[] Ui;
  delete[] Up;
  delete[] Lx;
  delete[] Lj;
  delete[] Lp;
  delete[] Acolstart;
  delete[] Arowindx;
  delete[] Adata;
  return retval;
#else
  throw Exception("LU Decompositions of sparse matrices requires UMFPACK support, which was not available at compile time.  You must have UMFPACK installed at compile time for FreeMat to enable this functionality.");
#endif
}

static ArrayVector SparseLUDecomposeComplex(const SparseMatrix<double> &Ar, const SparseMatrix<double> &Ai) {
#if 0
#warning finishme
  //#if HAVE_UMFPACK
  CCSForm Accs;
  ConvertSparseToCCS(Ar,Ai,Accs);
  double *null = (double *) NULL ;
  void *Symbolic, *Numeric ;
  (void) umfpack_zi_symbolic (int(Ar.cols()), int(Ar.cols()), Accs.colstart(), Accs.rowindx(), 
			      Accs.data(), Accs.imag(), &Symbolic, null, null);
  (void) umfpack_zi_numeric (Accs.colstart(), Accs.rowindx(), Accs.data(), Accs.imag(), 
			     Symbolic, &Numeric, null, null);
  // Set up the output arrays for the LU Decomposition.
  // The first matrix is L, which is stored in comprssed row form.
  int lnz;
  int unz;
  int n_row;
  int n_col;
  int nz_udiag;

  (void) umfpack_zi_get_lunz(&lnz,&unz,&n_row,&n_col,&nz_udiag,Numeric);

  MemBlock<int> Lpb(Arows+1); int *Lp = &Lpb;
  MemBlock<int> Ljb(lnz); int *Lj = &Ljb;
  MemBlock<double> Lxb(lnz); double *Lx = &Lxb;
  MemBlock<double> Lyb(lnz); double *Ly = &Lyb;
  MemBlock<int> Upb(Acols+1); int *Up = &Upb;
  MemBlock<int> Uib(unz); double *Ui = &Uib;
  MemBlock<double> Uxb(unz); double *Ux = &Uxb;
  MemBlock<double> Uyb(unz); double *Uy = &Uyb;
  MemBlock<int> Pb(Arows); int *P = &Pb;
  MemBlock<int> Qb(Acols); int *Q = &Qb;
  MemBlock<double> Rsb(Arows); double *Rs = &Rsb;
    
  int do_recip;
  umfpack_zi_get_numeric(Lp, Lj, Lx, Ly, Up, Ui, Ux, Uy, P, Q, NULL, NULL, &do_recip, Rs, Numeric);

#error FinishMe
  for (int i=0;i<Arows;i++)
    P[i]++;

  for (int i=0;i<Acols;i++)
    Q[i]++;

  IJVEntry<double>* llist = ConvertCCSToIJVListComplex(Lp,Lj,Lx,Ly,Arows,lnz);
  for (int j=0;j<lnz;j++) {
    int tmp;
    tmp = llist[j].I;
    llist[j].I = llist[j].J;
    llist[j].J = tmp;
  }
  IJVEntry<double>* ulist = ConvertCCSToIJVListComplex(Up,Ui,Ux,Uy,Acols,unz);
  IJVEntry<double>* rlist = new IJVEntry<double>[Arows];
  std::sort(llist,llist+lnz);
  for (int i=0;i<Arows;i++) {
    rlist[i].I = i;
    rlist[i].J = i;
    if (do_recip)
      rlist[i].Vreal = Rs[i];
    else
      rlist[i].Vreal = 1.0/Rs[i];
  }
  ArrayVector retval;
  // Push L, U, P, Q, R
  int Amid;
  Amid = (Arows < Acols) ? Arows : Acols;
  retval.push_back(Array(FM_DCOMPLEX,Dimensions(Arows,Amid),
			 ConvertIJVtoRLEComplex<double>(llist,lnz,Arows,Amid),true));
  retval.push_back(Array(FM_DCOMPLEX,Dimensions(Amid,Acols),
			 ConvertIJVtoRLEComplex<double>(ulist,unz,Amid,Acols),true));
  retval.push_back(Array(FM_INT32,Dimensions(1,Arows),P,false));
  retval.push_back(Array(FM_INT32,Dimensions(1,Acols),Q,false));
  retval.push_back(Array(FM_DOUBLE,Dimensions(Arows,Arows),
			 ConvertIJVtoRLEReal<double>(rlist,Arows,Arows,Arows),true));
  umfpack_di_free_symbolic(&Symbolic);
  umfpack_di_free_numeric(&Numeric);
  delete[] rlist;
  delete[] ulist;
  delete[] llist;
  delete[] Rs;
  delete[] Uy;
  delete[] Ux;
  delete[] Ui;
  delete[] Up;
  delete[] Ly;
  delete[] Lx;
  delete[] Lj;
  delete[] Lp;
  delete[] Acolstart;
  delete[] Arowindx;
  delete[] Adata;
  return retval;
#else
  throw Exception("LU Decompositions of sparse matrices requires UMFPACK support, which was not available at compile time.  You must have UMFPACK installed at compile time for FreeMat to enable this functionality.");
#endif
}

  
ArrayVector SparseLUDecompose(int nargout, Array A) {
  if (A.dataClass() != Double)
    throw Exception("FreeMat currently only supports the LU decomposition for double and dcomplex matrices");
  int Arows = int(A.rows());
  int Acols = int(A.cols());
  if (!A.isSquare())
    throw Exception("FreeMat currently only supports LU decompositions for square matrices");
  if (A.allReal())
    return SparseLUDecomposeReal(A.constRealSparse<double>());
  else
    return SparseLUDecomposeComplex(A.constRealSparse<double>(), A.constImagSparse<double>());
}

