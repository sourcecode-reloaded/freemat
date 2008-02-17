#include "NDimArray.hpp"
#include "NDimSlice.hpp"
#include "PermArray.hpp"
#include "ShapeArray.hpp"
#include "SparseArray.hpp"
#include <iostream>

// The next issue to solve is the dynamic typing one.
// Suppose we want to write a function to take the FFT of an array
#if 0
Array DoFFT<T>(const NumericalArray<T> &t, int dimension) {
  ConstNDimIterator<T> *p = t.getConstNDimIterator(dimension);
  NumericalArray<T> ret;
  
  while (p->valid()) {
    for (int i=0;i<p->count();i++)
      
  }
}

Array DoFFTFunction(const Array &t) {
  if (IsFloatArray(t))
    return DoFFT<float>(t);
  else if (IsDoubleArray(t))
    return DoFFT<double>(t);
}

#endif

static int count = 0;

template <typename T>
void printMatrix(const BaseArray<T>& A) {
  for (int i=1;i<=A.dimensions()[0];i++) {
    for (int j=1;j<=A.dimensions()[1];j++) {
      std::cout << A[NTuple(i,j)] << " ";
    }
    std::cout << "\n";
  }
  std::cout << "************************************************************ " << 
    count++ << "\n";
}

template <typename T>
void printNDim(const BaseArray<T>& A, int dim) {
  ConstNDimIterator<T> q(&A,dim);
  while (q.isValid()) {
    for (int i=1;i<=q.size();i++)  {
      std::cout << q[i] << " ";
    }
    q.nextSlice();
    std::cout << "\n";
  } 
  std::cout << "************************************************************ " << 
    count++ << "\n";
}

int main(int, const char *[]) {
  #if 0
//   float *t = new float[1000*1000];
//   for (int i=0;i<1000;i++)
//     for (int j=0;j<1000;j++)
//       t[i*1000+j] = i-j;

//   return 0;


//  NDimArray<float> T(NTuple(1000,1000));

//   for (int i=1;i<=1000;i++)
//     for (int j=1;j<=1000;j++)
//       T[NTuple(i,j)] = i-j;

//   NDimIterator<float> p(&T,0);
//   int i=0;
//   while (p.isValid()) {
//     for (int j=1;j<=p.size();j++) 
//       p[j] = i+1-j;
//     p.nextSlice();
//   }
//   return 0;

  NTuple lims(3,4,5);
  lims[3] = 6;
  // Loop through 
  NTuple pd(1,1,1);
  while (pd <= lims) {
    std::cout << pd << "\n";
    lims.increment(pd);
  }
  
  std::cout << "******************************************************\n";

  // Loop through 
  pd = NTuple(1,1,1);
  while (pd <= lims) {
    std::cout << pd << "\n";
    lims.increment(pd,1);
  }

  std::cout << "******************************************************\n";
  
  NDimArray<float> P(NTuple(3,4));
  for (int i=1;i<=12;i++)
    P[i] = i;
  printMatrix(P);

  printNDim(P,1);  

  printNDim(P,0);  

  const int N = 4;
  NDimArray<float> A;//(NTuple(N,N,N));
  A.resize(N*N*N);
  A.reshape(NTuple(N,N,N));
  //  NDimArray<float> B(A);
  // Initialize the array
  for (int i=1;i<=N;i++)
    for (int j=1;j<=N;j++)
      for (int k=1;k<=N;k++)
	A[NTuple(i,j,k)] = (float) (k*100+i*10+j);

  printNDim(A,1);

  printNDim(A,0);

  printNDim(A,2);

  // Print the array
  for (int i=1;i<=N;i++)  {
    for (int j=1;j<=N;j++)
      std::cout << A[NTuple(i,j)] << " ";
    std::cout << "\n";
  }

  NDimArray<float> B(NTuple(3,4));
  for (int i=1;i<=3;i++)
    for (int j=1;j<=4;j++)
      B[NTuple(i,j)] = (float) 10*i + j;

  for (int i=1;i<=3;i++) {
    for (int j=1;j<=4;j++)
      std::cout << B[NTuple(i,j)] << " ";
    std::cout << "\n";
  }

  NDimArray<float> C(B);
  C.resize(NTuple(5,6));
  
  for (int i=1;i<=5;i++) {
    for (int j=1;j<=6;j++)
      std::cout << C[NTuple(i,j)] << " ";
    std::cout << "\n";
  }

  NDimArray<float> H(B);
  H.resize(NTuple(2,3));
  
  for (int i=1;i<=2;i++) {
    for (int j=1;j<=3;j++)
      std::cout << H[NTuple(i,j)] << " ";
    std::cout << "\n";
  }

  NDimArray<float> I;
  I.resize(NTuple(11,15));
  for (int i=1;i<=11;i++)
    for (int j=1;j<=15;j++)
      I[NTuple(i,j)] = ((float) i) + ((float) j)/100.0;
  printMatrix<float>(I);

  RangeSet rset;
  rset.set(0,new StepRange(1,2,5));
  rset.set(1,new StepRange(1,2,9));
  NDimSlice<float> J(I,rset);
  std::cout << " slice size " << J.dimensions() << "\n";
  printNDim(J,0);

  J[NTuple(2,2)] = 0.4;
  printNDim(J,1);

  printNDim(J,1);

  printNDim(I,1);

  PermArray<float> K(J,NPerm(2,1));
  printMatrix<float>(K);
  
  ShapeArray<float> M(K,NTuple(6,2));
  printMatrix<float>(M);

  NDimArray<float> MR(resize(M,NTuple(6,3)));
  printMatrix<float>(MR);

  SparseMatrix<float> S(QVector<index_t>() << 1 << 2 << 5 << 6,
			QVector<index_t>() << 1 << 1 << 2 << 5,
			QVector<float>() << 4 << 3 << 1 << 7);
  S[NTuple(3,3)] = 5;
  printMatrix<float>(S);

#endif

  NDimArray<float> B(NTuple(1000,1000));

  for (int k=1;k<=100;k++) {
    ConstNDimIterator<float> *i = B.getConstNDimIterator(0);
    float accum = 0;
    while (i->isValid()) {
      for (int j=0;j<i->size();j++) {
	accum += i->get();
	i->advance();
      }
      i->nextSlice();
    }
    delete i;
  }
  return 0;
  //  B.resize(NTuple(1000,1000));

//   SparseMatrix<float> Z(QVector<index_t>() << 1 << 1000,
// 			QVector<index_t>() << 1 << 1000,
// 			QVector<float>() << 1 << 1);
  
#if 0
  float accum = 0;
  for (int k=0;k<100;k++) {
    for (int i=1;i<=1000*1000;i++)
      accum += B[i];
  }
  return 0;

  for (int i=1;i<=1000;i++)
    for (int j=1;j<=1000;j++)
      //      accum += Z[NTuple(j,i)];
      accum += B[NTuple(j,i)];
  
  return 0;
#endif
}
