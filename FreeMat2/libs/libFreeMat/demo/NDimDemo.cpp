
#include "BasicArray.hpp"
//#include "Array.hpp"
#include "Variant.hpp"
#include "VariantList.hpp"
//#include "Cell.hpp"
//#include "NumericArray.hpp"
//#include "NDimArray.hpp"
//#include "NDimSlice.hpp"
//#include "PermArray.hpp"
//#include "ShapeArray.hpp"
//#include "SparseArray.hpp"
#include <iostream>
#include <QVariant>
#include <QVarLengthArray>
#include <QTime>
//#include "Array.hpp"

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
void printMatrix(const BasicArray<T> &A) {
  for (int i=1;i<=A.dimensions()[0];i++) {
    for (int j=1;j<=A.dimensions()[1];j++) {
      std::cout << A[NTuple(i,j)] << " ";
    }
    std::cout << "\n";
  }
  std::cout << "************************************************************ " << 
    count++ << "\n";
}

template <typename T, typename S>
void printNDim(const T& A, int dim) {
  S q(&A,dim);
  while (q.isValid()) {
    for (int i=0;i<q.size();i++)  {
      std::cout << q.get() << " ";
      q.next();
    }
    q.nextSlice();
    std::cout << "\n";
  } 
  std::cout << "************************************************************ " << 
    count++ << "\n";
}

// template <typename T>
// void printNDim(const BasicArray<T>& A, int dim) {
//   ConstBasicIterator<T> q(&A,dim);
//   while (q.isValid()) {
//     for (int i=0;i<q.size();i++)  {
//       std::cout << q.get() << " ";
//       q.next();
//     }
//     q.nextSlice();
//     std::cout << "\n";
//   } 
//   std::cout << "************************************************************ " << 
//     count++ << "\n";
// }



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
  QTime timer;

  Variant L(DoubleArray,NTuple(5,5));
  BasicArray<double> &L2(L.real<double>());
  for (int i=1;i<=5;i++)
    for (int j=1;j<=5;j++)
      L2[NTuple(i,j)] = i*10+j;
  BasicArray<double> L0(L2);
  std::cout << "Allzeros " << AllZeros(L2) << "\n";
  BasicArray<index_t> row(NTuple(2,1));
  row[1] = 2; row[2] = 3;
  BasicArray<index_t> col(NTuple(3,1));
  col[1] = 1; col[2] = 3; col[3] = 5;
  IndexArrayList ndx;
  ndx.push_back(row); ndx.push_back(col);
  BasicArray<double> L3(L2.getNDimSubset(ndx));
  printMatrix(L2);
  printMatrix(row);
  printMatrix(col);
  printMatrix(L3);
  IndexArrayList ndx2;
  Variant tstring(QString("Hello there peiples"));
  qDebug() << tstring.string();
  BasicArray<index_t> colon(-1);
  ndx2.push_back(row); ndx2.push_back(colon);
  printMatrix(L2);
  BasicArray<double> L4(L2.getNDimSubset(ndx2));
  printMatrix(L4);
  BasicArray<double> L5(L2.getVectorSubset(colon));
  printMatrix(L5);
  IndexArrayList ndx3;
  ndx3.push_back(colon); ndx3.push_back(IndexArray(4));
  BasicArray<double> L6(L2.getNDimSubset(ndx3));
  // slice test!
  printMatrix(L6);
  printMatrix(L2);
  L2.setVectorSubset(col,9);
  printMatrix(L2);
  BasicArray<double> L7(L2);
  L7.setNDimSubset(ndx2,0);
  printMatrix(L7);
  BasicArray<double> L8(L2);
  L8.setNDimSubset(ndx3,1);
  printMatrix(L8);
  IndexArrayList ndx4;
  Variant row2(Index_TArray,NTuple(5,1));
  BasicArray<index_t> &row2_access(row2.real<index_t>());
  row2_access[1] = 1; row2_access[2] = 2;
  row2_access[3] = 3; row2_access[4] = 4;
  row2_access[5] = 5;
  ndx4.push_back(row2.real<index_t>());
  ndx4.push_back(IndexArray(4));
  BasicArray<index_t> L9(L2);
  L9.setNDimSubset(ndx4,1);
  printMatrix(L9);
  Variant delset(DoubleArray,NTuple(4,1));
  BasicArray<index_t> &deldat(delset.real<index_t>());
  deldat[1] = 1; deldat[2] = 3; deldat[3] = 6; deldat[4] = 8;
  BasicArray<index_t> L10(L0);
  BasicArray<index_t> rhs;
  printMatrix(L10);
  L10.setVectorSubset(delset.real<index_t>(),1.5);
  printMatrix(L10);
  L10.setVectorSubset(delset.real<index_t>(),rhs);
  printMatrix(L10);
  BasicArray<index_t> L11(L2);
  printMatrix(L11);
  IndexArrayList ndx5;
  ndx5.push_back(colon); ndx5.push_back(col.real<index_t>());
  L11.setNDimSubset(ndx5,rhs);
  printMatrix(L11);
  // Next - special cases for scalar indexing
  BasicArray<index_t> L12(L2);
  printMatrix(L12);
  for (int i=0;i<7;i++) {
    IndexArrayList args;
    args.push_back(IndexArray(i+1));
    args.push_back(IndexArray(i+1));
    L12.setNDimSubset(args,-4);
  }
  printMatrix(L12);
  Variant K(DoubleArray,NTuple(1000,1000));
  BasicArray<double> &Kref(K.real<double>());
  timer.start();
  for (int i=0;i<1000;i++)
    for (int j=0;j<1000;j++) {
      IndexArrayList args;
      args.push_back(IndexArray(j+1));
      args.push_back(IndexArray(i+1));
      Kref.setNDimSubset(args,(double)(i-j));
    }
  std::cout << "Time for toeplitz set " << timer.elapsed() << "\n";
  for (int i=0;i<1000;i++)
    for (int j=0;j<1000;j++) {
      Kref.setNDimSubset(NTuple(i+1,j+1),(double)(i-j));
    }
  std::cout << "Time for toeplitz (2) set " << timer.elapsed() << "\n";
  return 0;
  // Next step - fast VariantList & slicing.

  class QVectorVariant : public QVarLengthArray<Variant,2> {
  };

  Variant M(433);
  Variant R;
  for (int i=0;i<100000000;i++) {
    //    Variant K(DoubleArray,NTuple(1,1));
    //    K.set(0,M);
    Variant K(4243.0);
    R = Variant(K.realScalar<double>() + 1);
    //    Variant M = K;
    //QList<Variant> T;
    VariantList T;
    //    std::vector<Variant> T;
    T.push_back(K);
    //     Variant *p = new Variant[1];
    //     p[0] = K;
    //     delete[] p;
    //    Variant Q = K;
    //    Variant M[2];
    //    M[0] = K;
  }

  // Let's try something less grandiose for the slicing.
  // The array interface has:
  //   Variant getSubset(Variant index);
  //   Variant getNDimSubset(VariantVector index);
  //   Variant getField(Variant name);
  //   VariantVector getField(Variant name);
  
  // So VariantVector is the next item

  return 0;

  Variant Y(DoubleArray,NTuple(5,5));

  BasicArray<double> &Y2(Y.real<double>());
  for (int i=1;i<=5;i++)
    for (int j=1;j<=5;j++) {
      Y2[NTuple(i,j)] = i*10+j;
    }
  printNDim<BasicArray<double>,ConstBasicIterator<double> >(Y2,0);
  printNDim<BasicArray<double>,ConstBasicIterator<double> >(Y2,1);
  //  printMatrix<double>(Y.constReal<double>());
  //  Y.set(7,Variant(1,3));
  //  printNDim<Variant, ConstVariantIterator>(Y,0);

#if 0
  Variant L(UInt8Array,NTuple(5,5));
  VariantIterator LP(&L,0);
  for (int i=1;i<=5;i++) {
    for (int j=1;j<=5;j++) {
      LP.set(10*(i*10+j));
      LP.next();
    }
    LP.nextSlice();
  }
  printNDim<Variant, ConstVariantIterator>(L,0);

  return 0;
#endif

#if 0

  // Run a sum timing test
  for (int i=0;i<10;i++) {
    Variant Z(DoubleArray,NTuple(1000,1000));
    double sum = 0;
    ConstVariantIterator ZP(&Z,0);
    while (ZP.isValid()) {
      for (int j=0;j<ZP.size();j++)
	sum += ZP.get().realDouble();
      ZP.nextSlice();
    }
  }
  return 0;

  for (int i=0;i<10;i++) {
    Variant Z(DoubleArray,NTuple(1000,1000));
    BasicArray<double> &ZP2(Z.real<double>());
    ConstBasicIterator<double> QQ(&ZP2,0);
    double ssum = 0;
    while (QQ.isValid()) {
      for (int j=0;j<QQ.size();j++)
	ssum += QQ.get();
      QQ.nextSlice();
    }
  }

  //  std::cout << "sum = " << ssum << "\n";
  return 0;

  Variant Y(DoubleArray,NTuple(5,5));

  BasicArray<double> &Y2(Y.real<double>());
  for (int i=1;i<=5;i++)
    for (int j=1;j<=5;j++) {
      Y2[NTuple(i,j)] = i*10+j;
    }
  printNDim<BasicArray<double>,ConstBasicIterator<double> >(Y2,0);
  printNDim<BasicArray<double>,ConstBasicIterator<double> >(Y2,1);
  //  printMatrix<double>(Y.constReal<double>());

  printNDim<Variant, ConstVariantIterator>(Y,0);
  
  for (int k=0;k<20;k++) {
    BasicIterator<double> Z(&Y,0);
    int i=0;
    while (Z.isValid()) {
      for (int j=0;j<Z.size();j++)
	Z[j] = i+1-j;
      Z.nextSlice();
    }
  }
#endif

  //  Variant Q(3.1233);
  Variant V(Y);
  Variant P(V);
  Variant Q(P);

  for (int i=1;i<5;i++) {
    BasicArray<double> &Q2(V.real<double>());
    Q2[NTuple(i,i)] = (double) i;
  }
    
  printMatrix<double>(V.constReal<double>());

  printMatrix<double>(P.constReal<double>());

#if 0  

  BasicArray<double> &Q2(V.real<double>());
  Q2[NTuple(2,2)] = 99;
  printMatrix<double>(V.real<double>());
  printMatrix<double>(Y);
  
  
  Variant P(V);
  Variant Q = P;

  BasicArray<double> Y2(P.real<double>());
  Y2[NTuple(3,3)] = 0;
  printMatrix<double>(Y2);
  printMatrix<double>(Y);


#endif

#if 0
  double *T = new double[1000*1000];
  for (int k=0;k<20;k++) 
  for (int i=0;i<1000;i++)
    for (int j=0;j<1000;j++)
      T[i*1000+j] = i - j + 1;
#endif
  

  return 0;

  BasicArray<double> A(NTuple(6,6));
  for (int i=1;i<=6;i++)
    for (int j=1;j<=6;j++)
      A[NTuple(i,j)] = i*10 + j;

  printMatrix<double>(A);

  printNDim<BasicArray<double>,ConstBasicIterator<double> >(A,1);

#if 0
  Array A(new NumericArray<double>(NTuple(6,6)));
  NumericArray<double> *p = A.asDoubleArray();
  for (int i=1;i<=6;i++)
    for (int j=1;j<=6;j++) 
      p->real()[NTuple(i,j)] = i*10 + j;
  
  printMatrix<double>(p->real());

  RangeSet rset;
  rset[0] = new StepRange(1,2,6);
  rset[1] = new StepRange(1,2,6);
  Array B(new NumericArray<double>(*p,rset));
  
  NumericArray<double> *q = B.asDoubleArray();
  printMatrix<double>(q->real());

  CellArray C(NTuple(1,4));
  C[1] = A;
  C[4] = B;
#endif


//   for (int i=0;i<1000000;i++) {
//     NDimArray<float> B(A);
//   }

//   A[NTuple(1,1)] = 2;
//   A[NTuple(1,3)] = 5;
//   A[NTuple(2,2)] = 4;
//   printMatrix<float>(&A);
  
  //   Array t(3.14);
  //   double p = 0;
  //   for (int i=0;i<1000000;i++) {
  //     Array y = t;
  //     p = p + 1;
  //   }
  //   std::cout << "p = " << p << "\n";
  
  //   Array c(Array::Float, NTuple(3,3));
  //   NumericArray<float>* m = c.asFloatArray();
  //   BaseArray<float> *mp = m->real();
  //   (*mp)[NTuple(1,1)] = 1;
  //   (*mp)[NTuple(2,2)] = 2;
  //   (*mp)[NTuple(3,3)] = 3;
  //   (*mp)[NTuple(3,1)] = 5;
  //   printMatrix<float>(m->constReal());
  
  //   Array c(Array::Cell, NTuple(3,3));
  
  //  BaseArray<Array>* ptr = c.asCellArray();
  //  (*ptr)[NTuple(1,1)] = Array(1.23);
  //  (*ptr)[NTuple(2,2)] = Array(3.52);
  //  (*ptr)[NTuple(3,3)] = Array(4.32);
  
  return 0;

#if 0



  double accum = 0;
  for (int j=0;j<1000*1000;j++)
    accum += T[j].toDouble();

  return 0;

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
