#include "NDimArray.hpp"
#include "NDimSlice.hpp"
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
template <typename T>
void printMatrix(const BaseArray<T>& A) {
  for (int i=1;i<=A.dimensions()[0];i++) {
    for (int j=1;j<=A.dimensions()[1];j++) {
      std::cout << A[NTuple(i,j)] << " ";
    }
    std::cout << "\n";
  }
}

template <typename T>
void printNDim(T* q) {
  while (q->isValid()) {
    for (int i=0;i<q->size();i++)  {
      std::cout << q->get() << " ";
      q->next();
    }
    std::cout << "\n";
    q->nextSlice();
  } 
}

int main(int, const char *[]) {
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

  ConstNDimIterator<float> *q = A.getConstNDimIterator(1);
  printNDim(q);

  std::cout << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&1\n";
  
  ConstNDimIterator<float> *p = A.getConstNDimIterator(0);
  printNDim(p);

  std::cout << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&2\n";

  ConstNDimIterator<float> *r = A.getConstNDimIterator(2);
  printNDim(r);

  std::cout << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&3\n";

  //A.set(NTuple(i,j),(float) (i*10+j));

  for (int i=0;i<1000000;i++) {
    NDimArray<float> B(A);
  }

  // Print the array
  for (int i=1;i<=N;i++)  {
    for (int j=1;j<=N;j++)
      std::cout << A[NTuple(i,j)] << " ";
    std::cout << "\n";
  }

  std::cout << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&4\n";

  NDimArray<float> B(NTuple(3,4));
  for (int i=1;i<=3;i++)
    for (int j=1;j<=4;j++)
      B[NTuple(i,j)] = (float) 10*i + j;

  for (int i=1;i<=3;i++) {
    for (int j=1;j<=4;j++)
      std::cout << B[NTuple(i,j)] << " ";
    std::cout << "\n";
  }

  std::cout << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&5\n";

  NDimArray<float> C(B);
  C.resize(NTuple(5,6));
  
  for (int i=1;i<=5;i++) {
    for (int j=1;j<=6;j++)
      std::cout << C[NTuple(i,j)] << " ";
    std::cout << "\n";
  }

  std::cout << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&6\n";

  NDimArray<float> H(B);
  H.resize(NTuple(2,3));
  
  for (int i=1;i<=2;i++) {
    for (int j=1;j<=3;j++)
      std::cout << H[NTuple(i,j)] << " ";
    std::cout << "\n";
  }

  std::cout << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&6\n";

  NDimArray<float> I;
  I.resize(NTuple(11,15));
  for (int i=1;i<11;i++)
    for (int j=1;j<15;j++)
      I[NTuple(i,j)] = ((float) i) + ((float) j)/100.0;
  printMatrix<float>(I);

  RangeSet rset(NTuple(1,1),
		NTuple(2,2),
		NTuple(5,9));
  NDimSlice<float> J(I,rset);
  std::cout << " slice size " << J.dimensions() << "\n";
  //  printMatrix<float>(J);
  printNDim(J.getConstNDimIterator(0));

  return 0;
}
