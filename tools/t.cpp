
#include <iostream>
#include <string.h>
#include "MemPtr.hpp"

#define MAX(a,b) (((a) > (b)) ? (a) : (b))
extern "C"
{
  void zheev_(char *JOBZ, char *UPLO, int *N, double *A, int *LDA, 
	      double *W, double *WORK, int *LWORK, double *RWORK, int *INFO, int len);
}

void toogo() {
  double *a = new double[8];
  memset(a,0,sizeof(double)*8);
  a[0] = 1;
  a[2] = 1;
  a[3] = -1;
  a[4] = 1;
  a[5] = 1;
  a[6] = 1;
  char JOBZ = 'N';
  char UPLO = 'U';
  int n = 2;
  int N = n;
  int LDA = n;
  MemBlock<double> RWORK(MAX(1,3*N-2));
  int LWORK;
  int INFO;
  double WORKSZE[2];
  LWORK = -1;
  double *d = new double[4];
  zheev_(&JOBZ,&UPLO,&N,a,&LDA,d,WORKSZE,&LWORK,&RWORK,&INFO,1);
  LWORK = (int) WORKSZE[0];
  std::cout << "LWORK = " << LWORK << "\n";
  MemBlock<double> WORK(2*LWORK);
  zheev_(&JOBZ,&UPLO,&N,a,&LDA,d,&WORK,&LWORK,&RWORK,&INFO,1);
  for (int i=0;i<2;i++) 
    std::cout << "eig[" << i << "] = " << d[i] << "\n";
}

int main(int argc, char*argv[]) {
  toogo();
  return 0;
}
