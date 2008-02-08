#include "NDimArray.hpp"
#include <iostream>

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

  ConstNDimIterator<float> q(&A,1);
  while (q.isValid()) {
    for (int i=1;i<=q.lineSize();i++) 
      std::cout << q[i] << " ";
    std::cout << "\n";
    q.nextSlice();
  } 

  std::cout << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&1\n";
  
  ConstNDimIterator<float> p(&A,0);
  while (p.isValid()) {
    for (int i=1;i<=p.lineSize();i++) 
      std::cout << p[i] << " ";
    std::cout << "\n";
    p.nextSlice();
  } 

  std::cout << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&2\n";

  ConstNDimIterator<float> r(&A,2);
  while (r.isValid()) {
    for (int i=1;i<=r.lineSize();i++) 
      std::cout << r[i] << " ";
    std::cout << "\n";
    r.nextSlice();
  } 

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

  NDimArray<float> C(B.resize(NTuple(5,6)));
  
  for (int i=1;i<=5;i++) {
    for (int j=1;j<=6;j++)
      std::cout << C[NTuple(i,j)] << " ";
    std::cout << "\n";
  }

  std::cout << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&6\n";

  NDimArray<float> H(B.resize(NTuple(2,3)));
  
  for (int i=1;i<=2;i++) {
    for (int j=1;j<=3;j++)
      std::cout << H[NTuple(i,j)] << " ";
    std::cout << "\n";
  }


  return 0;
}
