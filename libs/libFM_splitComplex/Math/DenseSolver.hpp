#ifndef __DenseSolver_hpp__
#define __DenseSolver_hpp__


namespace FM
{
  class TermIF;

  template <class T>
  void DenseSolve(int m, int n, int k, T *c, const T *a, const T *b, TermIF *io);

  template <class T>
  void DenseSolveComplex(int m, int n, int k, T *c, const T *a, const T *b, TermIF *io);
}


#endif
