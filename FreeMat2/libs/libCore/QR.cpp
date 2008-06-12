ArrayVector QRDNoPivotFunction(bool compactDec, Array A) {
  int nrows = A.getDimensionLength(0);
  int ncols = A.getDimensionLength(1);
  int orows = nrows;
  int ocols = ncols;
  if ((!compactDec) && (nrows > ncols)) {
    Dimensions newDim(nrows,nrows);
    A.resize(newDim);
    ncols = nrows;
  } else 
    compactDec = true;
  Class Aclass(A.dataClass());
  if (Aclass < FM_FLOAT) {
    A.promoteType(FM_DOUBLE);
    Aclass = FM_DOUBLE;
  }
  int minmn = (nrows < ncols) ? nrows : ncols;
  ArrayVector retval;
  Array rmat, qmat;
  Dimensions dim;
  switch (Aclass) {
  default: throw Exception("illegal argument type to QRD");
  case FM_FLOAT:
    {
      float *q = (float*) Malloc(nrows*minmn*sizeof(float));
      float *r = (float*) Malloc(ncols*minmn*sizeof(float));
      floatQRD(nrows,ncols,q,r,(float*) A.getReadWriteDataPointer());
      if (!compactDec) {
	float *r2 = (float*) Malloc(orows*ocols*sizeof(float));
	memcpy(r2,r,orows*ocols*sizeof(float));
	dim.set(0,orows);
	dim.set(1,ocols);
	rmat = Array(FM_FLOAT,dim,r2);
	Free(r);
      } else {	  
	dim.set(0,minmn);
	dim.set(1,ncols);
	rmat = Array(FM_FLOAT,dim,r);
      }
      dim.set(0,nrows);
      dim.set(1,minmn);
      qmat = Array(FM_FLOAT,dim,q);
      retval.push_back(qmat);
      retval.push_back(rmat);
      break;
    }
  case FM_DOUBLE:
    {
      double *q = (double*) Malloc(nrows*minmn*sizeof(double));
      double *r = (double*) Malloc(ncols*minmn*sizeof(double));
      doubleQRD(nrows,ncols,q,r,(double*) A.getReadWriteDataPointer());
      if (!compactDec) {
	double *r2 = (double*) Malloc(orows*ocols*sizeof(double));
	memcpy(r2,r,orows*ocols*sizeof(double));
	dim.set(0,orows);
	dim.set(1,ocols);
	rmat = Array(FM_DOUBLE,dim,r2);
	Free(r);
      } else {	  
	dim.set(0,minmn);
	dim.set(1,ncols);
	rmat = Array(FM_DOUBLE,dim,r);
      }
      dim.set(0,nrows);
      dim.set(1,minmn);
      qmat = Array(FM_DOUBLE,dim,q);
      retval.push_back(qmat);
      retval.push_back(rmat);
      break;
    }
  case FM_COMPLEX:
    {
      float *q = (float*) Malloc(2*nrows*minmn*sizeof(float));
      float *r = (float*) Malloc(2*ncols*minmn*sizeof(float));
      complexQRD(nrows,ncols,q,r,(float*) A.getReadWriteDataPointer());
      if (!compactDec) {
	float *r2 = (float*) Malloc(2*orows*ocols*sizeof(float));
	memcpy(r2,r,2*orows*ocols*sizeof(float));
	dim.set(0,orows);
	dim.set(1,ocols);
	rmat = Array(FM_COMPLEX,dim,r2);
	Free(r);
      } else {	  
	dim.set(0,minmn);
	dim.set(1,ncols);
	rmat = Array(FM_COMPLEX,dim,r);
      }
      dim.set(0,nrows);
      dim.set(1,minmn);
      qmat = Array(FM_COMPLEX,dim,q);
      retval.push_back(qmat);
      retval.push_back(rmat);
      break;
    }
  case FM_DCOMPLEX:
    {
      double *q = (double*) Malloc(2*nrows*minmn*sizeof(double));
      double *r = (double*) Malloc(2*ncols*minmn*sizeof(double));
      dcomplexQRD(nrows,ncols,q,r,(double*) A.getReadWriteDataPointer());
      if (!compactDec) {
	double *r2 = (double*) Malloc(2*orows*ocols*sizeof(double));
	memcpy(r2,r,2*orows*ocols*sizeof(double));
	dim.set(0,orows);
	dim.set(1,ocols);
	rmat = Array(FM_DCOMPLEX,dim,r2);
	Free(r);
      } else {	  
	dim.set(0,minmn);
	dim.set(1,ncols);
	rmat = Array(FM_DCOMPLEX,dim,r);
      }
      dim.set(0,nrows);
      dim.set(1,minmn);
      qmat = Array(FM_DCOMPLEX,dim,q);
      retval.push_back(qmat);
      retval.push_back(rmat);
      break;
    }
  }
  return retval;
}

ArrayVector QRDPivotFunction(bool compactDec, Array A) {
  int nrows = A.getDimensionLength(0);
  int ncols = A.getDimensionLength(1);
  int orows = nrows;
  int ocols = ncols;
  int i;
  bool compactSav = compactDec;
  if ((!compactDec) && (nrows > ncols)) {
    Dimensions newDim(nrows,nrows);
    A.resize(newDim);
    ncols = nrows;
  } else 
    compactDec = true;
  Class Aclass(A.dataClass());
  if (Aclass < FM_FLOAT) {
    A.promoteType(FM_DOUBLE);
    Aclass = FM_DOUBLE;
  }
  int minmn = (nrows < ncols) ? nrows : ncols;
  ArrayVector retval;
  Array rmat, qmat, pmat;
  Dimensions dim;
  switch (Aclass) {
  default: throw Exception("illegal argument type to QRD");
  case FM_FLOAT:
    {
      float *q = (float*) Malloc(nrows*minmn*sizeof(float));
      float *r = (float*) Malloc(ncols*minmn*sizeof(float));
      int *p = (int*) Malloc(ncols*sizeof(int));
      floatQRDP(nrows,ncols,q,r,p,(float*) A.getReadWriteDataPointer());
      if (!compactDec) {
	float *r2 = (float*) Malloc(orows*ocols*sizeof(float));
	memcpy(r2,r,orows*ocols*sizeof(float));
	dim.set(0,orows);
	dim.set(1,ocols);
	rmat = Array(FM_FLOAT,dim,r2);
	Free(r);
      } else {	  
	dim.set(0,minmn);
	dim.set(1,ncols);
	rmat = Array(FM_FLOAT,dim,r);
      }
      if (!compactSav) {
	int *p2 = (int*) Malloc(ncols*ncols*sizeof(int));
	for (i=0;i<ncols;i++) 
	  p2[p[i] + i*ncols - 1] = 1;
	dim.set(0,ncols);
	dim.set(1,ncols);
	pmat = Array(FM_INT32,dim,p2);
	Free(p);
      } else {
	dim.set(0,1);
	dim.set(1,ncols);
	pmat = Array(FM_INT32,dim,p);
      }
      dim.set(0,nrows);
      dim.set(1,minmn);
      qmat = Array(FM_FLOAT,dim,q);
      retval.push_back(qmat);
      retval.push_back(rmat);
      retval.push_back(pmat);
      break;
    }
  case FM_DOUBLE:
    {
      double *q = (double*) Malloc(nrows*minmn*sizeof(double));
      double *r = (double*) Malloc(ncols*minmn*sizeof(double));
      int *p = (int*) Malloc(ncols*sizeof(int));
      doubleQRDP(nrows,ncols,q,r,p,(double*) A.getReadWriteDataPointer());
      if (!compactDec) {
	double *r2 = (double*) Malloc(orows*ocols*sizeof(double));
	memcpy(r2,r,orows*ocols*sizeof(double));
	dim.set(0,orows);
	dim.set(1,ocols);
	rmat = Array(FM_DOUBLE,dim,r2);
	Free(r);
      } else {	  
	dim.set(0,minmn);
	dim.set(1,ncols);
	rmat = Array(FM_DOUBLE,dim,r);
      }
      if (!compactSav) {
	int *p2 = (int*) Malloc(ncols*ncols*sizeof(int));
	for (i=0;i<ncols;i++) 
	  p2[p[i] + i*ncols - 1] = 1;
	dim.set(0,ncols);
	dim.set(1,ncols);
	pmat = Array(FM_INT32,dim,p2);
	Free(p);
      } else {
	dim.set(0,1);
	dim.set(1,ncols);
	pmat = Array(FM_INT32,dim,p);
      }
      dim.set(0,nrows);
      dim.set(1,minmn);
      qmat = Array(FM_DOUBLE,dim,q);
      retval.push_back(qmat);
      retval.push_back(rmat);
      retval.push_back(pmat);
      break;
    }
  case FM_COMPLEX:
    {
      float *q = (float*) Malloc(2*nrows*minmn*sizeof(float));
      float *r = (float*) Malloc(2*ncols*minmn*sizeof(float));
      int *p = (int*) Malloc(ncols*sizeof(int));
      complexQRDP(nrows,ncols,q,r,p,(float*) A.getReadWriteDataPointer());
      if (!compactDec) {
	float *r2 = (float*) Malloc(2*orows*ocols*sizeof(float));
	memcpy(r2,r,2*orows*ocols*sizeof(float));
	dim.set(0,orows);
	dim.set(1,ocols);
	rmat = Array(FM_COMPLEX,dim,r2);
	Free(r);
      } else {	  
	dim.set(0,minmn);
	dim.set(1,ncols);
	rmat = Array(FM_COMPLEX,dim,r);
      }
      if (!compactSav) {
	int *p2 = (int*) Malloc(ncols*ncols*sizeof(int));
	for (i=0;i<ncols;i++) 
	  p2[p[i] + i*ncols - 1] = 1;
	dim.set(0,ncols);
	dim.set(1,ncols);
	pmat = Array(FM_INT32,dim,p2);
	Free(p);
      } else {
	dim.set(0,1);
	dim.set(1,ncols);
	pmat = Array(FM_INT32,dim,p);
      }
      dim.set(0,nrows);
      dim.set(1,minmn);
      qmat = Array(FM_COMPLEX,dim,q);
      retval.push_back(qmat);
      retval.push_back(rmat);
      retval.push_back(pmat);
      break;
    }
  case FM_DCOMPLEX:
    {
      double *q = (double*) Malloc(2*nrows*minmn*sizeof(double));
      double *r = (double*) Malloc(2*ncols*minmn*sizeof(double));
      int *p = (int*) Malloc(ncols*sizeof(int));
      dcomplexQRDP(nrows,ncols,q,r,p,(double*) A.getReadWriteDataPointer());
      if (!compactDec) {
	double *r2 = (double*) Malloc(2*orows*ocols*sizeof(double));
	memcpy(r2,r,2*orows*ocols*sizeof(double));
	dim.set(0,orows);
	dim.set(1,ocols);
	rmat = Array(FM_DCOMPLEX,dim,r2);
	Free(r);
      } else {	  
	dim.set(0,minmn);
	dim.set(1,ncols);
	rmat = Array(FM_DCOMPLEX,dim,r);
      }
      if (!compactSav) {
	int *p2 = (int*) Malloc(ncols*ncols*sizeof(int));
	for (i=0;i<ncols;i++) 
	  p2[p[i] + i*ncols - 1] = 1;
	dim.set(0,ncols);
	dim.set(1,ncols);
	pmat = Array(FM_INT32,dim,p2);
	Free(p);
      } else {
	dim.set(0,1);
	dim.set(1,ncols);
	pmat = Array(FM_INT32,dim,p);
      }
      dim.set(0,nrows);
      dim.set(1,minmn);
      qmat = Array(FM_DCOMPLEX,dim,q);
      retval.push_back(qmat);
      retval.push_back(rmat);
      retval.push_back(pmat);
      break;
    }
  }
  return retval;
}

//!
//@Module QR QR Decomposition of a Matrix
//@@Section TRANSFORMS
//@@Usage
//Computes the QR factorization of a matrix.  The @|qr| function has
//multiple forms, with and without pivoting.  The non-pivot version
//has two forms, a compact version and a full-blown decomposition
//version.  The compact version of the decomposition of a matrix 
//of size @|M x N| is
//@[
//  [q,r] = qr(a,0)
//@]
//where @|q| is a matrix of size @|M x L| and @|r| is a matrix of
//size @|L x N| and @|L = min(N,M)|, and @|q*r = a|.  The QR decomposition is
//such that the columns of @|Q| are orthonormal, and @|R| is upper
//triangular.  The decomposition is computed using the LAPACK 
//routine @|xgeqrf|, where @|x| is the precision of the matrix.  Unlike
//MATLAB (and other MATLAB-compatibles), FreeMat supports decompositions
//of all four floating point types, @|float, complex, double, dcomplex|.
//
//The second form of the non-pivot decomposition omits the second @|0|
//argument:
//@[
//  [q,r] = qr(a)
//@]
//This second form differs from the previous form only for matrices
//with more rows than columns (@|M > N|).  For these matrices, the
//full decomposition is of a matrix @|Q| of size @|M x M| and 
//a matrix @|R| of size @|M x N|.  The full decomposition is computed
//using the same LAPACK routines as the compact decomposition, but
//on an augmented matrix @|[a 0]|, where enough columns are added to
//form a square matrix.
//
//Generally, the QR decomposition will not return a matrix @|R| with
//diagonal elements in any specific order.  The remaining two forms 
//of the @|qr| command utilize permutations of the columns of @|a|
//so that the diagonal elements of @|r| are in decreasing magnitude.
//To trigger this form of the decomposition, a third argument is
//required, which records the permutation applied to the argument @|a|.
//The compact version is
//@[
//  [q,r,e] = qr(a,0)
//@]
//where @|e| is an integer vector that describes the permutation of
//the columns of @|a| necessary to reorder the diagonal elements of
//@|r|.  This result is computed using the LAPACK routines @|(s,d)geqp3|.
//In the non-compact version of the QR decomposition with pivoting,
//@[
//  [q,r,e] = qr(a)
//@]
//the returned matrix @|e| is a permutation matrix, such that 
//@|q*r*e' = a|.
//@@Tests
//@{ test_qr1.m
//% Test the QR decomposition (without pivoting, minimal decomposition, double)
//function test_val = test_qr1
//a = [1,2,3;4,5,6;7,8,9;10,0,5];
//% double precision
//[q,r] = qr(a,0);
//qorg = q'*q - diag(ones(1,3));
//t1 = (max(abs(qorg(:)))<1e-15);
//t2 = ((size(q,1) == 4) & (size(q,2) == 3));
//t3 = ((size(r,1) == 3) & (size(r,2) == 3));
//aorg = q*r - a;
//t4 = (max(abs(qorg(:)))<1e-15);
//t5 = max(diag(r,-1)) == 0;
//t6 = max(diag(r,-2)) == 0;
//test_val = test(t1) & test(t2) & test(t3) & test(t4) & test(t5) & test(t6);
//@}
//@{ test_qr2.m
//% Test the QR decomposition (without pivoting, minimal decomposition, float)
//function test_val = test_qr2
//a = float([1,2,3;4,5,6;7,8,9;10,0,5]);
//% float precision
//[q,r] = qr(a,0);
//qorg = q'*q - diag(ones(1,3));
//t1 = (max(abs(qorg(:)))<1e-6);
//t2 = ((size(q,1) == 4) & (size(q,2) == 3));
//t3 = ((size(r,1) == 3) & (size(r,2) == 3));
//aorg = q*r - a;
//t4 = (max(abs(qorg(:)))<1e-6);
//t5 = max(diag(r,-1)) == 0;
//t6 = max(diag(r,-2)) == 0;
//test_val = test(t1) & test(t2) & test(t3) & test(t4) & test(t5) & test(t6);
//@}
//@{ test_qr3.m
//% Test the QR decomposition (without pivoting, minimal decomposition, complex)
//function test_val = test_qr3
//a = float([1,2,3;4,5,6;7,8,9;10,0,5]);
//a = a + i*(a-1);
//% float precision
//[q,r] = qr(a,0);
//qorg = q'*q - diag(ones(1,3));
//t1 = (max(abs(qorg(:)))<1e-6);
//t2 = ((size(q,1) == 4) & (size(q,2) == 3));
//t3 = ((size(r,1) == 3) & (size(r,2) == 3));
//aorg = q*r - a;
//t4 = (max(abs(qorg(:)))<1e-6);
//t5 = max(diag(r,-1)) == 0;
//t6 = max(diag(r,-2)) == 0;
//test_val = test(t1) & test(t2) & test(t3) & test(t4) & test(t5) & test(t6);
//@}
//@{ test_qr4.m
//% Test the QR decomposition (without pivoting, minimal decomposition, dcomplex)
//function test_val = test_qr4
//a = double([1,2,3;4,5,6;7,8,9;10,0,5]);
//a = a + i*(a-1);
//% float precision
//[q,r] = qr(a,0);
//qorg = q'*q - diag(ones(1,3));
//t1 = (max(abs(qorg(:)))<1e-15);
//t2 = ((size(q,1) == 4) & (size(q,2) == 3));
//t3 = ((size(r,1) == 3) & (size(r,2) == 3));
//aorg = q*r - a;
//t4 = (max(abs(qorg(:)))<1e-15);
//t5 = max(diag(r,-1)) == 0;
//t6 = max(diag(r,-2)) == 0;
//test_val = test(t1) & test(t2) & test(t3) & test(t4) & test(t5) & test(t6);
//@}
//@{ test_qr5.m
//% Test the QR decomposition (without pivoting, double)
//function test_val = test_qr5
//a = [1,2,3;4,5,6;7,8,9;10,0,5];
//% double precision
//[q,r] = qr(a);
//qorg = q'*q - diag(ones(1,4));
//t1 = (max(abs(qorg(:)))<1e-15);
//t2 = ((size(q,1) == 4) & (size(q,2) == 4));
//t3 = ((size(r,1) == 4) & (size(r,2) == 3));
//aorg = q*r - a;
//t4 = (max(abs(qorg(:)))<1e-15);
//t5 = max(diag(r,-1)) == 0;
//t6 = max(diag(r,-2)) == 0;
//t7 = max(diag(r,-3)) == 0;
//test_val = test(t1) & test(t2) & test(t3) & test(t4) & test(t5) & test(t6) & test(t7);
//@}
//@{ test_qr6.m
//% Test the QR decomposition (without pivoting, float)
//function test_val = test_qr6
//a = float([1,2,3;4,5,6;7,8,9;10,0,5]);
//% float precision
//[q,r] = qr(a);
//qorg = q'*q - diag(ones(1,4));
//t1 = (max(abs(qorg(:)))<1e-6);
//t2 = ((size(q,1) == 4) & (size(q,2) == 4));
//t3 = ((size(r,1) == 4) & (size(r,2) == 3));
//aorg = q*r - a;
//t4 = (max(abs(qorg(:)))<1e-6);
//t5 = max(diag(r,-1)) == 0;
//t6 = max(diag(r,-2)) == 0;
//t7 = max(diag(r,-3)) == 0;
//test_val = test(t1) & test(t2) & test(t3) & test(t4) & test(t5) & test(t6) & test(t7);
//@}
//@{ test_qr7.m
//% Test the QR decomposition (without pivoting, complex)
//function test_val = test_qr7
//a = float([1,2,3;4,5,6;7,8,9;10,0,5]);
//a = a + i*(a-1);
//% float precision
//[q,r] = qr(a);
//qorg = q'*q - diag(ones(1,4));
//t1 = (max(abs(qorg(:)))<1e-6);
//t2 = ((size(q,1) == 4) & (size(q,2) == 4));
//t3 = ((size(r,1) == 4) & (size(r,2) == 3));
//aorg = q*r - a;
//t4 = (max(abs(qorg(:)))<1e-6);
//t5 = max(diag(r,-1)) == 0;
//t6 = max(diag(r,-2)) == 0;
//t7 = max(diag(r,-3)) == 0;
//test_val = test(t1) & test(t2) & test(t3) & test(t4) & test(t5) & test(t6) & test(t7);
//@}
//@{ test_qr8.m
//% Test the QR decomposition (without pivoting, dcomplex)
//function test_val = test_qr8
//a = double([1,2,3;4,5,6;7,8,9;10,0,5]);
//a = a + i*(a-1);
//% float precision
//[q,r] = qr(a);
//qorg = q'*q - diag(ones(1,4));
//t1 = (max(abs(qorg(:)))<1e-15);
//t2 = ((size(q,1) == 4) & (size(q,2) == 4));
//t3 = ((size(r,1) == 4) & (size(r,2) == 3));
//aorg = q*r - a;
//t4 = (max(abs(qorg(:)))<1e-15);
//t5 = max(diag(r,-1)) == 0;
//t6 = max(diag(r,-2)) == 0;
//t7 = max(diag(r,-3)) == 0;
//test_val = test(t1) & test(t2) & test(t3) & test(t4) & test(t5) & test(t6) & test(t7);
//@}
//@{ test_qr9.m
//% Test the QR decomposition (full, without pivoting)
//function test_val = test_qr9
//  t1all = 1;
//  for n=2:4:100
//    a = float(rand(n,2*n));
//    [q,r] = qr(a);
//    err = abs(a-q*r);
//    err = max(err(:));
//    bnd = 8*max(abs(diag(r)))*feps*(n^(0.5));
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;
//  end
//  t2all = 1;
//  for n=2:4:100
//    a = double(rand(n,2*n));
//    [q,r] = qr(a);
//    err = abs(a-q*r);
//    err = max(err(:));
//    bnd = 8*max(abs(diag(r)))*eps*(n^(0.5));
//    t2 = (err < bnd);
//    if (~t2) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t2all = t2all & t2;
//  end
//  t3all = 1;
//  for n=2:4:100
//    a = complex(rand(n,2*n)+i*rand(n,2*n));
//    [q,r] = qr(a);
//    err = abs(a-q*r);
//    err = max(err(:));
//    bnd = 8*max(abs(diag(r)))*feps*(n^(0.5));
//    t3 = (err < bnd);
//    if (~t3) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t3all = t3all & t3;
//  end
//  t4all = 1;
//  for n=2:4:100
//    a = dcomplex(rand(n,2*n)+i*rand(n,2*n));
//    [q,r] = qr(a);
//    err = abs(a-q*r);
//    err = max(err(:));
//    bnd = 8*max(abs(diag(r)))*eps*(n^(0.5));
//    t4 = (err < bnd);
//    if (~t4) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t4all = t4all & t4;
//  end
//  test_val = t1all & t2all & t3all & t4all;
//@}
//@{ test_qr10.m
//% Test the QR decomposition (compact, without pivoting)
//function test_val = test_qr10
//  t1all = 1;
//  for n=2:4:100
//    a = float(rand(2*n,n));
//    [q,r] = qr(a,0);
//    err = abs(a-q*r);
//    err = max(err(:));
//    bnd = 10*max(abs(diag(r)))*feps*(n^(0.5));
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;
//  end
//  t2all = 1;
//  for n=2:4:100
//    a = double(rand(2*n,n));
//    [q,r] = qr(a,0);
//    err = abs(a-q*r);
//    err = max(err(:));
//    bnd = 10*max(abs(diag(r)))*eps*(n^(0.5));
//    t2 = (err < bnd);
//    if (~t2) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t2all = t2all & t2;
//  end
//  t3all = 1;
//  for n=2:4:100
//    a = complex(rand(2*n,n)+i*rand(2*n,n));
//    [q,r] = qr(a,0);
//    err = abs(a-q*r);
//    err = max(err(:));
//    bnd = 10*max(abs(diag(r)))*feps*(n^(0.5));
//    t3 = (err < bnd);
//    if (~t3) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t3all = t3all & t3;
//  end
//  t4all = 1;
//  for n=2:4:100
//    a = dcomplex(rand(2*n,n)+i*rand(2*n,n));
//    [q,r] = qr(a,0);
//    err = abs(a-q*r);
//    err = max(err(:));
//    bnd = 10*max(abs(diag(r)))*eps*(n^(0.5));
//    t4 = (err < bnd);
//    if (~t4) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t4all = t4all & t4;
//  end
//  test_val = t1all & t2all & t3all & t4all;
//@}
//@{ test_qr11.m
//% Test the QR decomposition (compact, pivoting)
//function test_val = test_qr11
//  t1all = 1;
//  for n=2:4:100
//    a = float(rand(2*n,n));
//    [q,r,ec] = qr(a,0);
//    e = zeros(n,n);
//    p = 0:(n-1);
//    mdx = ec + p*n;
//    e(mdx) = 1;
//    err = abs(a-q*r*e');
//    err = max(err(:));
//    bnd = 10*max(abs(diag(r)))*feps*(n^(0.5));
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;
//  end
//  t2all = 1;
//  for n=2:4:100
//    a = double(rand(2*n,n));
//    [q,r,ec] = qr(a,0);
//    e = zeros(n,n);
//    p = 0:(n-1);
//    mdx = ec + p*n;
//    e(mdx) = 1;
//    err = abs(a-q*r*e');
//    err = max(err(:));
//    bnd = 10*max(abs(diag(r)))*eps*(n^(0.5));
//    t2 = (err < bnd);
//    if (~t2) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t2all = t2all & t2;
//  end
//  t3all = 1;
//  for n=2:4:100
//    a = complex(rand(2*n,n)+i*rand(2*n,n));
//    [q,r,ec] = qr(a,0);
//    e = zeros(n,n);
//    p = 0:(n-1);
//    mdx = ec + p*n;
//    e(mdx) = 1;
//    err = abs(a-q*r*e');
//    err = max(err(:));
//    bnd = 10*max(abs(diag(r)))*feps*(n^(0.5));
//    t3 = (err < bnd);
//    if (~t3) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t3all = t3all & t3;
//  end
//  t4all = 1;
//  for n=2:4:100
//    a = dcomplex(rand(2*n,n)+i*rand(2*n,n));
//    [q,r,ec] = qr(a,0);
//    e = zeros(n,n);
//    p = 0:(n-1);
//    mdx = ec + p*n;
//    e(mdx) = 1;
//    err = abs(a-q*r*e');
//    err = max(err(:));
//    bnd = 10*max(abs(diag(r)))*eps*(n^(0.5));
//    t4 = (err < bnd);
//    if (~t4) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t4all = t4all & t4;
//  end
//  test_val = t1all & t2all & t3all & t4all;
//@}
//@{ test_qr12.m
//% Test the QR decomposition (full, pivoting)
//function test_val = test_qr12
//  t1all = 1;
//  for n=2:4:100
//    a = float(rand(n,n));
//    [q,r,e] = qr(a);
//    err = abs(a-q*r*e');
//    err = max(err(:));
//    bnd = 10*max(abs(diag(r)))*feps*(n^(0.5));
//    t1 = (err < bnd);
//    if (~t1) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t1all = t1all & t1;
//  end
//  t2all = 1;
//  for n=2:4:100
//    a = double(rand(n,n));
//    [q,r,e] = qr(a);
//    err = abs(a-q*r*e');
//    err = max(err(:));
//    bnd = 10*max(abs(diag(r)))*eps*(n^(0.5));
//    t2 = (err < bnd);
//    if (~t2) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t2all = t2all & t2;
//  end
//  t3all = 1;
//  for n=2:4:100
//    a = complex(rand(n,n)+i*rand(n,n));
//    [q,r,e] = qr(a);
//    err = abs(a-q*r*e');
//    err = max(err(:));
//    bnd = 10*max(abs(diag(r)))*feps*(n^(0.5));
//    t3 = (err < bnd);
//    if (~t3) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t3all = t3all & t3;
//  end
//  t4all = 1;
//  for n=2:4:100
//    a = dcomplex(rand(n,n)+i*rand(n,n));
//    [q,r,e] = qr(a);
//    err = abs(a-q*r*e');
//    err = max(err(:));
//    bnd = 10*max(abs(diag(r)))*eps*(n^(0.5));
//    t4 = (err < bnd);
//    if (~t4) printf('test failed: er = %e bnd = %e (num %d)\n',err,bnd,n); end;
//    t4all = t4all & t4;
//  end
//  test_val = t1all & t2all & t3all & t4all;
//@}
//!
ArrayVector QRDFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("qr function requires at least one argument - the matrix to decompose.");
  Array A(arg[0]);
  // Test for numeric
  if (A.isReferenceType())
    throw Exception("Cannot apply qr decomposition to reference types.");
  if (!A.is2D())
    throw Exception("Cannot apply matrix operations to N-Dimensional arrays.");
  if (A.anyNotFinite())
    throw Exception("QR Decomposition only defined for matrices with finite entries.");
  bool compactDecomposition = false;
  if (arg.size() == 2) {
    Array cflag(arg[1]);
    int cflag_int = cflag.getContentsAsIntegerScalar();
    if (cflag_int == 0) compactDecomposition = true;
  }
  if (nargout == 3)
    return QRDPivotFunction(compactDecomposition, A);
  else
    return QRDNoPivotFunction(compactDecomposition, A);
}
